/*$TET$header*/
	// TODO: place additional headers and other stuff here
#include <templet.hpp>
	//	is often needed
// options:
//  -- use or do not use comments
//  -- language (if use comments)
//  -- DM or SHM deployment
//  -- copy or do not copy common state to workers (if DM deployment)
//  -- use or do not use 'put' method
//  -- use or do not use 'if_task' method
/*$TET$*/

using namespace TEMPLET;

// the task state
struct task{
// saving the tasks before sending to a worker process
	void save(saver*s){
/*$TET$task$save*/
	// TODO: define serialization code here
	// with ::save(s, &data, sizeof(data)); calls
/*$TET$*/
	}
// restore the task state on a worker process
	void restore(restorer*r){
/*$TET$task$restore*/
	// TODO: define deserialization code here
	// with ::restore(r, &data, sizeof(data)); calls
/*$TET$*/
	}
/*$TET$task$data*/
	// TODO: define task data here
/*$TET$*/
};

// the result of task execution
struct result{
// save the result before sending to the master process
	void save(saver*s){
/*$TET$result$save*/
	// TODO: define serialization code here
	// with ::save(s, &data, sizeof(data)); calls
/*$TET$*/
	}
// restore the result on the master process
	void restore(restorer*r){
/*$TET$result$restore*/
	// TODO: define deserialization code here
	// with ::restore(r, &data, sizeof(data)); calls
/*$TET$*/
	}
/*$TET$result$data*/
	// TODO: define result data here
/*$TET$*/
};

// states and methods of the master process
struct bag{
	bag(int argc, char *argv[]){
/*$TET$bag$init*/
	// TODO: place initialization code here
/*$TET$*/
	}
	void run();
	
// task extraction method, if there is no task - it returns false
	bool get(task*t){
/*$TET$bag$get*/
	// TODO: define getting a task from the the bag code (if return false -- no task have got) 
/*$TET$*/	
	}
// placing the result method
	void put(result*r){
/*$TET$bag$put*/
	// TODO: define putting result to the bag code
/*$TET$*/	
	}
// saving worker processes common state method
	void save(saver*s){
/*$TET$bag$save*/
	// TODO: define serialization code here
	// with ::save(s, &data, sizeof(data)); calls
	// to copy a part of the master process state to all worker processes
/*$TET$*/
	}
//  restoring worker processes common state method
	void restore(restorer*r){
/*$TET$bag$restore*/
	// TODO: define deserialization code here
	// with ::restore(r, &data, sizeof(data)); calls
	// to copy a part of the master process state to all worker processes
/*$TET$*/
	}
/*$TET$bag$data*/
	// TODO: define bag data here
/*$TET$*/	
};

// worker process task execution procedure
void proc(task*t,result*r)
{
/*$TET$proc$data*/
	// TODO: define task processing code here
/*$TET$*/
}

/*$TET$footer*/
	// TODO: define you program entry point
	// int main(int argc, char *argv[])
	// including
	// 1) creating the bag object
	// 2) input code
	// 3) run with bag.run()
	// 4) output code
/*$TET$*/