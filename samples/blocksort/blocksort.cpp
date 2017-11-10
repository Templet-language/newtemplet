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

#define  PARALLEL_EXECUTION
#define  USE_OPENMP

//#define SIMULATED_EXECUTION
//const double TRADEOFF = 2.5;

#include <templet.hpp>

#include <omp.h>
#include <algorithm>

using namespace std;

const int NUM_BLOCKS = 12;
const int BLOCK_SIZE = 10000000;

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

/*$TET$*/

using namespace TEMPLET;

struct my_engine : engine{
	my_engine(int argc, char *argv[]){
		::init(this, argc, argv);
	}
	void run(){ TEMPLET::run(this); }
	void map(){ TEMPLET::map(this); }
};

#pragma templet ~mes=

struct mes : message{
	mes(actor*a, engine*e, int t) : _where(CLI), _cli(a), _client_id(t){
		::init(this, a, e);
	}

	void send(){
		if (_where == CLI){ TEMPLET::send(this, _srv, _server_id); _where = SRV; }
		else if (_where == SRV){ TEMPLET::send(this, _cli, _client_id); _where = CLI; }
	}

/*$TET$mes$$data*/
	int i;
/*$TET$*/

	enum { CLI, SRV } _where;
	actor* _srv;
	actor* _cli;
	int _client_id;
	int _server_id;
};

#pragma templet *sorter(out!mes)+

struct sorter : actor{
	enum tag{START,TAG_out};

	sorter(my_engine&e):out(this, &e, TAG_out){
		::init(this, &e, sorter_recv_adapter);
		::init(&_start, this, &e);
		::send(&_start, this, START);
/*$TET$sorter$sorter*/
/*$TET$*/
	}

	bool access(message*m){ return TEMPLET::access(m, this); }
	bool access(message&m){ return TEMPLET::access(&m, this); }

	void at(int _at){ TEMPLET::at(this, _at); }
	void delay(double t){ TEMPLET::delay(this, t); }
	double time(){ return TEMPLET::time(this); }
	void stop(){ TEMPLET::stop(this); }

	mes out;

	static void sorter_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_out: ((sorter*)a)->out_handler(*((mes*)m)); break;
			case START: ((sorter*)a)->start(); break;
		}
	}

	void start(){
/*$TET$sorter$start*/
#if defined(SIMULATED_EXECUTION)
		double time;
		time = omp_get_wtime();
#endif
		block_sort(i);  // <-- code fragment 2
#if defined(SIMULATED_EXECUTION)
		time = omp_get_wtime() - time;
		delay(time*(1+TRADEOFF));
#endif
		out.send();
/*$TET$*/
	}

	void out_handler(mes&m){
/*$TET$sorter$out*/
/*$TET$*/
	}

/*$TET$sorter$$code&data*/
	int i;
/*$TET$*/
	message _start;
};

#pragma templet *producer(in?mes,out!mes)

struct producer : actor{
	enum tag{START,TAG_in,TAG_out};

	producer(my_engine&e):out(this, &e, TAG_out){
		::init(this, &e, producer_recv_adapter);
/*$TET$producer$producer*/
		bc = NUM_BLOCKS;
		i = 0;
/*$TET$*/
	}

	bool access(message*m){ return TEMPLET::access(m, this); }
	bool access(message&m){ return TEMPLET::access(&m, this); }

	void at(int _at){ TEMPLET::at(this, _at); }
	void delay(double t){ TEMPLET::delay(this, t); }
	double time(){ return TEMPLET::time(this); }
	void stop(){ TEMPLET::stop(this); }

	void in(mes&m){m._server_id=TAG_in; m._srv=this;}
	mes out;

	static void producer_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_in: ((producer*)a)->in_handler(*((mes*)m)); break;
			case TAG_out: ((producer*)a)->out_handler(*((mes*)m)); break;
		}
	}

	void in_handler(mes&m){
/*$TET$producer$in*/
		bc--;
		if (!bc) out_handler(m);
/*$TET$*/
	}

	void out_handler(mes&m){
/*$TET$producer$out*/
		if (i == NUM_BLOCKS) return;
		out.i = i++;
		out.send();
/*$TET$*/
	}

/*$TET$producer$$code&data*/
	int i,bc;
/*$TET$*/
};

#pragma templet *merger(in?mes,out!mes)

struct merger : actor{
	enum tag{START,TAG_in,TAG_out};

	merger(my_engine&e):out(this, &e, TAG_out){
		::init(this, &e, merger_recv_adapter);
/*$TET$merger$merger*/
		is_first=true;
		_in=0;
/*$TET$*/
	}

	bool access(message*m){ return TEMPLET::access(m, this); }
	bool access(message&m){ return TEMPLET::access(&m, this); }

	void at(int _at){ TEMPLET::at(this, _at); }
	void delay(double t){ TEMPLET::delay(this, t); }
	double time(){ return TEMPLET::time(this); }
	void stop(){ TEMPLET::stop(this); }

	void in(mes&m){m._server_id=TAG_in; m._srv=this;}
	mes out;

