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

#define EVEREST_EXECUTION
#define USE_TASK_EMUL
#include <templet.hpp>

using namespace std;
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
	string _mes;
/*$TET$*/

	enum { CLI, SRV } _where;
	actor* _srv;
	actor* _cli;
	int _client_id;
	int _server_id;
};

#pragma templet *ping(p!mes)+

struct ping : actor{
	enum tag{START,TAG_p};

	ping(my_engine&e):p(this, &e, TAG_p){
		::init(this, &e, ping_recv_adapter);
		::init(&_start, this, &e);
		::send(&_start, this, START);
/*$TET$ping$ping*/
/*$TET$*/
	}

	bool access(message*m){ return TEMPLET::access(m, this); }
	bool access(message&m){ return TEMPLET::access(&m, this); }

	void at(int _at){ TEMPLET::at(this, _at); }
	void delay(double t){ TEMPLET::delay(this, t); }
	double time(){ return TEMPLET::time(this); }
	void stop(){ TEMPLET::stop(this); }

	mes p;

	static void ping_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_p: ((ping*)a)->p_handler(*((mes*)m)); break;
			case START: ((ping*)a)->start(); break;
		}
	}

	void start(){
/*$TET$ping$start*/
		p._mes = "Hello PONG!!!";
		p.send();
/*$TET$*/
	}

	void p_handler(mes&m){
/*$TET$ping$p*/
		cout << m._mes.c_str();
		stop();
/*$TET$*/
	}

/*$TET$ping$$code&data*/
/*$TET$*/
	message _start;
};

#pragma templet *pong(p?mes,tsk.task)

struct pong : actor{
	enum tag{START,TAG_p,TAG_tsk};

	pong(my_engine&e):tsk(*(e._teng)){
		::init(this, &e, pong_recv_adapter);
		tsk.set_on_ready([&]() { tsk_handler(tsk); resume(); });
/*$TET$pong$pong*/
		_p = 0;
/*$TET$*/
	}

	bool access(message*m){ return TEMPLET::access(m, this); }
	bool access(message&m){ return TEMPLET::access(&m, this); }

	void at(int _at){ TEMPLET::at(this, _at); }
	void delay(double t){ TEMPLET::delay(this, t); }
	double time(){ return TEMPLET::time(this); }
	void stop(){ TEMPLET::stop(this); }

	void p(mes&m){m._server_id=TAG_p; m._srv=this;}
	task tsk;
	void tsk_submit() { tsk.submit(); suspend(); };

	static void pong_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_p: ((pong*)a)->p_handler(*((mes*)m)); break;
		}
	}

	void p_handler(mes&m){
/*$TET$pong$p*/
		_p = &m;

		tsk.set_on_start([&]() {
			cout << m._mes.c_str() << endl;
			m._mes = "Hello PING!!!";
		});

		tsk_submit();
/*$TET$*/
	}

	void tsk_handler(task&m){
/*$TET$pong$tsk*/
		_p->send();
/*$TET$*/
	}

/*$TET$pong$$code&data*/
	mes* _p;
/*$TET$*/
};

int main(int argc, char *argv[])
{
	my_engine e(argc, argv);
/*$TET$footer*/
	taskengine eng;
	e.set_task_engine(eng);

	ping a_ping(e);
	pong a_pong(e);

	a_pong.p(a_ping.p);

	e.run();
/*$TET$*/
}
