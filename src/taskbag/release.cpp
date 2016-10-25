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
#include <templet.hpp>

// параллельное умножение матриц
// с использованием портфеля задач

const int P = 10;//число рабочих процессов (используется только в TEMPLET::stat)
const int N = 10;
double A[N][N], B[N][N], C[N][N];

using namespace std;

using namespace TEMPLET;

// состояние задачи
struct task{
// сохранение задачи перед отправкой рабочему процессу
	void save(saver*s){
		::save(s, &num, sizeof(num)); // строка num матрицы A
		::save(s, &A[num], sizeof(double)*N);
	}
// восстановление состояния задачи на рабочем процессе
	void restore(restorer*r){
		::restore(r, &num, sizeof(num)); // строка num матрицы A
		::restore(r, &A[num], sizeof(double)*N);
	}
	int num;// номер вычисляемой строки матрицы C
};

// результат выполнения задачи
struct result{
// сохранение результата перед отправкой управляющему процессу
	void save(saver*s){
		::save(s, &num, sizeof(num)); // строка num 
		::save(s, &C[num], sizeof(double)*N); // матрицы C
	}
// восстановление результата на управляющем процессе
	void restore(restorer*r){
		::restore(r, &num, sizeof(num)); // строка num 
		::restore(r, &C[num], sizeof(double)*N); // матрицы C
	}
	int num; // номер вычисленной строки матрицы c
};

// состояние и методы управляющего процесса
struct bag{
	bag(int argc, char *argv[]){
		cur = 0;
	}
	void run();
	void delay(double);
// метод извлечения задачи, если задачи нет -- возвращает false
	bool get(task*t){
		if (cur<N){ t->num = cur++; return true; }
		else return false;
	}
// метод помещения результата вычисления задачи
	void put(result*r){
	}
// сохранение состояния, общего для рабочих процессов
	void save(saver*s){
		::save(s, &B, sizeof(double)*N*N); // матрица B
	}
// восстановление общего состояния на рабочих процессах
	void restore(restorer*r){
		::restore(r, &B, sizeof(double)*N*N); // матрица B 
	}
	int cur;//номер текущей строки в матрице С
};

void delay(double);

// процедура выполнения задачи на рабочем процессе
void proc(task*t,result*r)
{
	int i = r->num = t->num;
	for (int j = 0; j<N; j++){// параллельное вычисление строки матрицы C
		C[i][j] = 0.0;
		for (int k = 0; k<N; k++)C[i][j] += A[i][k] * B[k][j];
	}
}

int main(int argc, char* argv[])
{
	bag b(argc, argv);

	// инициализация
	for (int i = 0; i<N; i++)
		for (int j = 0; j<N; j++)A[i][j] = N*i + j;

	for (int i = 0; i<N; i++)
		for (int j = 0; j<N; j++)B[i][j] = N*i + j;

	// параллельное умножение матриц
	b.run();

	double T1, Tp, Smax, Sp;
	int Pmax;

	if (TEMPLET::stat(&b, &T1, &Tp, &Pmax, &Smax, P, &Sp)){
		std::cout << "T1 = " << T1 << ", Tp = " << Tp << ", Pmax = " << Pmax << ", Smax = " << Smax << ", P = " << P << ", Sp = " << Sp;
	}

	// вывод результата параллельного умножения
	// отключить для больших N
	cout << "\nC(parallel)=\n";
	for (int i = 0; i<N; i++){
		for (int j = 0; j<N; j++){
			cout << C[i][j] << " ";
		}
		cout << '\n';
	}

	// очистка матрицы С
	for (int i = 0; i<N; i++)
		for (int j = 0; j<N; j++) C[i][j] = 0.0;

	// последовательное умножение матриц
	for (int i = 0; i<N; i++){
		for (int j = 0; j<N; j++){
			C[i][j] = 0.0;
			for (int k = 0; k<N; k++)C[i][j] += A[i][k] * B[k][j];
		}
	}

	// вывод результата последовательного умножения
	// отключить для больших N
	cout << "\nC(serial)=\n";
	for (int i = 0; i<N; i++){
		for (int j = 0; j<N; j++){
			cout << C[i][j] << " ";
		}
		cout << '\n';
	}
	return 0;
}
