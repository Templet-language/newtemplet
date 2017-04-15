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

// simulation of a calculation for an explicit FDM method using a pipeline
// the FTCS method for the heat equation 

const int SCALE = 1000;

const int W = SCALE*2;
const int H = SCALE;
const int T = SCALE*2;

double Fp[H][W];
double Fs[H][W];

using namespace std;
/*$TET$*/

struct pipeline{
	pipeline(int num_stage, int num_iter)
	{
/*$TET$pipe$pipe*/
		srand(0);
		for (int i = 0; i<H; i++)
		for (int j = 0; j<W; j++) Fs[i][j] = rand();
		srand(0);
		for (int i = 0; i<H; i++)
		for (int j = 0; j<W; j++) Fp[i][j] = rand();
/*$TET$*/
	}

	void run();
	int  numthreads();

	void stage(int num){
/*$TET$pipe$stage*/
		for (int j = 1; j < W - 1; j++)
			Fp[num][j] = (Fp[num][j - 1] + Fp[num][j + 1] + Fp[num - 1][j] + Fp[num + 1][j])*0.25;
/*$TET$*/
	}
/*$TET$pipe$data*/
/*$TET$*/
};

/*$TET$footer*/
int main(int argc, char* argv[])
{
	double Tp, Ts;

	pipeline p(H-2,T);

	std::cout << "task dimentions:" << endl
		<< "N (number of stages) = " << H - 2 << endl
		<< "T (number of data items passed thru the pipe) = " << T << endl;

	double start = omp_get_wtime();
	p.run();
	double end = omp_get_wtime(); 

	Tp = end - start;
	std::cout << "parallel computation time is " << Tp << " seconds" << endl;

	start = omp_get_wtime();
	
	for (int t = 1; t <= T; t++)
		for (int i = 1; i < H - 1; i++)
			for (int j = 1; j < W - 1; j++)
				Fs[i][j] = (Fs[i][j - 1] + Fs[i][j + 1] + Fs[i - 1][j] + Fs[i + 1][j])*0.25;
				
	end = omp_get_wtime();

	Ts = end - start;
	std::cout << "sequential computation time is " << Ts << " seconds" << endl;
		
	for (int i = 0; i < H; i++){
		for (int j = 0; j < W; j++){
			if (Fp[i][j] != Fs[i][j]){
				printf("Something went wrong!!!\n");
				return EXIT_FAILURE;
			}
		}
	}

	double alfa = ((double)(2 * T - 1) + (H - 3)) / (T * (H - 2));
	double n = p.numthreads();
	
	std::cout << "maximum theoretical speedup is " << 1 / alfa << endl;
	std::cout << "theoretical speedup for " << n << " threads is " << (1/(alfa + (1-alfa)/n)) << endl;
	std::cout << "experimental speedup is " << Ts / Tp << " for " << n << " threads" << endl;

	return EXIT_SUCCESS;
}
/*$TET$*/
