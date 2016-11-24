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
#define TET_DEBUG_SERIALIZATION
#include <templet.hpp>

// параллельное умножение матриц
// с использованием портфеля задач

const int P = 10; // число рабочих процессов (используется только для прогноза ускорения)
const int N = 1000;
double A[N][N], B[N][N], C[N][N];

using namespace std;
/*$TET$*/

using namespace TEMPLET;

// the task state
struct task{
// saving the tasks before sending to a worker process
	void save(saver*s){
/*$TET$task$save*/
		::save(s, &num, sizeof(num)); // строка num
		::save(s, &A[num][0], sizeof(double)*N); // матрицы A
/*$TET$*/
	}
// restore the task state on a worker process
	void restore(restorer*r){
/*$TET$task$restore*/
		::restore(r, &num, sizeof(num)); // строка num
		::restore(r, &A[num][0], sizeof(double)*N); // матрицы A
/*$TET$*/
	}
/*$TET$task$data*/
	int num;// номер вычисляемой строки матрицы C
/*$TET$*/
};

// the result of task execution
struct result{
// save the result before sending to the master process
	void save(saver*s){
/*$TET$result$save*/
		::save(s, &num, sizeof(num)); // строка num 
		::save(s, &C[num][0], sizeof(double)*N); // матрицы C
/*$TET$*/
	}
// restore the result on the master process
	void restore(restorer*r){
/*$TET$result$restore*/
		::restore(r, &num, sizeof(num)); // строка num 
		::restore(r, &C[num][0], sizeof(double)*N); // матрицы C
/*$TET$*/
	}
/*$TET$result$data*/
	int num; // номер вычисленной строки матрицы C
/*$TET$*/
};

// states and methods of the master process
struct bag{
	bag(int argc, char *argv[]);
	void run();
	void delay();// use delay(..) to spec 'get' or/and 'put' time in logical units
	double speedup();// speedup estimation
	
// task extraction method, if there is no task - it returns false
	bool get(task*t){
/*$TET$bag$get*/
		delay(0.10);
		if (cur<N){ t->num = cur++; return true; }
		else return false;
/*$TET$*/
	}
// placing the result method
	void put(result*r){
/*$TET$bag$put*/
// в этом примере не требует определения
/*$TET$*/
	}
// saving worker processes common state method
	void save(saver*s){
/*$TET$bag$save*/
		::save(s, &B[0][0], sizeof(double)*N*N); // матрица B
/*$TET$*/
	}
//  restoring worker processes common state method
	void restore(restorer*r){
/*$TET$bag$restore*/
		::restore(r, &B[0][0], sizeof(double)*N*N); // матрица B 
/*$TET$*/
	}
/*$TET$bag$data*/
	int cur; // курсор по строкам матрицы C
/*$TET$*/
};

void delay(double);// use delay(..) to spec 'proc' time in logical units

// worker process task execution procedure
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
	bag b(argc, argv, P);
	b.cur = 0; // начинаем вычисление с нулевой строки

	// инициализация матриц
	for (int i = 0; i<N; i++){
		for (int j = 0; j<N; j++){
			A[i][j] = 2.0; B[i][j] = 5.0; C[i][j] = 0.0;
		}
	}

	// параллельное умножение матриц c замером времени
	auto start = std::chrono::high_resolution_clock::now();
	b.run();
	auto end = std::chrono::high_resolution_clock::now();

	std::cout << "multiplication time is "
		<< (double)std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000000
		<< " seconds\n";

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
	if ((Smax = b.speedup())>0.0) std::cout << "Smax = " << Smax << " (if use " << P << " workers)";

	return EXIT_SUCCESS;
}
/*$TET$*/
