/*$TET$actor*/
#include <templet.hpp>
/*$TET$*/

using namespace TEMPLET;

#pragma templet ~mes=

struct mes : message_interface{
/*$TET$mes$$data*/
/*$TET$*/
};

#pragma templet *producer(out!mes)+

struct producer : actor_interface{
	producer(engine_interface&){
/*$TET$producer$producer*/
/*$TET$*/
	}

	mes out;

	void start(){
/*$TET$producer$start*/
/*$TET$*/
	}

	void out_handler(mes&m){
/*$TET$producer$out*/
/*$TET$*/
	}

/*$TET$producer$$code&data*/
/*$TET$*/
};

#pragma templet *sorter(in?mes,out!mes)

struct sorter : actor_interface{
	sorter(engine_interface&){
/*$TET$sorter$sorter*/
/*$TET$*/
	}

	void in(mes&){}
	mes out;

	void in_handler(mes&m){
/*$TET$sorter$in*/
/*$TET$*/
	}

	void out_handler(mes&m){
/*$TET$sorter$out*/
/*$TET$*/
	}

/*$TET$sorter$$code&data*/
/*$TET$*/
};

#pragma templet *stopper(in?mes)

struct stopper : actor_interface{
	stopper(engine_interface&){
/*$TET$stopper$stopper*/
/*$TET$*/
	}

	void in(mes&){}

	void in_handler(mes&m){
/*$TET$stopper$in*/
/*$TET$*/
	}

/*$TET$stopper$$code&data*/
/*$TET$*/
};

int main(int argc, char *argv[])
{
	engine_interface e(argc, argv);
/*$TET$footer*/
/*$TET$*/
}
