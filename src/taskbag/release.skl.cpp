/*$TET$$header*/
	// TODO: place additional headers and other stuff here
#include <templet.hpp>
	//	is often needed
/*$TET$*/

using namespace TEMPLET;

class task{
	void save(saver*s){
/*$TET$task$save*/
	// TODO: define serialization code here
	// with ::save(s, &data, sizeof(data)); calls
/*$TET$*/
	}			
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

class result{
	void save(saver*s){
/*$TET$result$save*/
	// TODO: define serialization code here
	// with ::save(s, &data, sizeof(data)); calls
/*$TET$*/
	}			
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

class bag{
public:
	bag(int argc, char *argv[]);
	void run();
	bool get(task*t){
/*$TET$bag$get*/
	// TODO: define getting a task from the the bag code (if return false -- no task have got) 
/*$TET$*/	
	}
	void put(result*r){
/*$TET$bag$put*/
	// TODO: define putting result to the bag code
/*$TET$*/	
	}
/*$TET$bag$data*/
	// TODO: define bag data here
/*$TET$*/	
};

void proc(task*t,result*r)
{
/*$TET$result$data*/
	// TODO: define task processing code here
/*$TET$*/
}

/*$TET$templet$footer*/
	// TODO: define you program entry point
	// int main(int argc, char *argv[])
	// including
	// 1) creating the bag bject
	// 2) input code
	// 3) run with bag.run()
	// 4) output code
	// 5) performance statistics code with ::stat(...)
/*$TET$*/