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

#pragma templet ~value_message$=

struct value_message : message{
	value_message(actor*a, engine*e, int t) : _where(CLI), _cli(a), _client_id(t){
		::init(this, e, value_message_save_adapter, value_message_restore_adapter);
	}

	friend void value_message_save_adapter(message*m, saver*s){
		((value_message*)m)->save(s);
	}

	friend void value_message_restore_adapter(message*m, restorer*r){
		((value_message*)m)->restore(r);
	}

	bool access(actor*a){
		return ::access(this, a);
	}

	void send(){
		if (_where == CLI){ ::send(this, _srv, _server_id); _where = SRV; }
		else if (_where == SRV){ ::send(this, _cli, _client_id); _where = CLI; }
	}

	void save(saver*s){
/*$TET$value_message$$save*/
		//::save(s, &x, sizeof(x));
/*$TET$*/
	}

	void restore(restorer*r){
/*$TET$value_message$$restore*/
		//::restore(r, &x, sizeof(x));
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
	enum tag{TAG_sin2_port=MES_TAGS::START+1,TAG_cos2_port=MES_TAGS::START+2};

	master(my_engine&e):_sin2_port(this, &e, TAG_sin2_port),_cos2_port(this, &e, TAG_cos2_port){
		::init(this, &e, master_recv_adapter, master_save_adapter, master_restore_adapter);
		::init(&_start, &e);
		::send(&_start, this, MES_TAGS::START);
/*$TET$master$master*/
/*$TET$*/
	}

	friend void master_save_adapter(actor*a, saver*s){
		((master*)a)->save(s);
	}

	friend void master_restore_adapter(actor*a, restorer*r){
		((master*)a)->restore(r);
	}

	void at(int _at){ ::at(this, _at); }
	void delay(double t){ ::delay(this, t); }
	void stop(){ ::stop(this); }

	value_message* sin2_port(){return &_sin2_port;}
	value_message* cos2_port(){return &_cos2_port;}

	friend void master_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_sin2_port: ((master*)a)->sin2_port(*((value_message*)m)); break;
			case TAG_cos2_port: ((master*)a)->cos2_port(*((value_message*)m)); break;
			case MES_TAGS::START: ((master*)a)->start(); break;
		}
	}

	void start(){
/*$TET$master$start*/
/*$TET$*/
	}

	void sin2_port(value_message&m){
/*$TET$master$sin2_port*/
/*$TET$*/
	}

	void cos2_port(value_message&m){
/*$TET$master$cos2_port*/
/*$TET$*/
	}

	void save(saver*s){
/*$TET$master$$save*/
		//::save(s, &x, sizeof(x));
/*$TET$*/
	}

	void restore(restorer*r){
/*$TET$master$$restore*/
		//::restore(r, &x, sizeof(x));
/*$TET$*/
	}

/*$TET$master$$code&data*/
/*$TET$*/

	value_message _sin2_port;
	value_message _cos2_port;
	message _start;
};

#pragma templet *worker(master_port?value_message)

struct worker : actor{
	enum tag{TAG_master_port};

	worker(my_engine&e){
		::init(this, &e, worker_recv_adapter);
/*$TET$worker$worker*/
/*$TET$*/
	}

	void at(int _at){ ::at(this, _at); }
	void delay(double t){ ::delay(this, t); }
	void stop(){ ::stop(this); }

	void master_port(value_message*m){m->_server_id=TAG_master_port; m->_srv=this;}

	friend void worker_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_master_port: ((worker*)a)->master_port(*((value_message*)m)); break;
		}
	}

	void master_port(value_message&m){
/*$TET$worker$master_port*/
/*$TET$*/
	}

/*$TET$worker$$code&data*/
/*$TET$*/

};

/*$TET$footer*/
/*$TET$*/
