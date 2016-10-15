/*$TET$$header*/
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

const int P=10;//число рабочих процессов (используется только в TEMPLET::stat)
const int N=10;
double a[N][N],b[N][N],c[N][N];
	
/*$TET$*/

using namespace TEMPLET;

class task{
	void save(saver*s){
/*$TET$task$save*/
	::save(s, &num, sizeof(num));
/*$TET$*/
	}			
	void restore(restorer*r){
/*$TET$task$restore*/
	::restore(r, &num, sizeof(num));
/*$TET$*/
	}
/*$TET$task$data*/
	int num;// номер вычисляемой строки матрицы c
/*$TET$*/
};

class result{
	void save(saver*s){
/*$TET$result$save*/
	::save(s, &num, sizeof(num)); // строка num 
	::save(s, &c[num], sizeof(double)*N); // матрицы c
/*$TET$*/
	}			
	void restore(restorer*r){
/*$TET$result$restore*/
	::restore(r, &num, sizeof(num)); // строка num 
	::restore(r, &c[num], sizeof(double)*N); // матрицы c
/*$TET$*/
	}
/*$TET$result$data*/
	int num; // номер вычисленной строки матрицы c
/*$TET$*/
};

class bag{
public:
	bag(int argc, char *argv[]){
/*$TET$bag$init*/
		cur = 0;
/*$TET$*/
	}
	void run();
	bool get(task*t){
/*$TET$bag$get*/
		if(cur<N){ t->num=cur++; return true;}
		else return false; 
/*$TET$*/	
	}
	void put(result*r){
/*$TET$bag$put*/
/*$TET$*/	
	}
/*$TET$bag$data*/
	int cur;//номер текущей строки в матрице С
/*$TET$*/	
};

void proc(task*t,result*r)
{
/*$TET$result$data*/
	int i=r->num=t->num;
	for(int j=0;j<N;j++){// параллельное вычисление строки матрицы C
		c[i][j]=0.0;
		for(int k=0;k<N;k++)c[i][j]+=a[i][k]*b[k][j];
	}
/*$TET$*/
}

/*$TET$templet$footer*/
int main(int argc, char* argv[])
{
	bag b(argc,argv);
	
	// инициализация
	for(int i=0;i<N;i++)
		for(int j=0;j<N;j++)a[i][j]=N*i+j;

	for(int i=0;i<N;i++)
		for(int j=0;j<N;j++)b[i][j]=N*i+j;

	// параллельное умножение матриц
	b.run();

	double T1, Tp, Smax, Sp;
	int Pmax;

	if (TEMPLET::stat(&b, &T1, &Tp, &Pmax, &Smax, P, &Sp)){
		std::cout << "T1 = " << T1 << ", Tp = " << Tp << ", Pmax = " << Pmax << ", Smax = " << Smax << ", P = " << P << ", Sp = " << Sp;
	}
		
	cout<<"\nduration = "<<bag.duration()<< " sec\n";

	// вывод результата параллельного умножения
	// отключить для больших N
	cout<<"\nC(parallel)=\n";
	for(int i=0;i<N;i++){
		for(int j=0;j<N;j++){
			cout<<c[i][j]<<" ";
		}
		cout<<'\n';
	}

	// очистка матрицы С
	for(int i=0;i<N;i++)
	for(int j=0;j<N;j++) c[i][j]=0.0;
	
	// последовательное умножение матриц
	for(int i=0;i<N;i++){
		for(int j=0;j<N;j++){
			c[i][j]=0.0;
			for(int k=0;k<N;k++)c[i][j]+=a[i][k]*b[k][j];
		}
	}

	// вывод результата последовательного умножения
	// отключить для больших N
	cout<<"\nC(serial)=\n";
	for(int i=0;i<N;i++){
		for(int j=0;j<N;j++){
			cout<<c[i][j]<<" ";
		}
		cout<<'\n';
	}
	return 0;
}
/*$TET$*/
