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
#include <queue>

using namespace std;
using namespace TEMPLET;

enum {FIRST_CALL, NEXT_CALL,REPLY};

struct task{
	void save(saver*s){
		::save(s, &num, sizeof(num)); // строка num
		::save(s, &A[num], sizeof(double)*N); // матрицы A
	}
	void restore(restorer*r){
		::restore(r, &num, sizeof(num)); // строка num
		::restore(r, &A[num], sizeof(double)*N); // матрицы A
	}
	int num;// номер вычисляемой строки матрицы C
};

struct result{
	void save(saver*s){
		::save(s, &num, sizeof(num)); // строка num 
		::save(s, &C[num], sizeof(double)*N); // матрицы C
	}
	void restore(restorer*r){
		::restore(r, &num, sizeof(num)); // строка num 
		::restore(r, &C[num], sizeof(double)*N); // матрицы C
	}
	int num; // номер вычисленной строки матрицы C
};

class task_result :  public message{
public:
	task_result(engine*e,actor*m) : _master(m){
		::init(this, e, save_adapter, restore_adapter);
		_call = true;
	}
	void save(saver*s){
		::save(s, &_call, sizeof(_call));
		if(_call) _result.save(s);
		else _task.save(s);
	}
	void restore(restorer*r){
		::restore(r,&_call,sizeof(_call));
		if(_call) _result.restore(r);
		else _task.restore(r);
	}

	bool _call;
	task _task;
	result _result;
	actor* _worker;

	friend void save_adapter(message*m, saver*s){((value_message*)m)->save(s);}
	friend void restore_adapter(message*m, restorer*r){((value_message*)m)->restore(r);}
};

class bag : public engine{
	bag(int argc, char *argv[]){
		::init(this,argc,argv);
		cur = 0; // начинаем вычисление с нулевой строки
	}
	~bag(){
		delete [] _workers;
	}
	void run();
	void delay(double);

	bool get(task*t){
		if (cur<N){ t->num = cur++; return true; }
		else return false;
	}
	void put(result*r){
// в этом примере не требует определения
	}
	void save(saver*s){
		::save(s, &B, sizeof(double)*N*N); // матрица B
	}
	void restore(restorer*r){
		::restore(r, &B, sizeof(double)*N*N); // матрица B 
	}
	int cur; // курсор по строкам матрицы C
	master* _master;
	worker* _workers;
	int _worker_num;
};

class master : public actor{
public:
	master(bag*b): _bag(b) {
		::init(this, b, recv_master, save_adapter, restore_adapter);
		_wait.clear(); _active = 0;
	}

	friend void recv_master (actor*a, message*m,int tag){
		task_result* tr=(task_result*)m;
		if(tag==FIRST_CALL){
			if(_bag->get(tr->_task)){
				::send(tr,a,REPLY);	active++;
			}
			else{
				tr->_worker=a;_wait.push(tr);
			}
		}
		else if (tag==NEXT_CALL){
			_bag->put(tr->_result);
			tr->_worker=a; _wait.push(tr);
			
			while(!_queue.empty() && tr=_queue.back() && _bag->get(tr)){
				_queue.pop(); ::send(tr,tr->_worker,REPLY); _active++;
			}

			if(!active)::stop(_bag);
		}
	}

	void save(saver*s){}
	void restore(restorer*r){}
	friend void save_adapter(actor*a, saver*s){((value_message*)a)->save(s);}
	friend void restore_adapter(actor*a, restorer*r){((value_message*)a)->restore(r);}

	bag* _bag;
	queue<task_result*> _wait;
	int _active;
};

void proc(task*t,result*r)
{
	int i = r->num = t->num;
	for (int j = 0; j<N; j++){// параллельное вычисление строки матрицы C
		C[i][j] = 0.0;
		for (int k = 0; k<N; k++)C[i][j] += A[i][k] * B[k][j];
	}
}

class worker : public actor{
public:
	worker(bag*b): _bag(b),_master(b->_master) {
		::init(this, b, recv_master, save_adapter, restore_adapter);
		_init = true;
	}

	friend void recv_worker (actor*a, message*m,int tag){
		task_result* tr = (task_result*)m;
		proc(&tr->_task,&tr->_result);
		tr->_call = true;
		::send(tr, _master, NEXT_CALL);
	}

	void save(saver*s){if(_init)_bag->save(s); _init=false;}
	void restore(restorer*r){if(_init)_bag->restore(s); _init=false;}
	friend void save_adapter(actor*a, saver*s){((value_message*)a)->save(s);}
	friend void restore_adapter(actor*a, restorer*r){((value_message*)a)->restore(r);}

	bool _init;
	master* _master;
	bag* _bag;
};

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
