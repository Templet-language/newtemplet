/*$TET$actor*/
#include <templet.hpp>
/*$TET$*/

using namespace TEMPLET;

struct my_engine : engine{
	my_engine(int argc, char *argv[]){
		::init(this, argc, argv);
	}
	bool run(){ return TEMPLET::run(this); }
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

#pragma templet *sorter(p!mes,t.tasksort)+

struct sorter : actor{
	enum tag{START,TAG_p,TAG_t};

	sorter(my_engine&e):p(this, &e, TAG_p),t(*(e._teng)){
		TEMPLET::init(this, &e, sorter_recv_adapter);
		TEMPLET::init(&_start, this, &e);
		TEMPLET::send(&_start, this, START);
		t.set_on_ready([&]() { t_handler(t); resume(); });
/*$TET$sorter$sorter*/
/*$TET$*/
	}

	bool access(message*m){ return TEMPLET::access(m, this); }
	bool access(message&m){ return TEMPLET::access(&m, this); }

	void at(int _at){ TEMPLET::at(this, _at); }
	void delay(double t){ TEMPLET::delay(this, t); }
	double time(){ return TEMPLET::time(this); }
	void stop(){ TEMPLET::stop(this); }

	mes p;
	tasksort t;
	void t_submit() { t.submit(); suspend(); };

	static void sorter_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_p: ((sorter*)a)->p_handler(*((mes*)m)); break;
			case START: ((sorter*)a)->start(); break;
		}
	}

	void start(){
/*$TET$sorter$start*/
/*$TET$*/
	}

	void p_handler(mes&m){
/*$TET$sorter$p*/
/*$TET$*/
	}

	void t_handler(tasksort&t){
/*$TET$sorter$t*/
/*$TET$*/
	}

/*$TET$sorter$$code&data*/
/*$TET$*/
	message _start;
};

#pragma templet *merger(p1?mes,p2?mes,t.taskmerge)

struct merger : actor{
	enum tag{START,TAG_p1,TAG_p2,TAG_t};

	merger(my_engine&e):t(*(e._teng)){
		TEMPLET::init(this, &e, merger_recv_adapter);
		t.set_on_ready([&]() { t_handler(t); resume(); });
/*$TET$merger$merger*/
/*$TET$*/
	}

	bool access(message*m){ return TEMPLET::access(m, this); }
	bool access(message&m){ return TEMPLET::access(&m, this); }

	void at(int _at){ TEMPLET::at(this, _at); }
	void delay(double t){ TEMPLET::delay(this, t); }
	double time(){ return TEMPLET::time(this); }
	void stop(){ TEMPLET::stop(this); }

	void p1(mes&m){m._server_id=TAG_p1; m._srv=this;}
	void p2(mes&m){m._server_id=TAG_p2; m._srv=this;}
	taskmerge t;
	void t_submit() { t.submit(); suspend(); };

	static void merger_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_p1: ((merger*)a)->p1_handler(*((mes*)m)); break;
			case TAG_p2: ((merger*)a)->p2_handler(*((mes*)m)); break;
		}
	}

	void p1_handler(mes&m){
/*$TET$merger$p1*/
/*$TET$*/
	}

	void p2_handler(mes&m){
/*$TET$merger$p2*/
/*$TET$*/
	}

	void t_handler(taskmerge&t){
/*$TET$merger$t*/
/*$TET$*/
	}

/*$TET$merger$$code&data*/
/*$TET$*/
};

/*$TET$code&data*/
/*$TET$*/

int main(int argc, char *argv[])
{
	my_engine e(argc, argv);
/*$TET$footer*/
/*$TET$*/
}
