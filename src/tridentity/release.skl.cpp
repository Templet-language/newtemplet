/*$TET$header*/
/*$TET$*/

using namespace TEMPLET;

class my_engine : public engine{
public:
	my_engine(int argc, char *argv[]){
		::init(this,argc,argv);
	}
	void run(){ ::run(this); }
	void map(){ ::map(this); }
};

class value_message :  public message{
public:
	value_message(actor*a, engine*e,int t) : where(CLI), _cli(a), client_id(t){
		::init(this, e, save_adapter, restore_adapter);
	}

	bool access(){
		bool ret;
		if (where == CLI) ret=::access(this, _cli);
		else if (where == SRV) ret=::access(this, _srv);
		return ret;
	}

	void send(){
		if (where == CLI){ ::send(this, _srv, server_id); where = SRV; }
		else if (where == SRV){ ::send(this, _cli, client_id); where = CLI; }
	}

	void save(saver*s){
/*$TET$value_message$$save*/
/*$TET$*/
	}
	void restore(restorer*r){
/*$TET$value_message$$restore*/
/*$TET$*/
	}

/*$TET$value_message$$data*/
/*$TET$*/

	enum { CLI, SRV } where;
	actor* _srv;
	actor* _cli;
	int client_id;
	int server_id;

	friend void save_adapter(message*m, saver*s){
		((value_message*)m)->save(s);
	}
	friend void restore_adapter(message*m, restorer*r){
		((value_message*)m)->restore(r);
	}
};

class master : public actor{
public:
	enum{ PORT_sin2=1, PORT_cos2=2, START=3 };

	master(my_engine&e) : _cos2(this,&e,PORT_cos2), _sin2(this,&e,PORT_sin2){
		::init(this, &e, recv_master, save_adapter, restore_adapter);
				
		::init(&_start, &e);
		::send(&_start, this, START);
	}

	value_message& sin2_port(){  return _sin2; }
	value_message& cos2_port(){  return _cos2; }

	void at(int _at){ ::at(this,_at); }
	void delay(double t){ ::delay(this, t); }
	void stop(){ ::stop(this); }

	void start(){
/*$TET$master$start*/
/*$TET$*/
	}

	void sin2_func(value_message&m){
/*$TET$master$sin2_func*/
/*$TET$*/
	}

	void cos2_func(value_message&m){
/*$TET$master$cos2_func*/
/*$TET$*/
	}

	void save(saver*s){
/*$TET$master$$save*/
/*$TET$*/
	}

	void restore(restorer*r){
/*$TET$master$$restore*/
/*$TET$*/
	}

	value_message _cos2;
	value_message _sin2;

/*$TET$master$$data*/
/*$TET$*/

	message _start;

	friend void recv_master (actor*a, message*m,int tag){
		master* th = (master*)a;
		if (tag == PORT_cos2) th->cos2_func(*((value_message*)m));
		else if (tag == PORT_sin2) th->sin2_func(*((value_message*)m));
		else if (tag == START) th->start();
	}

	friend void save_adapter(actor*a, saver*s){
		((value_message*)a)->save(s);
	}

	friend void restore_adapter(actor*a, restorer*r){
		((value_message*)a)->restore(r);
	}
	
};

class worker: public actor{
public:
	enum{PORT_master=1};

	worker(my_engine&e){
		::init(this, &e, recv_worker);
	}

	void master_port(value_message&m){
		m._srv = this; m.server_id = PORT_master;
	}
	
	void master_func(value_message&m){
/*$TET$worker$master_func*/
/*$TET$*/
	}

	void at(int _at){ ::at(this, _at); }
	void delay(double t){ ::delay(this, t); }

	friend void recv_worker(actor*a, message*m,int tag){
		worker* th = (worker*)a;
		if (tag == PORT_master) th->master_func(*((value_message*)m));
	}
};

/*$TET$footer*/
/*$TET$*/