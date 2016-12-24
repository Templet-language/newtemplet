/*$TET$header*/
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
#include <chrono>

// simulation of a calculation for an explicit FDM method using a pipeline
// the FTCS method for the heat equation 

const int N = 10;
const int T = 10;

double Mp[N][N];
double Ms[N][N];

using namespace std;
/*$TET$*/

struct pipeline{
	pipeline(int num_stage, int num_iter)
	{
/*$TET$pipe$pipe*/

/*$TET$*/
	}

	void run();
	void delay(double);
	double speedup();
	double speedup(int);
	int numthreads();

	void stage(int num){
/*$TET$pipe$stage*/

/*$TET$*/
	}
/*$TET$pipe$data*/

/*$TET$*/
};

/*$TET$footer*/
int main(int argc, char* argv[])
{
	double Tp, Ts;

	pipeline p(N,T);

	std::cout << "the task dimentions: N (number of stages) = " << N
		<< " T (number of data items passed thru) = " << T << endl;

	auto start = std::chrono::high_resolution_clock::now();
	p.run();
	auto end = std::chrono::high_resolution_clock::now();

	if (p.speedup() <= 0.0){
		Tp = ((double)std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000000);
		std::cout << "the parallel computation time is " << Tp << " seconds" << endl;

		start = std::chrono::high_resolution_clock::now();
		// sequential code
		end = std::chrono::high_resolution_clock::now();

		Ts = ((double)std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000000);
		std::cout << "the sequential computation time is " << Ts << " seconds" << endl;
	}

	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			if (Mp[i][j] != Ms[i][j]){
				printf("Something went wrong!!!\n");
				return EXIT_FAILURE;
			}
		}
	}


	if (p.speedup()>0.0){
		std::cout << "the maximum theoretical speedup is " << p.speedup() << endl;
		std::cout << "the theoretical speedup for " << p.numthreads() << " is " << p.speedup(p.numthreads()) << endl;
	}
	else
		std::cout << "the experimental speedup is " << Ts / Tp << "for " << p.numthreads() << "threads" << endl;

	return EXIT_SUCCESS;
}
/*$TET$*/
