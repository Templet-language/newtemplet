/*$TET$$header*/
#include <xtemplet.hpp>

class number: public templet::message {
public:
	number(templet::actor*a, templet::message_adaptor ma):templet::message(a, ma) {}
};

class sinetask : public templet::base_task {
public:
	sinetask(templet::base_engine&te, templet::actor*a, templet::task_adaptor ta):base_task(te,a,ta){}
};
/*$TET$*/

#pragma templet !master(cw!number,sw!number)

struct master :public templet::actor {
	static void on_cw_adapter(templet::actor*a, templet::message*m) {
		((master*)a)->on_cw(*(number*)m);}
	static void on_sw_adapter(templet::actor*a, templet::message*m) {
		((master*)a)->on_sw(*(number*)m);}

	master(templet::engine&e):templet::actor(e),
		cw(this, &on_cw_adapter),
		sw(this, &on_sw_adapter)
	{
/*$TET$master$master*/
/*$TET$*/
	}

	void start(){
/*$TET$master$start*/
/*$TET$*/
	}

	inline void on_cw(number&m) {
/*$TET$master$cw*/
/*$TET$*/
	}

	inline void on_sw(number&m) {
/*$TET$master$sw*/
/*$TET$*/
	}

	number cw;
	number sw;

/*$TET$master$$footer*/
/*$TET$*/
};

#pragma templet cworker(cw?number,t:base)

struct cworker :public templet::actor {
	static void on_cw_adapter(templet::actor*a, templet::message*m) {
		((cworker*)a)->on_cw(*(number*)m);}
	static void on_t_adapter(templet::actor*a, templet::base_task*t) {
		((cworker*)a)->on_t(*(templet::base_task*)t);}

	cworker(templet::engine&e,templet::base_engine&te_base):templet::actor(e),
		t(te_base, this, &on_t_adapter)
	{
/*$TET$cworker$cworker*/
/*$TET$*/
	}

	inline void on_cw(number&m) {
/*$TET$cworker$cw*/
/*$TET$*/
	}

	inline void on_t(templet::base_task&t) {
/*$TET$cworker$t*/
/*$TET$*/
	}

	void cw(number&m) { m.bind(this, &on_cw_adapter); }
	templet::base_task t;

/*$TET$cworker$$footer*/
/*$TET$*/
};

#pragma templet sworker(sw?number,t:base.sinetask)

struct sworker :public templet::actor {
	static void on_sw_adapter(templet::actor*a, templet::message*m) {
		((sworker*)a)->on_sw(*(number*)m);}
	static void on_t_adapter(templet::actor*a, templet::base_task*t) {
		((sworker*)a)->on_t(*(sinetask*)t);}

	sworker(templet::engine&e,templet::base_engine&te_base):templet::actor(e),
		t(te_base, this, &on_t_adapter)
	{
/*$TET$sworker$sworker*/
/*$TET$*/
	}

	inline void on_sw(number&m) {
/*$TET$sworker$sw*/
/*$TET$*/
	}

	inline void on_t(sinetask&t) {
/*$TET$sworker$t*/
/*$TET$*/
	}

	void sw(number&m) { m.bind(this, &on_sw_adapter); }
	sinetask t;

/*$TET$sworker$$footer*/
/*$TET$*/
};

/*$TET$$footer*/
int main()
{
	return 1;
}
/*$TET$*/
