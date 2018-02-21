/*$TET$actor*/
/*--------------------------------------------------------------------------*/
/*  Copyright 2018 Sergei Vostokin                                          */
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
#include "templet.hpp"

const int N_PRIME = 10;
const int N_FILTER = N_PRIME - 1;

/*$TET$*/

using namespace TEMPLET;

#pragma templet ~mes=

struct mes : message_interface{
/*$TET$mes$$data*/
	int number;
/*$TET$*/
};

#pragma templet *seeder(p!mes)+

struct seeder : actor_interface{
	seeder(engine_interface&){
/*$TET$seeder$seeder*/
		counter = 2;
/*$TET$*/
	}

	mes p;

	void start(){
/*$TET$seeder$start*/
		p.number = counter++;
		p.send();
/*$TET$*/
	}

	void p_handler(mes&m){
/*$TET$seeder$p*/
		p.number = counter++;
		p.send();
/*$TET$*/
	}

/*$TET$seeder$$code&data*/
	int counter;
/*$TET$*/
};

#pragma templet *filter(in?mes,out!mes)

struct filter : actor_interface{
	filter(engine_interface&){
/*$TET$filter$filter*/
		prime_number = 0;
/*$TET$*/
	}

	void in(mes&){}
	mes out;

	void in_handler(mes&m){
/*$TET$filter$in*/
		_in = &m;
		do_filtering();
/*$TET$*/
	}

	void out_handler(mes&m){
/*$TET$filter$out*/
		do_filtering();
/*$TET$*/
	}

/*$TET$filter$$code&data*/
	int prime_number;
	mes*_in;

	void do_filtering() {
		if (access(_in) && access(out)) {
			if (!prime_number) {
				prime_number = _in->number;
				_in->send();
			}
			else {
				if (_in->number % prime_number == 0)
					_in->send();
				else {
					out.number = _in->number;
					_in->send();
					out.send();
				}
			}
		}
	}
/*$TET$*/
};

#pragma templet *stopper(p?mes)

struct stopper : actor_interface{
	stopper(engine_interface&){
/*$TET$stopper$stopper*/
/*$TET$*/
	}

	void p(mes&){}

	void p_handler(mes&m){
/*$TET$stopper$p*/
		prime_number = m.number;
		stop();
/*$TET$*/
	}

/*$TET$stopper$$code&data*/
	int prime_number;
/*$TET$*/
};

int main(int argc, char *argv[])
{
	engine_interface e(argc, argv);
/*$TET$footer*/
	seeder  a_seeder(e);
	stopper a_stopper(e);

	filter*  f[N_FILTER];
	for (int i = 0; i<N_FILTER; i++) f[i] = new filter(e);

	for (int i = 0; i<N_FILTER - 1; i++)f[i + 1]->in(f[i]->out);

	f[0]->in(a_seeder.p);
	a_stopper.p(f[N_FILTER - 1]->out);

	e.run();

	for (int i = 0; i<N_FILTER; i++)
		std::cout << f[i]->prime_number << "  ";
	std::cout << a_stopper.prime_number;

/*$TET$*/
}
