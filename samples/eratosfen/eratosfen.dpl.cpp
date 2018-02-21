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

#pragma templet *seeder(p!mes)+

struct seeder : actor{
	enum tag{START,TAG_p};

	seeder(my_engine&e):p(this, &e, TAG_p){
		::init(this, &e, seeder_recv_adapter);
		::init(&_start, this, &e);
		::send(&_start, this, START);
/*$TET$seeder$seeder*/
/*$TET$*/
	}

	bool access(message*m){ return TEMPLET::access(m, this); }
	bool access(message&m){ return TEMPLET::access(&m, this); }

	void at(int _at){ TEMPLET::at(this, _at); }
	void delay(double t){ TEMPLET::delay(this, t); }
	double time(){ return TEMPLET::time(this); }
	void stop(){ TEMPLET::stop(this); }

	mes p;

	static void seeder_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_p: ((seeder*)a)->p_handler(*((mes*)m)); break;
			case START: ((seeder*)a)->start(); break;
		}
	}

	void start(){
/*$TET$seeder$start*/
/*$TET$*/
	}

	void p_handler(mes&m){
/*$TET$seeder$p*/
/*$TET$*/
	}

/*$TET$seeder$$code&data*/
/*$TET$*/
	message _start;
};

#pragma templet *filter(in?mes,out!mes)

struct filter : actor{
	enum tag{START,TAG_in,TAG_out};

	filter(my_engine&e):out(this, &e, TAG_out){
		::init(this, &e, filter_recv_adapter);
/*$TET$filter$filter*/
/*$TET$*/
	}

	bool access(message*m){ return TEMPLET::access(m, this); }
	bool access(message&m){ return TEMPLET::access(&m, this); }

	void at(int _at){ TEMPLET::at(this, _at); }
	void delay(double t){ TEMPLET::delay(this, t); }
	double time(){ return TEMPLET::time(this); }
	void stop(){ TEMPLET::stop(this); }

	void in(mes&m){m._server_id=TAG_in; m._srv=this;}
	mes out;

	static void filter_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_in: ((filter*)a)->in_handler(*((mes*)m)); break;
			case TAG_out: ((filter*)a)->out_handler(*((mes*)m)); break;
		}
	}

	void in_handler(mes&m){
/*$TET$filter$in*/
/*$TET$*/
	}

	void out_handler(mes&m){
/*$TET$filter$out*/
/*$TET$*/
	}

/*$TET$filter$$code&data*/
/*$TET$*/
};

#pragma templet *stopper(p?mes)

struct stopper : actor{
	enum tag{START,TAG_p};

	stopper(my_engine&e){
		::init(this, &e, stopper_recv_adapter);
/*$TET$stopper$stopper*/
/*$TET$*/
	}

	bool access(message*m){ return TEMPLET::access(m, this); }
	bool access(message&m){ return TEMPLET::access(&m, this); }

	void at(int _at){ TEMPLET::at(this, _at); }
	void delay(double t){ TEMPLET::delay(this, t); }
	double time(){ return TEMPLET::time(this); }
	void stop(){ TEMPLET::stop(this); }

	void p(mes&m){m._server_id=TAG_p; m._srv=this;}

	static void stopper_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_p: ((stopper*)a)->p_handler(*((mes*)m)); break;
		}
	}

	void p_handler(mes&m){
/*$TET$stopper$p*/
/*$TET$*/
	}

/*$TET$stopper$$code&data*/
/*$TET$*/
};

int main(int argc, char *argv[])
{
	my_engine e(argc, argv);
/*$TET$footer*/
/*$TET$*/
}
