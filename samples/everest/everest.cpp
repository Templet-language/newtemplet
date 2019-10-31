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
//#define USE_TASK_EMUL
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
		tsk.set_app_id("5cd2ef531000006cfef4ff7a");//init app
/*$TET$*/
	}

	void p(mes&){}
	task tsk;
	void tsk_submit(){}

	void p_handler(mes&m){
/*$TET$pong$p*/
		_p = &m;

#ifdef USE_TASK_EMUL
		tsk.set_on_start([&]() {
			cout << m._mes.c_str() << endl;
			m._mes = "Hello PING!!!";
		});
#else
		json in;
		tsk.input(in);
#endif

		tsk_submit();
/*$TET$*/
	}

	void tsk_handler(task&m){
/*$TET$pong$tsk*/
#ifndef USE_TASK_EMUL
		cout << m.result().dump();
#endif
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
	
#ifdef USE_TASK_EMUL
	taskengine eng;
#else
	taskengine eng("vostokinsv", "SergeyVostokin");
#endif
	e.set_task_engine(eng);

////////////////////////////////////////
	task sort1(eng, "5ccaba85100000638af4eabe");
	task sort2(eng, "5ccaba85100000638af4eabe");
	task merge(eng, "5d22422012000050bcf95406");

	json in_sort1, in_sort2, in_merge;

	in_sort1["name"] = "sort1";
	in_sort1["inputs"]["i"]  = 0;
	in_sort1["inputs"]["vi"] = 0;
	
	in_sort2["name"] = "sort2";
	in_sort2["inputs"]["i"] = 1;
	in_sort2["inputs"]["vi"] = 0;
	
	in_merge["name"] = "merge";
	in_merge["inputs"]["i"] = 0;
	in_merge["inputs"]["vi"] = 1;
	in_merge["inputs"]["j"] = 0;
	in_merge["inputs"]["vj"] = 1;

	sort1.submit(in_sort1);
	sort2.submit(in_sort2);

	eng.wait_for(sort1);
	eng.wait_for(sort2);

	cout << sort1.result().dump() << endl;
	cout << sort2.result().dump() << endl;

	merge.submit(in_merge);

	eng.wait_all();

	cout << merge.result().dump() << endl;
////////////////////////////////////////

	ping a_ping(e);
	pong a_pong(e);

	a_pong.p(a_ping.p);

	e.run();
/*$TET$*/
}
