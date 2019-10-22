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

#pragma templet ~mes=

struct mes : message_interface{
/*$TET$mes$$data*/
	string _mes;
/*$TET$*/
};

#pragma templet *ping(p!mes)+

struct ping : actor_interface{
	ping(engine_interface&){
/*$TET$ping$ping*/
/*$TET$*/
	}

	mes p;

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
};

#pragma templet *pong(p?mes,tsk.task)

struct pong : actor_interface{
	pong(engine_interface&){
/*$TET$pong$pong*/
		_p = 0;
/*$TET$*/
	}

	void p(mes&){}
	task tsk;
	void tsk_submit(){}

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
	engine_interface e(argc, argv);
/*$TET$footer*/
	taskengine eng;
	e.set_task_engine(eng);

	ping a_ping(e);
	pong a_pong(e);

	a_pong.p(a_ping.p);

	e.run();
/*$TET$*/
}
