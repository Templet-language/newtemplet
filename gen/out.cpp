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

enum MES_TAGS{ MES_message1, MES_sm, START };

#pragma templet ~message1
struct message1 : messsage{
	message1(actor*a, engine*e){
		::init(this, e);
	}

	bool access(actor*a){
		return ::access(this, a);
	}

	void send(actor*a){
		::send(this, a, MES_message1);
	}

/*$TET$message1$$data*/
/*$TET$*/
};

#pragma templet ~message2=
struct message2 : messsage{
	message2(actor*a, engine*e, int t) : _where(CLI), _cli(a), _client_id(t){
		::init(this, e);
	}

	bool access(){
		bool ret;
		if (_where == CLI) ret = ::access(this, _cli);
		else if (_where == SRV) ret = ::access(this, _srv);
		return ret;
	}

	void send(){
		if (_where == CLI){ ::send(this, _srv, _server_id); _where = SRV; }
		else if (_where == SRV){ ::send(this, _cli, _client_id); _where = CLI; }
	}

/*$TET$message2$$data*/
/*$TET$*/

	enum { CLI, SRV } _where;
	actor* _srv;
	actor* _cli;
	int _client_id;
	int _server_id;
};

#pragma templet ~message3$(submessage1!,submessage2?,submessage3?,-submessage4!)
struct message3 : messsage{
	message3(actor*a, engine*e, int t) : _where(CLI), _cli(a), _client_id(t){
		::init(this, e, save_adapter, restore_adapter);
	}

	friend void save_adapter(message*m, saver*s){
		((value_message*)m)->save(s);
	}

	friend void restore_adapter(message*m, restorer*r){
		((value_message*)m)->restore(r);
	}

	enum tag{TAG_submessage1,TAG_submessage2,TAG_submessage3,TAG_submessage4};

	struct submessage1{
/*$TET$message3$submessage1*/
/*$TET$*/
	} _submessage1;

	struct submessage2{
/*$TET$message3$submessage2*/
/*$TET$*/
	} _submessage2;

	struct submessage3{
/*$TET$message3$submessage3*/
/*$TET$*/
	} _submessage3;

	bool access(tag t){
		bool ret;
		if (_where == CLI) ret = ::access(this, _cli);
		else if (_where == SRV) ret = ::access(this, _srv);
		return ret && t==_tag;
	}

	void send(tag t){
		_tag = t;
		if (_where == CLI){ ::send(this, _srv, _server_id); _where = SRV; }
		else if (_where == SRV){ ::send(this, _cli, _client_id); _where = CLI; }
	}

	void save(saver*s){
/*$TET$message3$$save*/
		//::save(s, &x, sizeof(x));
/*$TET$*/
	}

	void restore(restorer*r){
/*$TET$message3$$restore*/
		//::restore(r, &x, sizeof(x));
/*$TET$*/
	}

	enum { CLI, SRV } _where;
	actor* _srv;
	actor* _cli;
	int _client_id;
	int _server_id;
	int _tag;
};

#pragma templet ~m(argsin2!,argcos2!,res?)
struct m : messsage{
	m(actor*a, engine*e, int t) : _where(CLI), _cli(a), _client_id(t){
		::init(this, e);
	}

	enum tag{TAG_argsin2,TAG_argcos2,TAG_res};

	struct argsin2{
/*$TET$m$argsin2*/
/*$TET$*/
	} _argsin2;

	struct argcos2{
/*$TET$m$argcos2*/
/*$TET$*/
	} _argcos2;

	struct res{
/*$TET$m$res*/
/*$TET$*/
	} _res;

	bool access(tag t){
		bool ret;
		if (_where == CLI) ret = ::access(this, _cli);
		else if (_where == SRV) ret = ::access(this, _srv);
		return ret && t==_tag;
	}

	void send(tag t){
		_tag = t;
		if (_where == CLI){ ::send(this, _srv, _server_id); _where = SRV; }
		else if (_where == SRV){ ::send(this, _cli, _client_id); _where = CLI; }
	}

	enum { CLI, SRV } _where;
	actor* _srv;
	actor* _cli;
	int _client_id;
	int _server_id;
	int _tag;
};

