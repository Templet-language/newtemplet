/*$TET$taskbag*/
/*$TET$*/

#define USE_OPENMP
#include <templet.hpp>
#include <queue>

using namespace std;
using namespace TEMPLET;

struct task_result : message{

	task_result(engine*e,actor*m,actor*w): _master(m), _worker(w) {
		::init(this, w, e);
		_call = FIRST_CALL;
	}

	struct task{
/*$TET$task$data*/
/*$TET$*/
	} _task;

	struct result{
/*$TET$result$data*/
/*$TET$*/
	} _result;
	
	void call(){ _call = (_call == REPLY ? NEXT_CALL : FIRST_CALL);  TEMPLET::send(this, _master, _call); }
	void reply(){ _call = REPLY; TEMPLET::send(this, _worker, _call); }

	enum { FIRST_CALL, NEXT_CALL, REPLY } _call;
	actor* _master;
	actor* _worker;
};

struct worker;

struct bag : actor{

	bag(int num_procs=0);
	~bag();
	void delay(double t){ TEMPLET::delay(this,t); }
	double time(){ return TEMPLET::time(this); }
	double speedup(){
		double T1, Tp, Smax, Sp; int Pmax;
		if (TEMPLET::stat(&_engine, &T1, &Tp, &Pmax, &Smax, 1, &Sp)) return Smax; else return -1;
	}

	void run(){	::map(&_engine); ::run(&_engine); }

	friend void recv_master(actor*a, message*m, int tag){((bag*)a)->recv((task_result*)m, tag);}

	queue<task_result*> _wait;
	int _active;

	void recv(task_result*m, int tag){
		_active--;
		if (tag == task_result::FIRST_CALL){
			if (get(&m->_task)){
				m->reply();_active++;
			}
			else{
				_wait.push(m);
			}
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
	
/*$TET$bag$data*/
/*$TET$*/

	engine _engine;
	task_result** _messages;
	worker** _workers;
	int _nworkers;
};

struct worker : actor{
public:
	worker(engine*e,bag*b):_bag(b){
		::init(this, e, recv_worker);
		_init = true;
	}
	void delay(double t){ TEMPLET::delay(this,t); }
	double time() { return TEMPLET::time(this); }

	void proc(task_result::task*t, task_result::result*r)
	{
/*$TET$proc$data*/
/*$TET$*/
	}

	friend void recv_worker (actor*a, message*m,int tag){
		task_result* tr = (task_result*)m;
		worker* w = (worker*)a;
		w->proc(&tr->_task,&tr->_result);
		tr->call();
	}

	bag* _bag;
	bool _init;
};

inline bag::bag(int num_procs) {
	::init(&_engine, 0, 0);
	::init(this, &_engine, recv_master);

	if (num_procs == 0) 
		_nworkers = omp_get_num_procs();
	else
		_nworkers = num_procs;

	_messages = new task_result*[_nworkers];
	_workers = new worker*[_nworkers];

	for (int i = 0; i < _nworkers; i++){
		_workers[i] = new worker(&_engine,this);
		_messages[i] = new task_result(&_engine,this,_workers[i]);
	}

	for (int i = 0; i < _nworkers; i++)_messages[i]->call();
	_active = _nworkers;
}

inline bag::~bag(){
	for (int i = 0; i < _nworkers; i++){delete _workers[i]; delete _messages[i];}
	delete[] _messages;
	delete[] _workers;
}

/*$TET$footer*/
/*$TET$*/
