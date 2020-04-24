#include <iostream>
using namespace std;

namespace templet {

	class message;
	class actor;
	typedef void (actor::*event_h)(message&);

	class task_engine { virtual void run()=0; };

	class shm_task_engine : public task_engine { virtual void run() {} };

	class task {
		actor& act;
		shm_task_engine* tsk_eng;
		//event_h ev_h; ?????
	public:
		task(actor&a, task_engine&te/* , event_h e_h*/) :act(a) /*, ev_h(e_h)*/ {
			tsk_eng = dynamic_cast<shm_task_engine*>(&te);
		}
	};

	class engine {
	friend class message;
		message* queue;
	public:
		void dispatch();
	};

	class actor {
	friend class message;
		engine& eng;
	public:
		actor(engine& e): eng(e){}
	};

	class message {
	friend class engine;
		actor* act;
		engine& eng;
		event_h ev_h;
	public:
		message(actor*a, event_h e_h) : act(a), eng(a->eng), ev_h(e_h) {}
		void send() { eng.queue = this; }
	};

	void engine::dispatch() {
		event_h ev_h = queue->ev_h;
		actor*  act = queue->act;
		(act->*ev_h)(*queue);
	}
	
}

class mytask : public templet::task{
public:
	mytask(templet::actor&a, templet::task_engine&te):templet::task(a,te){}
};

class mymessage : public templet::message {
public:
	mymessage(templet::actor*a, templet::event_h e_h):templet::message(a,e_h){}

};

class myactor: public templet::actor {
public:
	myactor(templet::engine&e,templet::task_engine&te): 
		templet::actor(e), 
		mes(this, (templet::event_h)(&myactor::mes_h)),
		tsk(*this,te)
	{
	}

	void mes_h(mymessage&) {
		cout << "\nhello\n";
	}

	mymessage mes;
	mytask tsk;
};

int main()
{
	templet::engine eng;
	templet::shm_task_engine tsk_eng;

	myactor act(eng,tsk_eng);
	act.mes.send();
	eng.dispatch();

    return 0;
}