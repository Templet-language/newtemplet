/*$TET$actor*/
#include <templet.hpp>
/*$TET$*/

using namespace TEMPLET;

class my_engine{
public:
	my_engine(int argc, char *argv[]);
	void run();
	void map();
};

#pragma templet ~mes=

struct mes : message{
	bool access(actor*);
	void send();

/*$TET$mes$$data*/
/*$TET$*/
};

#pragma templet *producer(out!mes)+

struct producer : actor{
	producer(my_engine&){
/*$TET$producer$producer*/
/*$TET$*/
	}

	void delay(double);
	double time();
	void at(int);
	void stop();

	mes* out();

	void start(){
/*$TET$producer$start*/
/*$TET$*/
	}

	void out(mes&m){
/*$TET$producer$out*/
/*$TET$*/
	}

/*$TET$producer$$code&data*/
/*$TET$*/

	mes _out;
};

#pragma templet *sorter(in?mes,out!mes)

struct sorter : actor{
	sorter(my_engine&){
/*$TET$sorter$sorter*/
/*$TET$*/
	}

	void delay(double);
	double time();
	void at(int);
	void stop();

	void in(mes*);
	mes* out();

	void in(mes&m){
/*$TET$sorter$in*/
/*$TET$*/
	}

	void out(mes&m){
/*$TET$sorter$out*/
/*$TET$*/
	}

/*$TET$sorter$$code&data*/
/*$TET$*/

	mes _out;
};

#pragma templet *stoper(in?mes)

struct stoper : actor{
	stoper(my_engine&){
/*$TET$stoper$stoper*/
/*$TET$*/
	}

	void delay(double);
	double time();
	void at(int);
	void stop();

	void in(mes*);

	void in(mes&m){
/*$TET$stoper$in*/
/*$TET$*/
	}

/*$TET$stoper$$code&data*/
/*$TET$*/

};

/*$TET$footer*/
/*$TET$*/
