/*$TET$header*/
#include <templet.hpp>
/*$TET$*/

using namespace TEMPLET;

class my_engine{
public:
	my_engine(int argc, char *argv[]);
	void run();
	void map();
};

#pragma templet ~value_message$=
struct value_message{
	bool access();
	void send();

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
};

#pragma templet *master$(sin2_port!value_message,cos2_port!value_message)+
struct master{
	master(my_engine&){
/*$TET$master$master*/
/*$TET$*/
	}

	void delay(double);
	void at(int);
	void stop();

	value_message* sin2_port();
	value_message* cos2_port();

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
};

#pragma templet *worker(master_port?value_message)
struct worker{
	worker(my_engine&){
/*$TET$worker$worker*/
/*$TET$*/
	}

	void delay(double);
	void at(int);
	void stop();

	void master_port(value_message*);

	void master_port(value_message&m){
/*$TET$worker$master_port*/
/*$TET$*/
	}

/*$TET$worker$$code&data*/
/*$TET$*/

};

/*$TET$footer*/
/*$TET$*/
