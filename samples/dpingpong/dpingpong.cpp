/*$TET$actor*/

//#define DISTRIBUTED_EXECUTION
#define DEBUG_SERIALIZATION
#define DEBUG_EXECUTION
#include "templet.hpp"

#include <iostream>

#define STR_BUF 100
using namespace std;
/*$TET$*/

using namespace TEMPLET;

struct my_engine : engine{
	my_engine(int argc, char *argv[]){
		::init(this, argc, argv);
	}
	void run(){ TEMPLET::run(this); }
	void map(){ TEMPLET::map(this); }
};

#pragma templet ~mes$=

struct mes : message{
	mes(actor*a, engine*e, int t) : _where(CLI), _cli(a), _client_id(t){
		::init(this, a, e, mes_save_adapter, mes_restore_adapter);
	}

	static void mes_save_adapter(message*m, saver*s){
		((mes*)m)->save(s);
	}

	static void mes_restore_adapter(message*m, restorer*r){
		((mes*)m)->restore(r);
	}

	void send(){
		if (_where == CLI){ TEMPLET::send(this, _srv, _server_id); _where = SRV; }
		else if (_where == SRV){ TEMPLET::send(this, _cli, _client_id); _where = CLI; }
	}

	void save(saver*s){
/*$TET$mes$$save*/
        int size = _mes.size();
        ::save(s, &size, sizeof(int));
		::save(s, _mes.c_str(), size);
/*$TET$*/
	}

	void restore(restorer*r){
/*$TET$mes$$restore*/
        int size;
        char buf[STR_BUF];
		::restore(r, &size, sizeof(int));
		::restore(r, &buf, size);
		buf[size] = '\0';
		_mes.clear();
		_mes.append(buf);
		
/*$TET$*/
	}

/*$TET$mes$$data*/
	string _mes;
/*$TET$*/

	enum { CLI, SRV } _where;
	actor* _srv;
	actor* _cli;
	int _client_id;
	int _server_id;
};

#pragma templet *ping$(p!mes)+

struct ping : actor{
	enum tag{START,TAG_p};

	ping(my_engine&e):p(this, &e, TAG_p){
		::init(this, &e, ping_recv_adapter, ping_save_adapter, ping_restore_adapter);
		::init(&_start, this, &e);
		::send(&_start, this, START);
/*$TET$ping$ping*/
/*$TET$*/
	}

	static void ping_save_adapter(actor*a, saver*s){
		((ping*)a)->save(s);
	}

	static void ping_restore_adapter(actor*a, restorer*r){
		((ping*)a)->restore(r);
	}

	bool access(message*m){ return TEMPLET::access(m, this); }
	bool access(message&m){ return TEMPLET::access(&m, this); }

	void at(int _at){ TEMPLET::at(this, _at); }
	void delay(double t){ TEMPLET::delay(this, t); }
	double time(){ return TEMPLET::time(this); }
	void stop(){ TEMPLET::stop(this); }

	mes p;

	static void ping_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_p: ((ping*)a)->p_handler(*((mes*)m)); break;
			case START: ((ping*)a)->start(); break;
		}
	}

	void start(){
/*$TET$ping$start*/
		p._mes = "Hello PONG!!!";
		p.send();
/*$TET$*/
	}

	void p_handler(mes&m){
/*$TET$ping$p*/
		cout << m._mes.c_str();
		stop();
/*$TET$*/
	}

	void save(saver*s){
/*$TET$ping$$save*/
        	int x=42;
		TEMPLET::save(s, &x, sizeof(x));
/*$TET$*/
	}

	void restore(restorer*r){
/*$TET$ping$$restore*/
		int x;
		TEMPLET::restore(r, &x, sizeof(x));
		cout << " ping code=" << x;
/*$TET$*/
	}

/*$TET$ping$$code&data*/
/*$TET$*/
	message _start;
};

#pragma templet *pong$(p?mes)

struct pong : actor{
	enum tag{START,TAG_p};

	pong(my_engine&e){
		::init(this, &e, pong_recv_adapter, pong_save_adapter, pong_restore_adapter);
/*$TET$pong$pong*/
/*$TET$*/
	}

	static void pong_save_adapter(actor*a, saver*s){
		((pong*)a)->save(s);
	}

	static void pong_restore_adapter(actor*a, restorer*r){
		((pong*)a)->restore(r);
	}

	bool access(message*m){ return TEMPLET::access(m, this); }
	bool access(message&m){ return TEMPLET::access(&m, this); }

	void at(int _at){ TEMPLET::at(this, _at); }
	void delay(double t){ TEMPLET::delay(this, t); }
	double time(){ return TEMPLET::time(this); }
	void stop(){ TEMPLET::stop(this); }

	void p(mes&m){m._server_id=TAG_p; m._srv=this;}

	static void pong_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_p: ((pong*)a)->p_handler(*((mes*)m)); break;
		}
	}

	void p_handler(mes&m){
/*$TET$pong$p*/
		cout << m._mes.c_str() << endl;
		m._mes = "Hello PING!!!";
		m.send();
/*$TET$*/
	}

	void save(saver*s){
/*$TET$pong$$save*/
        int x=42;
		TEMPLET::save(s, &x, sizeof(x));
/*$TET$*/
	}

	void restore(restorer*r){
/*$TET$pong$$restore*/
		int x;
		TEMPLET::restore(r, &x, sizeof(x));
		cout << " pong code=" << x;
/*$TET$*/
	}

/*$TET$pong$$code&data*/
/*$TET$*/
};

int main(int argc, char *argv[])
{
	my_engine e(argc, argv);
/*$TET$footer*/
	ping a_ping(e);
	pong a_pong(e);
    
    a_ping.at(1);
    a_pong.at(1);
    
	a_pong.p(a_ping.p);
	
	e.map();

	e.run();
/*$TET$*/
}
