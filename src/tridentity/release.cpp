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

using namespace TEMPLET;

class my_engine : public engine{
public:
	my_engine(int argc, char *argv[]){
		::init(this,argc,argv);
	}
	void run(){ ::run(this); }
	void map(){ ::map(this); }
};

class value_message :  public message{
public:
	value_message(actor*a, engine*e,int t) : where(CLI), _cli(a), client_id(t){
		::init(this, e, save_adapter, restore_adapter);
	}

	bool access(){
		bool ret;
		if (where == CLI) ret=::access(this, _cli);
		else if (where == SRV) ret=::access(this, _srv);
		return ret;
	}

	void send(){
		if (where == CLI){ ::send(this, _srv, server_id); where = SRV; }
		else if (where == SRV){ ::send(this, _cli, client_id); where = CLI; }
	}

	void save(saver*s){
		::save(s, &x, sizeof(x));
	}
	void restore(restorer*r){
		::restore(r, &x, sizeof(x)); 
	}

	double x;
	enum {COS2,SIN2} task;

	enum { CLI, SRV } where;
	actor* _srv;
	actor* _cli;
	int client_id;
	int server_id;

	friend void save_adapter(message*m, saver*s){
		((value_message*)m)->save(s);
	}
	friend void restore_adapter(message*m, restorer*r){
		((value_message*)m)->restore(r);
	}
};

class master : public actor{
public:
	enum{ PORT_sin2=1, PORT_cos2=2, START=3 };

	master(my_engine&e) : _cos2(this,&e,PORT_cos2), _sin2(this,&e,PORT_sin2){
		::init(this, &e, recv_master, save_adapter, restore_adapter);
				
		::init(&_start, &e);
		::send(&_start, this, START);
	}

	value_message& sin2_port(){  return _sin2; }
	value_message& cos2_port(){  return _cos2; }

	void at(int _at){ ::at(this,_at); }
	void delay(double t){ ::delay(this, t); }
	void stop(){ ::stop(this); }

	void start(){
		_cos2.x = _sin2.x = x;
		_cos2.task = value_message::COS2;
		_sin2.task = value_message::SIN2;
		_cos2.send(); _sin2.send();
	}

	void sin2_func(value_message&m){
		if (_cos2.access()){
			x = _cos2.x + _sin2.x; delay(1.0);
			stop();
		}
	}

	void cos2_func(value_message&m){
		if (_sin2.access()){
			x = _cos2.x + _sin2.x; delay(1.0);
			stop();
		}
	}

	void save(saver*s){
		::save(s, &x, sizeof(x));
	}

	void restore(restorer*r){
		::restore(r, &x, sizeof(x)); 
	}

	value_message _cos2;
	value_message _sin2;

	double x;

	message _start;

	friend void recv_master (actor*a, message*m,int tag){
		master* th = (master*)a;
		if (tag == PORT_cos2) th->cos2_func(*((value_message*)m));
		else if (tag == PORT_sin2) th->sin2_func(*((value_message*)m));
		else if (tag == START) th->start();
	}

	friend void save_adapter(actor*a, saver*s){
		((value_message*)a)->save(s);
	}

	friend void restore_adapter(actor*a, restorer*r){
		((value_message*)a)->restore(r);
	}
	
};

class worker: public actor{
public:
	enum{PORT_master=1};

	worker(my_engine&e){
		::init(this, &e, recv_worker);
	}

	void master_port(value_message&m){
		m._srv = this; m.server_id = PORT_master;
	}
	
	void master_func(value_message&m){
		if (m.task == value_message::COS2){
			m.x = cos(m.x)*cos(m.x); delay(1.0);
			m.send();
		}
		else if (m.task == value_message::SIN2){
			m.x = sin(m.x)*sin(m.x); delay(1.0);
			m.send();
		}
	}

	void at(int _at){ ::at(this, _at); }
	void delay(double t){ ::delay(this, t); }

	friend void recv_worker(actor*a, message*m,int tag){
		worker* th = (worker*)a;
		if (tag == PORT_master) th->master_func(*((value_message*)m));
	}
};

int main(int argc, char *argv[])
{
	my_engine _my_engine(argc,argv);

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
