/*$TET$header*/
#include <templet.hpp>
/*$TET$*/

using namespace TEMPLET;

struct my_engine : engine{
	my_engine(int argc, char *argv[]){
		::init(this, argc, argv);
	}
	void run(){ ::run(this); }
	void map(){ ::map(this); }
};

enum MES_TAGS{ START };

#pragma templet ~mes=

struct mes : message{
	mes(actor*a, engine*e, int t) : _where(CLI), _cli(a), _client_id(t){
		::init(this, e);
	}

	bool access(actor*a){
		return ::access(this, a);
	}

	void send(){
		if (_where == CLI){ ::send(this, _srv, _server_id); _where = SRV; }
		else if (_where == SRV){ ::send(this, _cli, _client_id); _where = CLI; }
	}

/*$TET$mes$$data*/
/*$TET$*/

	enum { CLI, SRV } _where;
	actor* _srv;
	actor* _cli;
	int _client_id;
	int _server_id;
};

#pragma templet *ping(p!mes)+

struct ping : actor{
	enum tag{TAG_p=MES_TAGS::START+1};

	ping(my_engine&e):_p(this, &e, TAG_p){
		::init(this, &e, ping_recv_adapter);
		::init(&_start, &e);
		::send(&_start, this, MES_TAGS::START);
/*$TET$ping$ping*/
/*$TET$*/
	}

	void at(int _at){ ::at(this, _at); }
	void delay(double t){ ::delay(this, t); }
	void stop(){ ::stop(this); }

	mes* p(){return &_p;}

	friend void ping_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_p: ((ping*)a)->p(*((mes*)m)); break;
			case MES_TAGS::START: ((ping*)a)->start(); break;
		}
	}

	void start(){
/*$TET$ping$start*/
/*$TET$*/
	}

	void p(mes&m){
/*$TET$ping$p*/
/*$TET$*/
	}

/*$TET$ping$$code&data*/
/*$TET$*/

	mes _p;
	message _start;
};

#pragma templet *pong(p?mes)

struct pong : actor{
	enum tag{TAG_p};

	pong(my_engine&e){
		::init(this, &e, pong_recv_adapter);
/*$TET$pong$pong*/
/*$TET$*/
	}

	void at(int _at){ ::at(this, _at); }
	void delay(double t){ ::delay(this, t); }
	void stop(){ ::stop(this); }

	void p(mes*m){m->_server_id=TAG_p; m->_srv=this;}

	friend void pong_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_p: ((pong*)a)->p(*((mes*)m)); break;
		}
	}

	void p(mes&m){
/*$TET$pong$p*/
/*$TET$*/
	}

/*$TET$pong$$code&data*/
/*$TET$*/

};

/*$TET$footer*/
/*$TET$*/
