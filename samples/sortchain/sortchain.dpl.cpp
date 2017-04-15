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

enum MESSAGE_TAGS{ START };

#pragma templet ~mes=

struct mes : message{
	mes(actor*a, engine*e, int t) : _where(CLI), _cli(a), _client_id(t){
		::init(this, a, e);
	}

	bool access(actor*a){
		return TEMPLET::access(this, a);
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

#pragma templet *producer(out!mes)+

struct producer : actor{
	enum tag{TAG_out=START+1};

	producer(my_engine&e):_out(this, &e, TAG_out){
		::init(this, &e, producer_recv_adapter);
		::init(&_start, this, &e);
		::send(&_start, this, START);
/*$TET$producer$producer*/
/*$TET$*/
	}

	void at(int _at){ TEMPLET::at(this, _at); }
	void delay(double t){ TEMPLET::delay(this, t); }
	double time(){ return TEMPLET::time(this); }
	void stop(){ TEMPLET::stop(this); }

	mes* out(){return &_out;}

	static void producer_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_out: ((producer*)a)->out(*((mes*)m)); break;
			case START: ((producer*)a)->start(); break;
		}
	}

	void start(){
/*$TET$producer$start*/
/*$TET$*/
	}

	void out(mes&m){
/*$TET$producer$out*/
/*$TET$*/
	}

/*$TET$producer$$code&data*/
/*$TET$*/

	mes _out;
	message _start;
};

#pragma templet *sorter(in?mes,out!mes)

struct sorter : actor{
	enum tag{TAG_in,TAG_out};

	sorter(my_engine&e):_out(this, &e, TAG_out){
		::init(this, &e, sorter_recv_adapter);
/*$TET$sorter$sorter*/
/*$TET$*/
	}

	void at(int _at){ TEMPLET::at(this, _at); }
	void delay(double t){ TEMPLET::delay(this, t); }
	double time(){ return TEMPLET::time(this); }
	void stop(){ TEMPLET::stop(this); }

	void in(mes*m){m->_server_id=TAG_in; m->_srv=this;}
	mes* out(){return &_out;}

	static void sorter_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_in: ((sorter*)a)->in(*((mes*)m)); break;
			case TAG_out: ((sorter*)a)->out(*((mes*)m)); break;
		}
	}

	void in(mes&m){
/*$TET$sorter$in*/
/*$TET$*/
	}

	void out(mes&m){
/*$TET$sorter$out*/
/*$TET$*/
	}

/*$TET$sorter$$code&data*/
/*$TET$*/

	mes _out;
};

#pragma templet *stoper(in?mes)

struct stoper : actor{
	enum tag{TAG_in};

	stoper(my_engine&e){
		::init(this, &e, stoper_recv_adapter);
/*$TET$stoper$stoper*/
/*$TET$*/
	}

	void at(int _at){ TEMPLET::at(this, _at); }
	void delay(double t){ TEMPLET::delay(this, t); }
	double time(){ return TEMPLET::time(this); }
	void stop(){ TEMPLET::stop(this); }

	void in(mes*m){m->_server_id=TAG_in; m->_srv=this;}

	static void stoper_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_in: ((stoper*)a)->in(*((mes*)m)); break;
		}
	}

	void in(mes&m){
/*$TET$stoper$in*/
/*$TET$*/
	}

/*$TET$stoper$$code&data*/
/*$TET$*/

};

/*$TET$footer*/
/*$TET$*/
