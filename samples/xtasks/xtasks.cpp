/*$TET$$header*/

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

	void on_start(){
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

#pragma templet cworker(cw?number,t:base.mytask)

struct cworker :public templet::actor {
	static void on_cw_adapter(templet::actor*a, templet::message*m) {
		((cworker*)a)->on_cw(*(number*)m);}
	static void on_t_adapter(templet::actor*a, templet::base_task*t) {
		((cworker*)a)->on_t(*(mytask*)t);

	cworker(templet::engine&e):templet::actor(e),
		t(te_base, this, &on_t_adapter)
	{
/*$TET$cworker$cworker*/
/*$TET$*/
	}

	inline void on_cw(number&m) {
/*$TET$cworker$cw*/
/*$TET$*/
	}

	inline void on_t(mytask&t) {
/*$TET$cworker$t*/
/*$TET$*/
	}

	void cw(number&m) { m.bind(this, &on_cw_adapter); }
	mytask t;

/*$TET$cworker$$footer*/
/*$TET$*/
};

#pragma templet sworker(sw?number,t:base)

struct sworker :public templet::actor {
	static void on_sw_adapter(templet::actor*a, templet::message*m) {
		((sworker*)a)->on_sw(*(number*)m);}
	static void on_t_adapter(templet::actor*a, templet::base_task*t) {
		((sworker*)a)->on_t(*(templet::base_task*)t);

	sworker(templet::engine&e):templet::actor(e),
		t(te_base, this, &on_t_adapter)
	{
/*$TET$sworker$sworker*/
/*$TET$*/
	}

	inline void on_sw(number&m) {
/*$TET$sworker$sw*/
/*$TET$*/
	}

	inline void on_t(templet::base_task&t) {
/*$TET$sworker$t*/
/*$TET$*/
	}

	void sw(number&m) { m.bind(this, &on_sw_adapter); }
	templet::base_task t;

/*$TET$sworker$$footer*/
/*$TET$*/
};

/*$TET$$footer*/
/*$TET$*/
