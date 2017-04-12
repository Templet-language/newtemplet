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
#include <templet.hpp>

using namespace std;

const int N = 10;
int arr[N];
/*$TET$*/

using namespace TEMPLET;

class my_engine{
public:
	my_engine(int argc, char *argv[]);
	void run();
	void map();
};

#pragma templet ~mes=

struct mes : message{
	bool access(actor*);
	void send();

/*$TET$mes$$data*/
	int number;
/*$TET$*/
};

#pragma templet *producer(out!mes)+

struct producer : actor{
	producer(my_engine&){
/*$TET$producer$producer*/
/*$TET$*/
	}

	void delay(double);
	double time();
	void at(int);
	void stop();

	mes* out();

	void start(){
/*$TET$producer$start*/
		cur = 0;
		_out.number = arr[cur++];
		_out.send();
/*$TET$*/
	}

	void out(mes&m){
/*$TET$producer$out*/
		if (cur == N) return;
		_out.number = arr[cur++];
		_out.send();
/*$TET$*/
	}

/*$TET$producer$$code&data*/
	int cur;
/*$TET$*/

	mes _out;
};

#pragma templet *sorter(in?mes,out!mes)

struct sorter : actor{
	sorter(my_engine&){
/*$TET$sorter$sorter*/
		is_first=true;
		_in=0;
/*$TET$*/
	}

	void delay(double);
	double time();
	void at(int);
	void stop();

	void in(mes*);
	mes* out();

	void in(mes&m){
/*$TET$sorter$in*/
		_in=&m;
		if(_out.access(this))proc();
/*$TET$*/
	}

	void out(mes&m){
/*$TET$sorter$out*/
		if(_in->access(this))proc();
/*$TET$*/
	}

/*$TET$sorter$$code&data*/
	void proc(){
		if(is_first){
			number=_in->number;
			is_first=false;
			_in->send();
		}
		else{
			if(number <= _in->number){
				_out.number = _in->number;
				_in->send();_out.send();
			}
			else{
				_out.number = number;
				number = _in->number;
				_in->send();_out.send();
			}
		}
	}

	int number;
	bool is_first;
	mes* _in;
/*$TET$*/

	mes _out;
};

#pragma templet *stoper(in?mes)

struct stoper : actor{
	stoper(my_engine&){
/*$TET$stoper$stoper*/
/*$TET$*/
	}

	void delay(double);
	double time();
	void at(int);
	void stop();

	void in(mes*);

	void in(mes&m){
/*$TET$stoper$in*/
		number=m.number;
		stop();
/*$TET$*/
	}

/*$TET$stoper$$code&data*/
	int number;
/*$TET$*/

};

/*$TET$footer*/
int main(int argc, char *argv[])
{
	my_engine e(argc, argv);
	
	sorter** a_sorter = new sorter*[N-1];
	for(int i=0;i<N-1;i++)a_sorter[i]=new sorter(e);

	producer a_producer(e);
	stoper an_endpoint(e);

	mes* prev=a_producer.out();
	for(int i=0;i<N-1;i++){
		a_sorter[i]->in(prev);
		prev=a_sorter[i]->out();
	}
	an_endpoint.in(prev);

	srand(0);
	for(int i=0;i<N;i++) arr[i]=rand();
	
	printf("before the sort: \n");
	for (int i = 0; i < N; i++) printf("%d\n", arr[i]);

	e.run();
	
	for(int i=0;i<N-1;i++){
		arr[i]=a_sorter[i]->number;
	}
	arr[N-1]=an_endpoint.number;
	
	printf("\n\nafter the sort: \n");
	for(int i=0;i<N;i++) printf("%d\n",arr[i]);
	

	return 0;
}
/*$TET$*/
