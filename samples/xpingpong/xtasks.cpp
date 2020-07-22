/*$TET$$header*/

#define _CRT_SECURE_NO_DEPRECATE

#include <xtemplet.hpp>
#include <xeverest.hpp>
#include <string>
#include <iostream>

class hello : public templet::message {
public:
	hello(templet::actor*a, templet::message_adaptor ma) :templet::message(a, ma) {}
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
		p.send();
/*$TET$*/
	}

	inline void on_p(hello&m) {
/*$TET$ping$p*/
		stop();
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
		_p = 0;
		t.app_id("app_id");
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
		_p = &m;

		json in;
		in["name"] = "echo-application";
		in["inputs"]["input-string"] = m.str;

		if (t.submit(in)) std::cout << "task submit succeeded" << std::endl;
		else std::cout << "task submit failed" << std::endl;
/*$TET$*/
	}

	inline void on_t(templet::everest_task&t) {
/*$TET$pong$t*/
		json out = t.result();
		_p->str =  out["output-string"];
		_p->send();
/*$TET$*/
	}

	void p(hello&m) { m.bind(this, &on_p_adapter); }
	templet::everest_task t;

/*$TET$pong$$footer*/
	hello* _p;
/*$TET$*/
};

/*$TET$$footer*/
int main()
{
	templet::engine eng;
	templet::everest_engine teng("access_token");

	if (!teng) {
		std::cout << "task engine is not connected to the Everest server..." << std::endl;
		return EXIT_FAILURE;
	}

	ping a_ping(eng);
	pong a_pong(eng, teng);

	a_pong.p(a_ping.p);

	a_ping.p.str = "Yoo-Hoo!";

	eng.start();

try_continue:
	teng.run();

	if (eng.stopped()) {
		std::cout << "answer from remote Pong:" << std::endl;
		std::cout << a_ping.p.str << std::endl;
		return EXIT_SUCCESS;
	}

	static int tries = 1;
	templet::everest_error cntxt;

	if (teng.error(&cntxt)) {
		std::cout << "task engine failure..." << std::endl;
		if (tries) {
			tries--;
			// try to fix an error
			teng.reset();
			goto try_continue;
		}
	}
	else 
		std::cout << "logical error" << std::endl;
	
	return EXIT_FAILURE;
}
/*$TET$*/
