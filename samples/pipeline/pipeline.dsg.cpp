/*$TET$header*/
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
	int  numthreads();

	void stage(int num){
/*$TET$pipe$stage*/
		// the stage callback procedure
		// num -- the stage number to be activated (from 1 to num_stage)
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
/*$TET$*/
