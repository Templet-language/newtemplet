/*$TET$pipeline*/
/*$TET$*/

#include <omp.h>

struct pipeline{
	int T;
	int H;
	pipeline(int num_stage, int num_iter)
	{
		H = num_stage + 2;
		T = num_iter;
/*$TET$pipe$pipe*/
/*$TET$*/
	}

	void run()
	{
		#pragma omp parallel
		{
			for (int t = 1; t <= (2 * T - 1) + (H - 3); t++){

				if (t % 2 == 1){
					#pragma omp for schedule(dynamic)
					for (int i = 1; i < H - 1; i += 2)
						if (i <= t && i > t - 2 * T)	stage(i);
				}
				if (t % 2 == 0){
					#pragma omp for schedule(dynamic)
					for (int i = 2; i < H - 1; i += 2)
						if (i <= t && i > t - 2 * T)	stage(i);
				}
			}
		}
	}

	int numthreads(){ return omp_get_max_threads(); }

	void stage(int num){
/*$TET$pipe$stage*/
/*$TET$*/
	}
/*$TET$pipe$data*/
/*$TET$*/
};

/*$TET$footer*/
/*$TET$*/