#pragma templet ~sm
struct sm : messsage{
	sm(actor*a, engine*e){
		::init(this, e);
	}

	bool access(actor*a){
		return ::access(this, a);
	}

	void send(actor*a){
		::send(this, a, MES_sm);
	}

/*$TET$sm$$data*/
/*$TET$*/
};

#pragma templet ~ping$=
struct ping : messsage{
	ping(actor*a, engine*e, int t) : _where(CLI), _cli(a), _client_id(t){
		::init(this, e, save_adapter, restore_adapter);
	}

	friend void save_adapter(message*m, saver*s){
		((value_message*)m)->save(s);
	}

	friend void restore_adapter(message*m, restorer*r){
		((value_message*)m)->restore(r);
	}

	bool access(){
		bool ret;
		if (_where == CLI) ret = ::access(this, _cli);
		else if (_where == SRV) ret = ::access(this, _srv);
		return ret;
	}

	void send(){
		if (_where == CLI){ ::send(this, _srv, _server_id); _where = SRV; }
		else if (_where == SRV){ ::send(this, _cli, _client_id); _where = CLI; }
	}

	void save(saver*s){
/*$TET$ping$$save*/
		//::save(s, &x, sizeof(x));
/*$TET$*/
	}

	void restore(restorer*r){
/*$TET$ping$$restore*/
		//::restore(r, &x, sizeof(x));
/*$TET$*/
	}

/*$TET$ping$$data*/
/*$TET$*/

	enum { CLI, SRV } _where;
	actor* _srv;
	actor* _cli;
	int _client_id;
	int _server_id;
};

#pragma templet *actor1
struct actor1 : actor{
	actor1(my_engine&e){
		::init(this, &e, recv_adapter);
/*$TET$actor1$actor1*/
/*$TET$*/
	}

	void at(int _at){ ::at(this, _at); }
	void delay(double t){ ::delay(this, t); }
	void stop(){ ::stop(this); }

	friend void recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case MES_TAGS::MES_message1: ((actor1*)a)->recv_message1(*((message1*)m)); break;
			case MES_TAGS::MES_sm: ((actor1*)a)->recv_sm(*((sm*)m)); break;
		}
	}

	void recv_message1(message1&m){
/*$TET$actor1$recv_message1*/
/*$TET$*/
	}

	void recv_sm(sm&m){
/*$TET$actor1$recv_sm*/
/*$TET$*/
	}

/*$TET$actor1$$code&data*/
/*$TET$*/
};

#pragma templet *actor2+
struct actor2 : actor{
	actor2(my_engine&e){
		::init(this, &e, recv_adapter);
		::init(&_start, &e);
		::send(&_start, this, MES_TAGS::START);
/*$TET$actor2$actor2*/
/*$TET$*/
	}

	void at(int _at){ ::at(this, _at); }
	void delay(double t){ ::delay(this, t); }
	void stop(){ ::stop(this); }

	friend void recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case MES_TAGS::MES_message1: ((actor2*)a)->recv_message1(*((message1*)m)); break;
			case MES_TAGS::MES_sm: ((actor2*)a)->recv_sm(*((sm*)m)); break;
			case MES_TAGS::START: ((actor2*)a)->start(); break;
		}
	}

	void start(){
/*$TET$actor2$start*/
/*$TET$*/
	}

	void recv_message1(message1&m){
/*$TET$actor2$recv_message1*/
/*$TET$*/
	}

	void recv_sm(sm&m){
/*$TET$actor2$recv_sm*/
/*$TET$*/
	}

/*$TET$actor2$$code&data*/
/*$TET$*/
	message _start;
};

#pragma templet *actor3$(?,port1?message2,port3!message4)+
struct actor3 : actor{
	enum tag{TAG_port1=MES_TAGS::START+1,TAG_port3=MES_TAGS::START+2};

	actor3(my_engine&e):_port3(this, &e, TAG_port3){
		::init(this, &e, recv_adapter, save_adapter, restore_adapter);
		::init(&_start, &e);
		::send(&_start, this, MES_TAGS::START);
/*$TET$actor3$actor3*/
/*$TET$*/
	}

