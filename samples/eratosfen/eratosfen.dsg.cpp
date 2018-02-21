/*$TET$actor*/
#include <templet.hpp>
/*$TET$*/

using namespace TEMPLET;

#pragma templet ~mes=

struct mes : message_interface{
/*$TET$mes$$data*/
/*$TET$*/
};

#pragma templet *seeder(p!mes)+

struct seeder : actor_interface{
	seeder(engine_interface&){
/*$TET$seeder$seeder*/
/*$TET$*/
	}

	mes p;

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
};

#pragma templet *filter(in?mes,out!mes)

struct filter : actor_interface{
	filter(engine_interface&){
/*$TET$filter$filter*/
/*$TET$*/
	}

	void in(mes&){}
	mes out;

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

struct stopper : actor_interface{
	stopper(engine_interface&){
/*$TET$stopper$stopper*/
/*$TET$*/
	}

	void p(mes&){}

	void p_handler(mes&m){
/*$TET$stopper$p*/
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
