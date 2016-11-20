/*$TET$header*/
#include <templet.hpp>
/*$TET$*/
#include <queue>
#include <thread>

using namespace std;
using namespace TEMPLET;

enum {FIRST_CALL, NEXT_CALL, REPLY};

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
};

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
};

class task_result :  public message{
public:
	task_result(engine*e,actor*m) : _master(m){
		::init(this, e, save_adapter, restore_adapter);
		_call = true;
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

	bool _call;
	task _task;
	result _result;
	actor* _master;////////////////////
	actor* _worker;

	friend void save_adapter(message*m, saver*s){((task_result*)m)->save(s);}
	friend void restore_adapter(message*m, restorer*r){((task_result*)m)->restore(r);}
};

class master;
class worker;

class bag : public engine{
public:
	bag(int argc, char *argv[]){
		::init(this,argc,argv);
		_worker_num = std::thread::hardware_concurrency();
		//_workers = new worker[_worker_num];

/*$TET$bag$init*/
/*$TET$*/
	}
	~bag(){
		delete [] _workers;
	}
	void run();
	void delay(double);

	bool get(task*t){
/*$TET$bag$get*/
/*$TET$*/	
	}
	void put(result*r){
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
	master* _master;
	worker* _workers;
	int _worker_num;
};

class master : public actor{
public:
	master(bag*b): _bag(b) {
		::init(this, b, recv_master, save_adapter, restore_adapter);
		_active = 0;
	}

	void save(saver*s){}
	void restore(restorer*r){}
	friend void save_adapter(actor*a, saver*s){((master*)a)->save(s);}
	friend void restore_adapter(actor*a, restorer*r){((master*)a)->restore(r);}

	bag* _bag;
	queue<task_result*> _wait;
	int _active;

	friend void recv_master (actor*a, message*m,int tag){
		master* mst = (master*)a;
		bag* bag = mst->_bag;
		task_result* tr=(task_result*)m;

		if(tag==FIRST_CALL){
			if(bag->get(&tr->_task)){
				::send(tr,a,REPLY); mst->active++;
			}
			else{
				tr->_worker=a; mst->_wait.push(tr);
			}
		}
		else if (tag==NEXT_CALL){
			bag->put(&tr->_result);
			tr->_worker=a; mst->_wait.push(tr);
			
			while(!mst->_wait.empty() && (tr=mst->_wait.back()) && bag->get(&tr->_task)){
				mst->_wait.pop(); 
				TEMPLET::send(tr, tr->_worker, REPLY);
				mst->_active++;
			}

			if(!mst->_active)::stop(a);
		}
	}
};

void proc(task*t,result*r)
{
/*$TET$proc$data*/
/*$TET$*/
}

class worker : public actor{
public:
	worker(bag*b): _bag(b),_master(b->_master) {
		::init(this, b, recv_worker, save_adapter, restore_adapter);
		_init = true;
	}

	friend void recv_worker (actor*a, message*m,int tag){
		worker* work = (worker*)a;
		task_result* tr = (task_result*)m;
		proc(&tr->_task,&tr->_result);
		tr->_call = true;
		::send(tr, work->_master, NEXT_CALL);
	}

	void save(saver*s){if(_init)_bag->save(s); _init=false;}
	void restore(restorer*r){if(_init)_bag->restore(r); _init=false;}
	friend void save_adapter(actor*a, saver*s){((worker*)a)->save(s);}
	friend void restore_adapter(actor*a, restorer*r){((worker*)a)->restore(r);}

	bool _init;
	master* _master;
	bag* _bag;
};

/*$TET$footer*/
/*$TET$*/