	friend void save_adapter(actor*a, saver*s){
		((actor3*)a)->save(s);
	}

	friend void restore_adapter(actor*a, restorer*r){
		((actor3*)a)->restore(r);
	}

	void at(int _at){ ::at(this, _at); }
	void delay(double t){ ::delay(this, t); }
	void stop(){ ::stop(this); }

	void port1(message2*m){m->_server_id=TAG_port1; m->_srv=this;}
	message4* port3(){return &_port3;}

	friend void recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case MES_TAGS::MES_message1: ((actor3*)a)->recv_message1(*((message1*)m)); break;
			case MES_TAGS::MES_sm: ((actor3*)a)->recv_sm(*((sm*)m)); break;
			case TAG_port1: ((actor3*)a)->port1(*((message2*)m)); break;
			case TAG_port3: ((actor3*)a)->port3(*((message4*)m)); break;
			case MES_TAGS::START: ((actor3*)a)->start(); break;
		}
	}

	void start(){
/*$TET$actor3$start*/
/*$TET$*/
	}

	void recv_message1(message1&m){
/*$TET$actor3$recv_message1*/
/*$TET$*/
	}

	void recv_sm(sm&m){
/*$TET$actor3$recv_sm*/
/*$TET$*/
	}

	void port1(message2&m){
/*$TET$actor3$port1*/
/*$TET$*/
	}

	void port3(message4&m){
/*$TET$actor3$port3*/
/*$TET$*/
	}

	void save(saver*s){
/*$TET$actor3$$save*/
		//::save(s, &x, sizeof(x));
/*$TET$*/
	}

	void restore(restorer*r){
/*$TET$actor3$$restore*/
		//::restore(r, &x, sizeof(x));
/*$TET$*/
	}

/*$TET$actor3$$code&data*/
/*$TET$*/

	message4 _port3;
	message _start;
};

#pragma templet *master$(?,p1!m,p2?ping,p3!m)+
struct master : actor{
	enum tag{TAG_p1=MES_TAGS::START+1,TAG_p2=MES_TAGS::START+2,TAG_p3=MES_TAGS::START+3};

	master(my_engine&e):_p1(this, &e, TAG_p1),_p3(this, &e, TAG_p3){
		::init(this, &e, recv_adapter, save_adapter, restore_adapter);
		::init(&_start, &e);
		::send(&_start, this, MES_TAGS::START);
/*$TET$master$master*/
/*$TET$*/
	}

	friend void save_adapter(actor*a, saver*s){
		((master*)a)->save(s);
	}

	friend void restore_adapter(actor*a, restorer*r){
		((master*)a)->restore(r);
	}

	void at(int _at){ ::at(this, _at); }
	void delay(double t){ ::delay(this, t); }
	void stop(){ ::stop(this); }

	m* p1(){return &_p1;}
	void p2(ping*m){m->_server_id=TAG_p2; m->_srv=this;}
	m* p3(){return &_p3;}

	friend void recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case MES_TAGS::MES_message1: ((master*)a)->recv_message1(*((message1*)m)); break;
			case MES_TAGS::MES_sm: ((master*)a)->recv_sm(*((sm*)m)); break;
			case TAG_p1: ((master*)a)->p1(*((m*)m)); break;
			case TAG_p2: ((master*)a)->p2(*((ping*)m)); break;
			case TAG_p3: ((master*)a)->p3(*((m*)m)); break;
			case MES_TAGS::START: ((master*)a)->start(); break;
		}
	}

	void start(){
/*$TET$master$start*/
/*$TET$*/
	}

	void recv_message1(message1&m){
/*$TET$master$recv_message1*/
/*$TET$*/
	}

	void recv_sm(sm&m){
/*$TET$master$recv_sm*/
/*$TET$*/
	}

	void p1(m&m){
/*$TET$master$p1*/
/*$TET$*/
	}

	void p2(ping&m){
/*$TET$master$p2*/
/*$TET$*/
	}

	void p3(m&m){
/*$TET$master$p3*/
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

	m _p1;
	m _p3;
	message _start;
};

/*$TET$footer*/
/*$TET$*/
