/*$TET$actor*/
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

#pragma templet ~mes=

struct mes : message_interface{
/*$TET$mes$$data*/
	int number;
/*$TET$*/
};

#pragma templet *producer(out!mes)+

struct producer : actor_interface{
	producer(engine_interface&){
/*$TET$producer$producer*/
/*$TET$*/
	}

	mes out;

	void start(){
/*$TET$producer$start*/
		cur = 0;
		out.number = arr[cur++];
		out.send();
/*$TET$*/
	}

	void out_handler(mes&m){
/*$TET$producer$out*/
		if (cur == N) return;
		out.number = arr[cur++];
		out.send();
/*$TET$*/
	}

/*$TET$producer$$code&data*/
	int cur;
/*$TET$*/
};

#pragma templet *sorter(in?mes,out!mes)

struct sorter : actor_interface{
	sorter(engine_interface&){
/*$TET$sorter$sorter*/
		is_first=true;
		_in=0;
/*$TET$*/
	}

	void in(mes&){}
	mes out;

	void in_handler(mes&m){
/*$TET$sorter$in*/
		_in=&m;
		sort();
/*$TET$*/
	}

	void out_handler(mes&m){
/*$TET$sorter$out*/
		sort();
/*$TET$*/
	}

/*$TET$sorter$$code&data*/
	void sort(){
		if (!(access(_in) && access(&out)))return;

		if(is_first){
			number=_in->number;
			is_first=false;
			_in->send();
		}
		else{
			if(number <= _in->number){
				out.number = _in->number;
				_in->send();out.send();
			}
			else{
				out.number = number;
				number = _in->number;
				_in->send();out.send();
			}
		}
	}

	int number;
	bool is_first;
	mes* _in;
/*$TET$*/
};

#pragma templet *stoper(in?mes)

struct stoper : actor_interface{
	stoper(engine_interface&){
/*$TET$stoper$stoper*/
/*$TET$*/
	}

	void in(mes&){}

	void in_handler(mes&m){
/*$TET$stoper$in*/
		number=m.number;
		stop();
/*$TET$*/
	}

/*$TET$stoper$$code&data*/
	int number;
/*$TET$*/
};

int main(int argc, char *argv[])
{
	engine_interface e(argc, argv);
/*$TET$footer*/

	sorter** a_sorter = new sorter*[N-1];
	for(int i=0;i<N-1;i++)a_sorter[i]=new sorter(e);

	producer a_producer(e);
	stoper an_endpoint(e);

	mes* prev=&a_producer.out;
	for(int i=0;i<N-1;i++){
		a_sorter[i]->in(*prev);
		prev=&(a_sorter[i]->out);
	}
	an_endpoint.in(*prev);

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
/*$TET$*/
}
