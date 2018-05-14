/*$TET$taskbag*/
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

//#define SIMULATED_EXECUTION
#define PARALLEL_EXECUTION

// параллельное умножение матриц
// с использованием портфеля задач
// в общей памяти

const int P = 10;// используется с #define SIMULATED_EXECUTION
const int N = 1000;
double A[N][N], B[N][N], C[N][N];

using namespace std;
/*$TET$*/

struct task{
/*$TET$task$data*/
	int num;// номер вычисляемой строки матрицы C
/*$TET$*/
};

struct result{
/*$TET$result$data*/
	int num; // номер вычисленной строки матрицы C
/*$TET$*/
};

struct bag{
	bag(int num_procs=0);
	void run();
	void delay(double);
	double time();
	double speedup();

	bool get(task*t){
/*$TET$bag$get*/
		delay(0.10);
		if (cur<N){ t->num = cur++; return true; }
		else return false;
/*$TET$*/
	}
	void put(result*r){
/*$TET$bag$put*/
// в этом примере не требует определения
/*$TET$*/
	}
/*$TET$bag$data*/
	int cur; // курсор по строкам матрицы C
/*$TET$*/
};

void delay(double);
double time();

void proc(task*t,result*r)
{
/*$TET$proc$data*/
	delay(1.00);
	int i = r->num = t->num;
	for (int j = 0; j<N; j++){// параллельное вычисление строки матрицы C
		C[i][j] = 0.0;
		for (int k = 0; k<N; k++)C[i][j] += A[i][k] * B[k][j];
	}
/*$TET$*/
}

/*$TET$footer*/
int main(int argc, char* argv[])
{
#ifdef SIMULATED_EXECUTION
	bag b(P);
#else
	bag b;
#endif

	b.cur = 0; // начинаем вычисление с нулевой строки

	// инициализация матриц
	for (int i = 0; i<N; i++){
		for (int j = 0; j<N; j++){
			A[i][j] = 2.0; B[i][j] = 5.0; C[i][j] = 0.0;
		}
	}

	// параллельное умножение матриц c замером времени
	double start = omp_get_wtime();
	b.run();
	double end = omp_get_wtime();

	std::cout << "multiplication time is "
		<< (end - start) << " seconds\n";

	// проверяем результат
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			if (C[i][j] != 10.0*N){
				printf("Something went wrong!!!\n");
				return EXIT_FAILURE;
			}
		}
	}
	
	// узнаём прогноз ускорения алгоритма (Smax), если накладные расходы составляют 10% от времени вычисления 
	double Smax;
	if ((Smax = b.speedup())>0.0) std::cout << "Smax = " << Smax << " for "<< P << " processors" << '\n';

	return EXIT_SUCCESS;
}
/*$TET$*/
