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

#pragma templet ~value_message$=

struct value_message : message{
	value_message(actor*a, engine*e, int t) : _where(CLI), _cli(a), _client_id(t){
		::init(this, a, e, value_message_save_adapter, value_message_restore_adapter);
	}

	static void value_message_save_adapter(message*m, saver*s){
		((value_message*)m)->save(s);
	}

	static void value_message_restore_adapter(message*m, restorer*r){
		((value_message*)m)->restore(r);
	}

	void send(){
		if (_where == CLI){ TEMPLET::send(this, _srv, _server_id); _where = SRV; }
		else if (_where == SRV){ TEMPLET::send(this, _cli, _client_id); _where = CLI; }
	}

	void save(saver*s){
/*$TET$value_message$$save*/
		//TEMPLET::save(s, &x, sizeof(x));
/*$TET$*/
	}

	void restore(restorer*r){
/*$TET$value_message$$restore*/
		//TEMPLET::restore(r, &x, sizeof(x));
/*$TET$*/
	}

/*$TET$value_message$$data*/
/*$TET$*/

	enum { CLI, SRV } _where;
	actor* _srv;
	actor* _cli;
	int _client_id;
	int _server_id;
};

#pragma templet *master$(sin2_port!value_message,cos2_port!value_message)+

struct master : actor{
	enum tag{START,TAG_sin2_port,TAG_cos2_port};

	master(my_engine&e):sin2_port(this, &e, TAG_sin2_port),cos2_port(this, &e, TAG_cos2_port){
		::init(this, &e, master_recv_adapter, master_save_adapter, master_restore_adapter);
		::init(&_start, this, &e);
		::send(&_start, this, START);
/*$TET$master$master*/
/*$TET$*/
	}

	static void master_save_adapter(actor*a, saver*s){
		((master*)a)->save(s);
	}

	static void master_restore_adapter(actor*a, restorer*r){
		((master*)a)->restore(r);
	}

	bool access(message*m){ return TEMPLET::access(m, this); }

	void at(int _at){ TEMPLET::at(this, _at); }
	void delay(double t){ TEMPLET::delay(this, t); }
	double time(){ return TEMPLET::time(this); }
	void stop(){ TEMPLET::stop(this); }

	value_message sin2_port;
	value_message cos2_port;

	static void master_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_sin2_port: ((master*)a)->sin2_port_handler(*((value_message*)m)); break;
			case TAG_cos2_port: ((master*)a)->cos2_port_handler(*((value_message*)m)); break;
			case START: ((master*)a)->start(); break;
		}
	}

	void start(){
/*$TET$master$start*/
/*$TET$*/
	}

	void sin2_port_handler(value_message&m){
/*$TET$master$sin2_port*/
/*$TET$*/
	}

	void cos2_port_handler(value_message&m){
/*$TET$master$cos2_port*/
/*$TET$*/
	}

	void save(saver*s){
/*$TET$master$$save*/
		//TEMPLET::save(s, &x, sizeof(x));
/*$TET$*/
	}

	void restore(restorer*r){
/*$TET$master$$restore*/
		//TEMPLET::restore(r, &x, sizeof(x));
/*$TET$*/
	}

/*$TET$master$$code&data*/
/*$TET$*/
	message _start;
};

#pragma templet *worker(master_port?value_message)

struct worker : actor{
	enum tag{START,TAG_master_port};

	worker(my_engine&e){
		::init(this, &e, worker_recv_adapter);
/*$TET$worker$worker*/
/*$TET$*/
	}

	bool access(message*m){ return TEMPLET::access(m, this); }

	void at(int _at){ TEMPLET::at(this, _at); }
	void delay(double t){ TEMPLET::delay(this, t); }
	double time(){ return TEMPLET::time(this); }
	void stop(){ TEMPLET::stop(this); }

	void master_port(value_message&m){m._server_id=TAG_master_port; m._srv=this;}

	static void worker_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_master_port: ((worker*)a)->master_port_handler(*((value_message*)m)); break;
		}
	}

	void master_port_handler(value_message&m){
/*$TET$worker$master_port*/
/*$TET$*/
	}

/*$TET$worker$$code&data*/
/*$TET$*/
};

int main(int argc, char *argv[])
{
	my_engine e(argc, argv);
/*$TET$footer*/
/*$TET$*/
}
