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

class my_engine{
public:
	my_engine(int argc, char *argv[]);
	void run();
	void map();
};

#pragma templet ~value_message$=

struct value_message : message{
	bool access(actor*);
	void send();

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

struct master : actor{
	master(my_engine&){
/*$TET$master$master*/
/*$TET$*/
	}

	void delay(double);
	double time();
	void at(int);
	void stop();

	value_message* sin2_port();
	value_message* cos2_port();

	void start(){
/*$TET$master$start*/
		_sin2_port.x = _cos2_port.x = x;
		_cos2_port.task = value_message::COS2;
		_sin2_port.task = value_message::SIN2;
		_cos2_port.send(); _sin2_port.send();
/*$TET$*/
	}

	void sin2_port(value_message&m){
/*$TET$master$sin2_port*/
		if (_cos2_port.access(this)){
			x = _cos2_port.x + _sin2_port.x; delay(1.0);
			stop();
		}
/*$TET$*/
	}

	void cos2_port(value_message&m){
/*$TET$master$cos2_port*/
		if (_sin2_port.access(this)){
			x = _cos2_port.x + _sin2_port.x; delay(1.0);
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

	value_message _sin2_port;
	value_message _cos2_port;
};

#pragma templet *worker(master_port?value_message)

struct worker : actor{
	worker(my_engine&){
/*$TET$worker$worker*/
/*$TET$*/
	}

	void delay(double);
	double time();
	void at(int);
	void stop();

	void master_port(value_message*);

	void master_port(value_message&m){
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

/*$TET$footer*/
int main(int argc, char *argv[])
{
	my_engine _my_engine(argc, argv);

	master _master(_my_engine);
	worker _sin2_worker(_my_engine);
	worker _cos2_worker(_my_engine);

	_sin2_worker.master_port(_master.sin2_port());
	_cos2_worker.master_port(_master.cos2_port());

	_master.at(0);
	_sin2_worker.at(1);
	_cos2_worker.at(2);

	_my_engine.map();

	double x = 1234;

	_master.x = x;

	_my_engine.run();

	std::cout << "sin^2(" << x << ")+cos^2(" << x << ")=" << _master.x << '\n';

	double T1, Tp, Smax, Sp;
	int Pmax, P = 5;

	if (TEMPLET::stat(&_my_engine, &T1, &Tp, &Pmax, &Smax, P, &Sp)){
		std::cout << "T1 = " << T1 << ", Tp = " << Tp << ", Pmax = " << Pmax << ", Smax = " << Smax << ", P = " << P << ", Sp = " << Sp;
	}

	return 0;
}
/*$TET$*/
