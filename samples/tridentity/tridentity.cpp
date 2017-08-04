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
/*$TET$*/

using namespace TEMPLET;

#pragma templet ~value_message$=

struct value_message : message_interface{
	void save(saver*s){
/*$TET$value_message$$save*/
		::save(s, &x, sizeof(x));
/*$TET$*/
	}

	void restore(restorer*r){
/*$TET$value_message$$restore*/
		::restore(r, &x, sizeof(x)); 
/*$TET$*/
	}
/*$TET$value_message$$data*/
	double x;
	enum {COS2,SIN2} task;
/*$TET$*/
};

#pragma templet *master$(sin2_port!value_message,cos2_port!value_message)+

struct master : actor_interface{
	master(engine_interface&){
/*$TET$master$master*/
/*$TET$*/
	}

	value_message sin2_port;
	value_message cos2_port;

	void start(){
/*$TET$master$start*/
		sin2_port.x = cos2_port.x = x;
		cos2_port.task = value_message::COS2;
		sin2_port.task = value_message::SIN2;
		cos2_port.send(); sin2_port.send();
/*$TET$*/
	}

	void sin2_port_handler(value_message&m){
/*$TET$master$sin2_port*/
		if (access(cos2_port)){
			x = cos2_port.x + sin2_port.x; delay(1.0);
			stop();
		}
/*$TET$*/
	}

	void cos2_port_handler(value_message&m){
/*$TET$master$cos2_port*/
		if (access(sin2_port)){
			x = cos2_port.x + sin2_port.x; delay(1.0);
			stop();
		}
/*$TET$*/
	}

	void save(saver*s){
/*$TET$master$$save*/
		::save(s, &x, sizeof(x));
/*$TET$*/
	}

	void restore(restorer*r){
/*$TET$master$$restore*/
		::restore(r, &x, sizeof(x)); 
/*$TET$*/
	}

/*$TET$master$$code&data*/
	double x;
/*$TET$*/
};

#pragma templet *worker(master_port?value_message)

struct worker : actor_interface{
	worker(engine_interface&){
/*$TET$worker$worker*/
/*$TET$*/
	}

	void master_port(value_message&){}

	void master_port_handler(value_message&m){
/*$TET$worker$master_port*/
		if (m.task == value_message::COS2){
			m.x = cos(m.x)*cos(m.x); delay(1.0);
			m.send();
		}
		else if (m.task == value_message::SIN2){
			m.x = sin(m.x)*sin(m.x); delay(1.0);
			m.send();
		}
/*$TET$*/
	}

/*$TET$worker$$code&data*/
/*$TET$*/
};

int main(int argc, char *argv[])
{
	engine_interface e(argc, argv);
/*$TET$footer*/
	master _master(e);
	worker _sin2_worker(e);
	worker _cos2_worker(e);

	_sin2_worker.master_port(_master.sin2_port);
	_cos2_worker.master_port(_master.cos2_port);

	_master.at(0);
	_sin2_worker.at(1);
	_cos2_worker.at(2);

	e.map();

	double x = 1234;

	_master.x = x;

	e.run();

	std::cout << "sin^2(" << x << ")+cos^2(" << x << ")=" << _master.x << '\n';

	double T1, Tp, Smax, Sp;
	int Pmax, P = 5;

	if (TEMPLET::stat(&e, &T1, &Tp, &Pmax, &Smax, P, &Sp)){
		std::cout << "T1 = " << T1 << ", Tp = " << Tp << ", Pmax = " << Pmax << ", Smax = " << Smax << ", P = " << P << ", Sp = " << Sp;
	}

	return 0;
/*$TET$*/
}