	static void merger_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_in: ((merger*)a)->in_handler(*((mes*)m)); break;
			case TAG_out: ((merger*)a)->out_handler(*((mes*)m)); break;
		}
	}

	void in_handler(mes&m){
/*$TET$merger$in*/
		_in=&m;
		merge();
/*$TET$*/
	}

	void out_handler(mes&m){
/*$TET$merger$out*/
		merge();
/*$TET$*/
	}

/*$TET$merger$$code&data*/
	void merge(){
		if (!(access(_in) && access(out)))return;        // <-- code fragment 3 begin

		if(is_first){
			is_first=false;	j = _in->i;
			_in->send();
		}
		else{
#if defined(SIMULATED_EXECUTION)
			double time;
			time = omp_get_wtime();
#endif
			block_merge(j,_in->i);
#if defined(SIMULATED_EXECUTION)
			time = omp_get_wtime() - time;
			delay(time*(1 + TRADEOFF));
#endif
			out.i = _in->i;
			_in->send();out.send();
		}                                                // <-- code fragment 3 end
	}

	int  j;
	bool is_first;
	mes* _in;
/*$TET$*/
};

#pragma templet *stopper(in?mes)

struct stopper : actor{
	enum tag{START,TAG_in};

	stopper(my_engine&e){
		::init(this, &e, stopper_recv_adapter);
/*$TET$stopper$stopper*/
/*$TET$*/
	}

	bool access(message*m){ return TEMPLET::access(m, this); }
	bool access(message&m){ return TEMPLET::access(&m, this); }

	void at(int _at){ TEMPLET::at(this, _at); }
	void delay(double t){ TEMPLET::delay(this, t); }
	double time(){ return TEMPLET::time(this); }
	void stop(){ TEMPLET::stop(this); }

	void in(mes&m){m._server_id=TAG_in; m._srv=this;}

	static void stopper_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_in: ((stopper*)a)->in_handler(*((mes*)m)); break;
		}
	}

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
	my_engine e(argc, argv);
/*$TET$footer*/

	std::cout << "NUM_BLOCKS = " << NUM_BLOCKS << endl
		<< "BLOCK_SIZE = " << BLOCK_SIZE << endl
		<< "OMP_NUM_PROCS = " << omp_get_num_procs() << endl;

	for (int i = 0; i < NUM_BLOCKS*BLOCK_SIZE; i++)	block_array[i] = rand();
	
	double time = omp_get_wtime();
	std::sort(&block_array[0], &block_array[NUM_BLOCKS*BLOCK_SIZE]);                  // <-- code fragment 0
	time = omp_get_wtime() - time;

	std::cout << "\nSequential sort time is " << time << " sec\n";

	//////////////////// sequential blocksort /////////////////////
	for (int i = 0; i < NUM_BLOCKS*BLOCK_SIZE; i++)	block_array[i] = rand();
	
	time = omp_get_wtime();

	for (int i = 0; i<NUM_BLOCKS; i++) block_sort(i);                                 // <-- code fragment 1
	for (int i = 1; i<NUM_BLOCKS; i++) for (int j = 0; j<i; j++) block_merge(j, i);   //
	
	time = omp_get_wtime() - time;

	if (!is_sorted())std::cout << "\nSomething went wrong in the sequential block-sort!!!\n";
	else std::cout << "Sequential block-sort time is " << time << " sec\n";
	///////////////////////////////////////////////////////////////

	/////////////////// parallel actor blocksort //////////////////
	producer a_producer(e);
	stopper a_stoper(e);

	sorter** a_sorter = new sorter*[NUM_BLOCKS];
	for (int i = 0; i < NUM_BLOCKS; i++) { 
		a_sorter[i] = new sorter(e); 
		a_sorter[i]->i = i;
		a_producer.in(a_sorter[i]->out);
	}

	merger** a_merger = new merger*[NUM_BLOCKS-1];
	for(int i=0;i<NUM_BLOCKS-1;i++)a_merger[i]=new merger(e);
	
	mes* prev=&a_producer.out;
	for(int i=0;i<NUM_BLOCKS -1;i++){
		a_merger[i]->in(*prev);
		prev=&(a_merger[i]->out);
	}
	a_stoper.in(*prev);

	for (int i = 0; i < NUM_BLOCKS*BLOCK_SIZE; i++)	block_array[i] = rand();
	
	time = omp_get_wtime();
	e.run();
	time = omp_get_wtime() - time;

	if (!is_sorted())std::cout << "\nSomething went wrong in the parallel actor block-sort!!!\n";
	else std::cout << "\nParallel block-sort time is " << time << " sec\n";

#if defined(SIMULATED_EXECUTION)
	double T1, Tp;
	int Pmax;
	double Smax;
	int P=1;
	double Sp;

	if (TEMPLET::stat(&e, &T1, &Tp, &Pmax, &Smax, P, &Sp)) {
		std::cout << "\nSimulated sequential time = " << T1 / (1 + TRADEOFF) << " sec\n"
			<< "Simulated parallel time = " << Tp << " sec\n" 
			<< "Number of procs in the simulation = " << Pmax << endl
			<< "TRADEOFF is " << TRADEOFF*100. << " percents";
	}
#endif
	return 0;
	/////////////////////////////////////////////////////////////////
	
/*$TET$*/
}
