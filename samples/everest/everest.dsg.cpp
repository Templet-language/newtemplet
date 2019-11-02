/*$TET$actor*/
#include <templet.hpp>
/*$TET$*/

using namespace TEMPLET;

#pragma templet ~mes=

struct mes : message_interface{
/*$TET$mes$$data*/
/*$TET$*/
};

#pragma templet *sorter(p!mes,t.tasksort)+

struct sorter : actor_interface{
	sorter(engine_interface&){
/*$TET$sorter$sorter*/
/*$TET$*/
	}

	mes p;
	tasksort t;
	void t_submit(){}

	void start(){
/*$TET$sorter$start*/
/*$TET$*/
	}

	void p_handler(mes&m){
/*$TET$sorter$p*/
/*$TET$*/
	}

	void t_handler(tasksort&t){
/*$TET$sorter$t*/
/*$TET$*/
	}

/*$TET$sorter$$code&data*/
/*$TET$*/
};

#pragma templet *merger(p1?mes,p2?mes,t.taskmerge)

struct merger : actor_interface{
	merger(engine_interface&){
/*$TET$merger$merger*/
/*$TET$*/
	}

	void p1(mes&){}
	void p2(mes&){}
	taskmerge t;
	void t_submit(){}

	void p1_handler(mes&m){
/*$TET$merger$p1*/
/*$TET$*/
	}

	void p2_handler(mes&m){
/*$TET$merger$p2*/
/*$TET$*/
	}

	void t_handler(taskmerge&t){
/*$TET$merger$t*/
/*$TET$*/
	}

/*$TET$merger$$code&data*/
/*$TET$*/
};

/*$TET$code&data*/
/*$TET$*/

int main(int argc, char *argv[])
{
	engine_interface e(argc, argv);
/*$TET$footer*/
/*$TET$*/
}
