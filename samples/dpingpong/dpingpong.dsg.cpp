/*$TET$actor*/
#include <templet.hpp>
/*$TET$*/

using namespace TEMPLET;

#pragma templet ~mes$=

struct mes : message_interface{
	void save(saver*s){
/*$TET$mes$$save*/
		//::save(s, &x, sizeof(x));
/*$TET$*/
	}

	void restore(restorer*r){
/*$TET$mes$$restore*/
		//::restore(r, &x, sizeof(x));
/*$TET$*/
	}
/*$TET$mes$$data*/
/*$TET$*/
};

#pragma templet *ping$(p!mes)+

struct ping : actor_interface{
	ping(engine_interface&){
/*$TET$ping$ping*/
/*$TET$*/
	}

	mes p;

	void start(){
/*$TET$ping$start*/
/*$TET$*/
	}

	void p_handler(mes&m){
/*$TET$ping$p*/
/*$TET$*/
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

/*$TET$ping$$code&data*/
/*$TET$*/
};

#pragma templet *pong$(p?mes)

struct pong : actor_interface{
	pong(engine_interface&){
/*$TET$pong$pong*/
/*$TET$*/
	}

	void p(mes&){}

	void p_handler(mes&m){
/*$TET$pong$p*/
/*$TET$*/
	}

	void save(saver*s){
/*$TET$pong$$save*/
		//::save(s, &x, sizeof(x));
/*$TET$*/
	}

	void restore(restorer*r){
/*$TET$pong$$restore*/
		//::restore(r, &x, sizeof(x));
/*$TET$*/
	}

/*$TET$pong$$code&data*/
/*$TET$*/
};

int main(int argc, char *argv[])
{
	engine_interface e(argc, argv);
/*$TET$footer*/
/*$TET$*/
}
