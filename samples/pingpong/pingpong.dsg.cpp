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

#pragma templet *ping(p!mes)+

struct ping : actor{
	ping(my_engine&){
/*$TET$ping$ping*/
/*$TET$*/
	}

	void delay(double);
	double time();
	void at(int);
	void stop();

	mes* p();

	void start(){
/*$TET$ping$start*/
/*$TET$*/
	}

	void p(mes&m){
/*$TET$ping$p*/
/*$TET$*/
	}

/*$TET$ping$$code&data*/
/*$TET$*/

	mes _p;
};

#pragma templet *pong(p?mes)

struct pong : actor{
	pong(my_engine&){
/*$TET$pong$pong*/
/*$TET$*/
	}

	void delay(double);
	double time();
	void at(int);
	void stop();

	void p(mes*);

	void p(mes&m){
/*$TET$pong$p*/
/*$TET$*/
	}

/*$TET$pong$$code&data*/
/*$TET$*/

};

/*$TET$footer*/
/*$TET$*/
