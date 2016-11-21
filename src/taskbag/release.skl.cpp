/*$TET$header*/
#define TET_DEBUG_SERIAL
#include <templet.hpp>
/*$TET$*/
#include <queue>
#include <thread>

using namespace std;
using namespace TEMPLET;

struct task_result : message{

	task_result(engine*e,actor*m,actor*w): _master(m), _worker(w) {
		::init(this, e, save_adapter, restore_adapter);
		_call = FIRST_CALL;
	}

	void save(saver*s){
		::save(s, &_call, sizeof(_call));
		if(_call) _result.save(s);
		else _task.save(s);
	}

	void restore(restorer*r){
		::restore(r,&_call,sizeof(_call));
		if(_call) _result.restore(r);
		else _task.restore(r);
	}

	struct task{
		void save(saver*s){
/*$TET$task$save*/
/*$TET$*/
		}
		void restore(restorer*r){
/*$TET$task$restore*/
/*$TET$*/
		}
/*$TET$task$data*/
/*$TET$*/
	} _task;

	struct result{
		void save(saver*s){
/*$TET$result$save*/
/*$TET$*/
		}
		void restore(restorer*r){
/*$TET$result$restore*/
/*$TET$*/
		}
/*$TET$result$data*/
/*$TET$*/
	} _result;
	
	void call(){ _call = (_call == REPLY ? NEXT_CALL : FIRST_CALL);  TEMPLET::send(this, _master, _call); }
	void reply(){ _call = REPLY; TEMPLET::send(this, _worker, _call); }

	enum { FIRST_CALL, NEXT_CALL, REPLY } _call;
	actor* _master;
	actor* _worker;

	friend void save_adapter(message*m, saver*s){((task_result*)m)->save(s);}
	friend void restore_adapter(message*m, restorer*r){((task_result*)m)->restore(r);}
};

struct master : actor{

	master(engine*e) {::init(this, e, recv_master); _active = 0;}

	friend void recv_master(actor*a, message*m, int tag){((master*)a)->recv((task_result*)m, tag);}

	queue<task_result*> _wait;
	int _active;

	void recv(task_result*m, int tag){
		if (tag == task_result::FIRST_CALL){
			if (get(&m->_task)){
				m->reply(); _active++;
			}
			else
				_wait.push(m);
		}
		else if (tag == task_result::NEXT_CALL){
			put(&m->_result);
			_wait.push(m);

			while (!_wait.empty() && (m = _wait.front()) && get(&m->_task)){
				_wait.pop(); m->reply(); _active++;
			}

			if (!_active)TEMPLET::stop(this);
		}
	}

	bool get(task_result::task*t){
/*$TET$bag$get*/
/*$TET$*/	
	}

	void put(task_result::result*r){
/*$TET$bag$put*/
/*$TET$*/	
	}
	
	void save(saver*s){
/*$TET$bag$save*/
/*$TET$*/
	}
	
	void restore(restorer*r){
/*$TET$bag$restore*/
/*$TET$*/
	}
	
/*$TET$bag$data*/
/*$TET$*/
};

struct worker : actor{
public:
	worker(engine*e,master*m): _master(m) {
		::init(this, e, recv_worker, save_adapter, restore_adapter);
		_init = true;
	}

	friend void proc(task_result::task*t, task_result::result*r)
	{
/*$TET$proc$data*/
/*$TET$*/
	}

	friend void recv_worker (actor*a, message*m,int tag){
		task_result* tr = (task_result*)m;
		proc(&tr->_task,&tr->_result);
		tr->call();
	}

	void save(saver*s){if(_init)_master->save(s); _init=false;}
	void restore(restorer*r){if(_init)_master->restore(r); _init=false;}
	friend void save_adapter(actor*a, saver*s){((worker*)a)->save(s);}
	friend void restore_adapter(actor*a, restorer*r){((worker*)a)->restore(r);}

	bool _init;
	master* _master;
};

struct bag : engine{
	bag(int argc, char *argv[]){
		::init(this, argc, argv);
		_nworkers = ::nodes(this);

		_messages = new task_result*[_nworkers];
		_master = new master(this);
		_workers = new worker*[_nworkers];

		for (int i = 0; i < _nworkers; i++){
			_workers[i] = new worker(this,_master);
			_messages[i] = new task_result(this,_master,_workers[i]);
		}

		::at(_master, 0);
		for (int i = 0; i < _nworkers; i++)	::at(_workers[i], i);
	}

	~bag(){
		for (int i = 0; i < _nworkers; i++){delete _workers[i]; delete _messages[i];}

		delete[] _messages;
		delete _master;
		delete[] _workers;
	}

	void run(){	::map(this); ::run(this); }

	task_result** _messages;
	master* _master;
	worker** _workers;
	int _nworkers;
};

/*$TET$footer*/
/*$TET$*/
