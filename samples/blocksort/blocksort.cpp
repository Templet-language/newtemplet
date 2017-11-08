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
#include <algorithm>

using namespace std;

const int NUM_BLOCKS = 2;
const int BLOCK_SIZE = 50000000;

//const int NUM_BLOCKS = 1;
//const int BLOCK_SIZE = 100000000;

int block_array[NUM_BLOCKS*BLOCK_SIZE];

void block_sort(int block_num)
{
	std::sort(&block_array[block_num*BLOCK_SIZE], &block_array[(block_num+1)*BLOCK_SIZE]);
}

void block_merge(int less_block_num, int more_block_num)
{
	static int tmp_array[2*BLOCK_SIZE];
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

/*$TET$*/

using namespace TEMPLET;

#pragma templet ~mes=

struct mes : message_interface{
/*$TET$mes$$data*/
	int block_j;
/*$TET$*/
};

#pragma templet *sorter(out!mes)+

struct sorter : actor_interface{
	sorter(engine_interface&){
/*$TET$sorter$sorter*/
/*$TET$*/
	}

	mes out;

	void start(){
/*$TET$sorter$start*/
		block_sort(block_i);
		out.send();
/*$TET$*/
	}

	void out_handler(mes&m){
/*$TET$sorter$out*/
/*$TET$*/
	}

/*$TET$sorter$$code&data*/
	int block_i;
/*$TET$*/
};

#pragma templet *producer(in?mes,out!mes)

struct producer : actor_interface{
	producer(engine_interface&){
/*$TET$producer$producer*/
		block_j = NUM_BLOCKS;
/*$TET$*/
	}

	void in(mes&){}
	mes out;

	void in_handler(mes&m){
/*$TET$producer$in*/
		block_j--;
		if (!block_j) start();
/*$TET$*/
	}

	void out_handler(mes&m){
/*$TET$producer$out*/
		if (block_j == NUM_BLOCKS) return;
		out.block_j = ++block_j;
		out.send();
/*$TET$*/
	}

/*$TET$producer$$code&data*/
	void start() {
		block_j = 0;
		out.block_j = block_j++;
		out.send();
	}

	int block_j;
/*$TET$*/
};

#pragma templet *merger(in?mes,out!mes)

struct merger : actor_interface{
	merger(engine_interface&){
/*$TET$merger$merger*/
		is_first=true;
		_in=0;
/*$TET$*/
	}

	void in(mes&){}
	mes out;

	void in_handler(mes&m){
/*$TET$merger$in*/
		_in=&m;
		sort();
/*$TET$*/
	}

	void out_handler(mes&m){
/*$TET$merger$out*/
		sort();
/*$TET$*/
	}

/*$TET$merger$$code&data*/
	void sort(){
		if (!(access(_in) && access(out)))return;

		if(is_first){
			is_first=false;
			_in->send();
		}
		else{
			block_merge(_in->block_j,block_i);
			out.block_j = _in->block_j;
			_in->send();out.send();
		}
	}

	int  block_i;
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
		stop();
/*$TET$*/
	}

/*$TET$stopper$$code&data*/
/*$TET$*/
};

int main(int argc, char *argv[])
{
	engine_interface e(argc, argv);
/*$TET$footer*/

	//for (int i = 0; i < NUM_BLOCKS*BLOCK_SIZE; i++)	block_array[i] = rand();

	//////////////// generalized sequantial blocksort /////////////
	double time = omp_get_wtime();
//#pragma omp parallel for
	//for (int i = 0; i<NUM_BLOCKS; i++) block_sort(i);
	//for (int i = 1; i<NUM_BLOCKS; i++) for (int j = 0; j<i; j++) block_merge(j, i);
	//time = omp_get_wtime() - time;

	//if (!is_sorted())std::cout << "\nSomething went wrong in the sequantial blocksort!!!\n";
	//else std::cout << "Sort time is " << time << " sec";
	///////////////////////////////////////////////////////////////

	/////////////////// parallel actor blocksort //////////////////
	producer a_producer(e);
	stopper a_stoper(e);

	sorter** a_sorter = new sorter*[NUM_BLOCKS - 1];
	for (int i = 0; i < NUM_BLOCKS - 1; i++) { 
		a_sorter[i] = new sorter(e); 
		a_sorter[i]->block_i = i;
		a_producer.in(a_sorter[i]->out);
	}

	merger** a_merger = new merger*[NUM_BLOCKS-1];
	for(int i=0;i<NUM_BLOCKS-1;i++)a_merger[i]=new merger(e);
	
	mes* prev=&a_producer.out;
	for(int i=0;i<NUM_BLOCKS -1;i++){
		a_merger[i]->in(*prev);
		a_merger[i]->block_i = i;
		prev=&(a_merger[i]->out);
	}
	a_stoper.in(*prev);

	for (int i = 0; i < NUM_BLOCKS*BLOCK_SIZE; i++)	block_array[i] = rand();
	
	time = omp_get_wtime();
	for (int i = 0; i<NUM_BLOCKS; i++) block_sort(i); /////////////
	e.run();
	time = omp_get_wtime() - time;

	if (!is_sorted())std::cout << "\nSomething went wrong in the parallel actor blocksort!!!\n";
	else std::cout << "Sort time is " << time << " sec";

	return 0;
	///////////////////////////////////////////////////////////////
	
/*$TET$*/
}
