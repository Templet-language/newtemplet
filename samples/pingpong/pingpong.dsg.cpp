/*$TET$actor*/
#include <templet.hpp>
/*$TET$*/

using namespace TEMPLET;

#pragma templet ~mes=

struct mes : message_interface{
/*$TET$mes$$data*/
/*$TET$*/
};

#pragma templet *ping(p!mes)+

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

/*$TET$ping$$code&data*/
/*$TET$*/
};

#pragma templet *pong(p?mes)

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

/*$TET$pong$$code&data*/
/*$TET$*/
};

int main(int argc, char *argv[])
{
	engine_interface e(argc, argv);
/*$TET$footer*/
/*$TET$*/
}
