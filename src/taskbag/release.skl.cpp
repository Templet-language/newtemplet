/*$TET$header*/
#include <templet.hpp>
/*$TET$*/
#include <queue>

using namespace std;
using namespace TEMPLET;

enum {FIRST_CALL, NEXT_CALL};

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

	friend void save_adapter(message*m, saver*s){((value_message*)m)->save(s);}
	friend void restore_adapter(message*m, restorer*r){((value_message*)m)->restore(r);}
};

class bag : public engine{
	bag(int argc, char *argv[]){
		::init(this,argc,argv);
/*$TET$bag$init*/
/*$TET$*/
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
		_wait.clear();
	}

	friend void recv_master (actor*a, message*m,int tag){
		task_result* tr=(task_result*)m;
		if(tag==FIRST_CALL){
			
		}
		else if (tag==NEXT_CALL){
			
		}
	}

	void save(saver*s){}
	void restore(restorer*r){}
	friend void save_adapter(actor*a, saver*s){((value_message*)a)->save(s);}
	friend void restore_adapter(actor*a, restorer*r){((value_message*)a)->restore(r);}

	bag* _bag;
	queue<pair<task_result*,actor*>> _wait;
};

void proc(task*t,result*r)
{
/*$TET$proc$data*/
/*$TET$*/
}

class worker : public actor{
public:
	worker(bag*b): _bag(b),_master(b->_master) {
		::init(this, b, recv_master, save_adapter, restore_adapter);
		_init = true;
	}

	friend void recv_worker (actor*a, message*m,int tag){
		task_result* tr = (task_result*)m;
		proc(&tr->_task,&tr->_result);
		tr->_call = true;
		::send(tr, _master, NEXT_CALL);
	}

	void save(saver*s){if(_init)_bag->save(s); _init=false;}
	void restore(restorer*r){if(_init)_bag->restore(s); _init=false;}
	friend void save_adapter(actor*a, saver*s){((value_message*)a)->save(s);}
	friend void restore_adapter(actor*a, restorer*r){((value_message*)a)->restore(r);}

	bool _init;
	master* _master;
	bag* _bag;
};

/*$TET$footer*/
/*$TET$*/
