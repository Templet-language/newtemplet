/*$TET$header*/
#include <templet.hpp>
// TODO: place additional headers and other stuff here
/*$TET$*/

struct pipeline{
	pipeline(int num_stage, int num_iter)
	{
/*$TET$pipe$pipe*/
		// place custom constructor here
		// num_stage -- number of stages in the pipeline
		// num_iter  -- number of data items passed thru the pipeline 
/*$TET$*/
	}

	void run();
	void delay(double);
	double speedup();
	double speedup(int);
	int numthreads();

	void stage(int num){
/*$TET$pipe$stage*/
		// the stage callback procedure
		// num -- the stage number to be activated (from 0 to num_stage-1)
		// use the 'delay(..)' method to specify a delay in logical time units (in this method)
		// use the 'speedup()' method to estimate the theoretical speedup 
		//    (after the run method; for any or given number of threads)
		// use the 'numthreads()' to get the number of system threads (after run method)
/*$TET$*/
	}
/*$TET$pipe$data*/
		// the pipeline data
/*$TET$*/
};

/*$TET$footer*/
	// TODO: define you program entry point
	// int main(int argc, char *argv[])
	// including
	// 1) the pipeline object creation
	// 2) an input code
	// 3) the program run with a .run()
	// 4) an output code
	// 5) an optional speedup estimation code
/*$TET$*/
