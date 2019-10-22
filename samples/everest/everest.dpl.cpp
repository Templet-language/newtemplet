/*$TET$actor*/
#include <templet.hpp>
/*$TET$*/

using namespace TEMPLET;

struct my_engine : engine{
	my_engine(int argc, char *argv[]){
		::init(this, argc, argv);
	}
	void run(){ TEMPLET::run(this); }
	void map(){ TEMPLET::map(this); }
};

#pragma templet ~mes=

struct mes : message{
	mes(actor*a, engine*e, int t) : _where(CLI), _cli(a), _client_id(t){
		::init(this, a, e);
	}

	void send(){
		if (_where == CLI){ TEMPLET::send(this, _srv, _server_id); _where = SRV; }
		else if (_where == SRV){ TEMPLET::send(this, _cli, _client_id); _where = CLI; }
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
	enum tag{START,TAG_p};

	ping(my_engine&e):p(this, &e, TAG_p){
		::init(this, &e, ping_recv_adapter);
		::init(&_start, this, &e);
		::send(&_start, this, START);
/*$TET$ping$ping*/
/*$TET$*/
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
/*$TET$*/
	}

	void p_handler(mes&m){
/*$TET$ping$p*/
/*$TET$*/
	}

/*$TET$ping$$code&data*/
/*$TET$*/
	message _start;
};

#pragma templet *pong(p?mes,tsk.task)

struct pong : actor{
	enum tag{START,TAG_p,TAG_tsk};

	pong(my_engine&e):tsk(*(e._teng)){
		::init(this, &e, pong_recv_adapter);
		tsk.set_on_ready([&]() { tsk_handler(tsk); resume(); });
/*$TET$pong$pong*/
/*$TET$*/
	}

	bool access(message*m){ return TEMPLET::access(m, this); }
	bool access(message&m){ return TEMPLET::access(&m, this); }

	void at(int _at){ TEMPLET::at(this, _at); }
	void delay(double t){ TEMPLET::delay(this, t); }
	double time(){ return TEMPLET::time(this); }
	void stop(){ TEMPLET::stop(this); }

	void p(mes&m){m._server_id=TAG_p; m._srv=this;}
	task tsk;
	void tsk_submit() { tsk.submit(); suspend(); };

	static void pong_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_p: ((pong*)a)->p_handler(*((mes*)m)); break;
		}
	}

	void p_handler(mes&m){
/*$TET$pong$p*/
/*$TET$*/
	}

	void tsk_handler(task&m){
/*$TET$pong$tsk*/
/*$TET$*/
	}

/*$TET$pong$$code&data*/
/*$TET$*/
};

int main(int argc, char *argv[])
{
	my_engine e(argc, argv);
/*$TET$footer*/
/*$TET$*/
}
