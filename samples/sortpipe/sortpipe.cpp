/*$TET$pipeline*/
/*--------------------------------------------------------------------------*/
/*  Copyright 2016 Sergei Vostokin                                          */
/*                                                                          */
/*  Licensed under the Apache License, Version 2.0 (the "License");         */
/*  you may not use this file except in compliance with the License.        */
/*  You may obtain a copy of the License at                                 */
/*                                                                          */
/*  http://www.apache.org/licenses/LICENSE-2.0                              */
/*                                                                          */
/*  Unless required by applicable law or agreed to in writing, software     */
/*  distributed under the License is distributed on an "AS IS" BASIS,       */
/*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*/
/*  See the License for the specific language governing permissions and     */
/*  limitations under the License.                                          */
/*--------------------------------------------------------------------------*/
#include <iostream>
#include <omp.h>
#include <limits.h>
#include <time.h>

// insertion sort using a pipeline

const int N = 100000;

int  unsorted[N];
int  temp[N+1];
int  sorted[N];

int cur;

using namespace std;
/*$TET$*/

struct pipeline{
	pipeline(int num_stage, int num_iter)
	{
/*$TET$pipe$pipe*/
		srand((unsigned int)time(NULL));
		for (int i = 0; i<N; i++){
			unsorted[i] = rand();
			sorted[i] = INT_MAX;
		}
		sorted[0] = unsorted[0];
		cur = 1;
/*$TET$*/
	}

	void run();
	int  numthreads();

	void stage(int num){
/*$TET$pipe$stage*/
		num = num - 1;
		if(num == 0)temp[0] = unsorted[cur++];
		
		if (temp[num]>=sorted[num]) 
			temp[num + 1] = temp[num];
		else {
			temp[num + 1] = sorted[num];
			sorted[num] = temp[num];
		}
/*$TET$*/
	}
/*$TET$pipe$data*/
/*$TET$*/
};

/*$TET$footer*/
int main(int argc, char* argv[])
{
	pipeline p(N,N-1);

	std::cout << "input array size:"  << N << endl << endl;
	if(N <= 10){
		std::cout << "input array:"  << endl;
		for(int i=0;i<N;i++) cout << unsorted[i] << " ";
		cout << endl << endl;
	}

	double start = omp_get_wtime();
	p.run();
	double end = omp_get_wtime(); 

	std::cout << "parallel computation time is " << (end - start) << " seconds" << endl << endl;

	if(N <= 10){
		std::cout << "output array:"  << endl;
		for(int i=0;i<N;i++) cout << sorted[i] << " ";
		cout << endl;
	}

	return EXIT_SUCCESS;
}
/*$TET$*/
