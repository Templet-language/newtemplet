/*$TET$$header*/

#define _CRT_SECURE_NO_DEPRECATE

#include <xtemplet.hpp>
#include <xeverest.hpp>
#include <string>

class hello : public templet::message {
public:
	hello(templet::actor*a=0, templet::message_adaptor ma=0) :templet::message(a, ma) {}
	std::string str;
};

/*$TET$*/

#pragma templet !ping(p!hello)

struct ping :public templet::actor {
	static void on_p_adapter(templet::actor*a, templet::message*m) {
		((ping*)a)->on_p(*(hello*)m);}

	ping(templet::engine&e) :ping() {
		ping::engines(e);
	}

	ping() :templet::actor(true),
		p(this, &on_p_adapter)
	{
/*$TET$ping$ping*/
/*$TET$*/
	}

	void engines(templet::engine&e) {
		templet::actor::engine(e);
/*$TET$ping$engines*/
/*$TET$*/
	}

	void start() {
/*$TET$ping$start*/
/*$TET$*/
	}

	inline void on_p(hello&m) {
/*$TET$ping$p*/
/*$TET$*/
	}

	hello p;

/*$TET$ping$$footer*/
/*$TET$*/
};

#pragma templet pong(p?hello,t:everest)

struct pong :public templet::actor {
	static void on_p_adapter(templet::actor*a, templet::message*m) {
		((pong*)a)->on_p(*(hello*)m);}
	static void on_t_adapter(templet::actor*a, templet::task*t) {
		((pong*)a)->on_t(*(templet::everest_task*)t);}

	pong(templet::engine&e,templet::everest_engine&te_everest) :pong() {
		pong::engines(e,te_everest);
	}

	pong() :templet::actor(false),
		t(this, &on_t_adapter)
	{
/*$TET$pong$pong*/
/*$TET$*/
	}

	void engines(templet::engine&e,templet::everest_engine&te_everest) {
		templet::actor::engine(e);
		t.engine(te_everest);
/*$TET$pong$engines*/
/*$TET$*/
	}

	inline void on_p(hello&m) {
/*$TET$pong$p*/
/*$TET$*/
	}

	inline void on_t(templet::everest_task&t) {
/*$TET$pong$t*/
		// error handling !!!
/*$TET$*/
	}

	void p(hello&m) { m.bind(this, &on_p_adapter); }
	templet::everest_task t;

/*$TET$pong$$footer*/
/*$TET$*/
};

/*$TET$$footer*/
int main()
{
	templet::engine eng;
	templet::everest_engine teng("access_token");

	ping a_ping(eng);
	pong a_pong(eng, teng);

	a_pong.p(a_ping.p);

	if (teng.ready()) {
		eng.start();
		if (teng.wait_all() && eng.stopped()) {
			
			return EXIT_SUCCESS;
		}
	}
	return EXIT_FAILURE;
}
/*$TET$*/
