/*$TET$actor*/
#include <templet.hpp>
/*$TET$*/

using namespace TEMPLET;

#pragma templet ~value_message$=

struct value_message : message_interface{
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

struct master : actor_interface{
	master(engine_interface&){
/*$TET$master$master*/
/*$TET$*/
	}

	value_message sin2_port;
	value_message cos2_port;

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
};

#pragma templet *worker(master_port?value_message)

struct worker : actor_interface{
	worker(engine_interface&){
/*$TET$worker$worker*/
/*$TET$*/
	}

	void master_port(value_message&){}

	void master_port_handler(value_message&m){
/*$TET$worker$master_port*/
/*$TET$*/
	}

/*$TET$worker$$code&data*/
/*$TET$*/
};

int main(int argc, char *argv[])
{
	engine_interface e(argc, argv);
/*$TET$footer*/
/*$TET$*/
}
