/*$TET$dtaskbag*/
#include <templet.hpp>
// TODO: place additional headers and other stuff here
/*$TET$*/

using namespace TEMPLET;

struct task{
	void save(saver*s){
/*$TET$task$save*/
	// the task state
	// save -- saving the task before sending it to a worker process
	// TODO: define serialization code here
	// with ::save(s, &data, sizeof(data)); calls
/*$TET$*/
	}
	void restore(restorer*r){
/*$TET$task$restore*/
	// restore -- restoring the task state on a worker process
	// TODO: define deserialization code here
	// with ::restore(r, &data, sizeof(data)); calls
/*$TET$*/
	}
/*$TET$task$data*/
	// TODO: define task's data here
/*$TET$*/
};

struct result{
	void save(saver*s){
/*$TET$result$save*/
	// the result of the task execution
	// save -- saving the result before sending it to the master process
	// TODO: define serialization code here
	// with ::save(s, &data, sizeof(data)); calls
/*$TET$*/
	}
	void restore(restorer*r){
/*$TET$result$restore*/
	// restore -- restoring the result on the master process
	// TODO: define deserialization code here
	// with ::restore(r, &data, sizeof(data)); calls
/*$TET$*/
	}
/*$TET$result$data*/
	// TODO: define result data here
/*$TET$*/
};

struct bag{
	bag(int argc, char *argv[], int P=0);
	void run();
	void delay(double);
	double time();
	double speedup();

	bool get(task*t){
/*$TET$bag$get*/
	// states and methods of the master process
	// use delay(..) to spec 'get' or/and 'put' time in logical units
	// use time() to get the current time in logical units in 'get' or/and 'put' 
	// 'speedup();' is a speedup estimation
	// get -- task extraction method, if there is no task - it returns false
	// TODO: define getting a task from the the bag code (if return false -- no task have got) 
/*$TET$*/	
	}
	void put(result*r){
/*$TET$bag$put*/
	// put -- placing the result back to the bag
	// TODO: define putting result to the bag code
/*$TET$*/	
	}
	void save(saver*s){
/*$TET$bag$save*/
	// save -- saving worker processes common state method
	// TODO: define serialization code here
	// with ::save(s, &data, sizeof(data)); calls
	// to copy a part of the master process state to all worker processes
/*$TET$*/
	}
	void restore(restorer*r){
/*$TET$bag$restore*/
	// restore -- restoring worker processes common state method
	// TODO: define deserialization code here
	// with ::restore(r, &data, sizeof(data)); calls
	// to copy a part of the master process state to all worker processes
/*$TET$*/
	}
/*$TET$bag$data*/
	// TODO: define bag data here
/*$TET$*/	
};

void delay(double);
double time();

void proc(task*t,result*r)
{
/*$TET$proc$data*/
	// proc -- task execution procedure of a worker process 
	// TODO: define task processing code here
	// use delay(..) to spec 'proc' execution time in logical units
	// use time() to get the current time in logical units
/*$TET$*/
}

/*$TET$footer*/
	// TODO: define you program entry point
	// int main(int argc, char *argv[])
	// including
	// 1) the bag object creation
	// 2) an input code
	// 3) program run with a bag.run()
	// 4) an output code
	// 5) optional speedup estimation code
/*$TET$*/
