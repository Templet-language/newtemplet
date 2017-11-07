/*$TET$actor*/
/*--------------------------------------------------------------------------*/
/*  Copyright 2017 Sergei Vostokin                                          */
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
#include <omp.h>

using namespace std;

const int N = 10;
int arr[N];

/*---------------------------------*/
#include <algorithm>

const int NUM_BLOCKS = 100;
const int BLOCK_SIZE = 10000;

int block_array[NUM_BLOCKS*BLOCK_SIZE];

void block_sort(int block_num)
{
	std::sort(&block_array[block_num*BLOCK_SIZE], &block_array[(block_num+1)*BLOCK_SIZE]);
}

void block_merge(int less_block_num, int more_block_num)
{
	int tmp_array[2*BLOCK_SIZE];
	std::merge(&block_array[less_block_num*BLOCK_SIZE], &block_array[(less_block_num + 1)*BLOCK_SIZE],
		&block_array[more_block_num*BLOCK_SIZE], &block_array[(more_block_num + 1)*BLOCK_SIZE],
		&tmp_array[0]);
	std::copy(&tmp_array[0], &tmp_array[BLOCK_SIZE], &block_array[less_block_num*BLOCK_SIZE]);
	std::copy(&tmp_array[BLOCK_SIZE], &tmp_array[2*BLOCK_SIZE], &block_array[more_block_num*BLOCK_SIZE]);
}

bool is_sorted()
{
	int prev = block_array[0];
	for (int i = 1; i < NUM_BLOCKS*BLOCK_SIZE; i++) {
		if (prev > block_array[i])return false;
		prev = block_array[i];
	}
	return true;
}
/*---------------------------------*/



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
		if (!(access(_in) && access(out)))return;

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

#pragma templet *stopper(in?mes)

struct stopper : actor_interface{
	stopper(engine_interface&){
/*$TET$stopper$stopper*/
/*$TET$*/
	}

	void in(mes&){}

	void in_handler(mes&m){
/*$TET$stopper$in*/
		number=m.number;
		stop();
/*$TET$*/
	}

/*$TET$stopper$$code&data*/
	int number;
/*$TET$*/
};

int main(int argc, char *argv[])
{
	//engine_interface e(argc, argv);
/*$TET$footer*/

	for (int i = 0; i < NUM_BLOCKS*BLOCK_SIZE; i++)	block_array[i] = rand();

	// generalized sequantial blocksort
	double time = omp_get_wtime();
	for (int i = 0; i<NUM_BLOCKS; i++) block_sort(i);
	for (int i = 1; i<NUM_BLOCKS; i++) for (int j = 0; j<i; j++) block_merge(j, i);
	time = omp_get_wtime() - time;

	if (!is_sorted())std::cout << "\nSomething went wrong!!!\n";
	else std::cout << "Sort time is " << time << " sec";
/*
	sorter** a_sorter = new sorter*[N-1];
	for(int i=0;i<N-1;i++)a_sorter[i]=new sorter(e);

	producer a_producer(e);
	stopper a_stoper(e);

	mes* prev=&a_producer.out;
	for(int i=0;i<N-1;i++){
		a_sorter[i]->in(*prev);
		prev=&(a_sorter[i]->out);
	}
	a_stoper.in(*prev);

	srand(0);
	for(int i=0;i<N;i++) arr[i]=rand();
	
	printf("before the sort: \n");
	for (int i = 0; i < N; i++) printf("%d\n", arr[i]);

	e.run();
	
	for(int i=0;i<N-1;i++){
		arr[i]=a_sorter[i]->number;
	}
	arr[N-1]=a_stoper.number;
	
	printf("\n\nafter the sort: \n");
	for(int i=0;i<N;i++) printf("%d\n",arr[i]);

	return 0;
	*/
/*$TET$*/
}
