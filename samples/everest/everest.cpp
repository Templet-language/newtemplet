/*$TET$actor*/
/*--------------------------------------------------------------------------*/
/*  Copyright 2019 Sergei Vostokin                                          */
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

#define EVEREST_EXECUTION
#include <templet.hpp>
#include <iostream>

using namespace TEMPLET;
using namespace std;

struct tasksort :task {
	tasksort(taskengine&e):task(e, "5ccaba85100000638af4eabe"){}
	tasksort(){}
	void sort(int i, int vi){
		json in_sort;
		in_sort["name"] = "sort-task";
		in_sort["inputs"]["i"] = i;
		in_sort["inputs"]["vi"] = vi;
		input(in_sort);
	}
	void sort_time(ostream&s) { s << result().dump(); }
};

struct taskmerge :task {
	taskmerge(taskengine&e):task(e, "5d22422012000050bcf95406"){}
	taskmerge(){}
	void merge(int i, int vi, int j, int vj) {
		json in_merge;
		in_merge["name"] = "merge-task";
		in_merge["inputs"]["i"] = i;
		in_merge["inputs"]["vi"] = vi;
		in_merge["inputs"]["j"] = j;
		in_merge["inputs"]["vj"] = vj;
		input(in_merge);
	}
	void merge_time(ostream&s) { s << result().dump(); }
};

/*$TET$*/

using namespace TEMPLET;

struct my_engine : engine{
	my_engine(int argc, char *argv[]){
		::init(this, argc, argv);
	}
	bool run(){ return TEMPLET::run(this); }
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
/*$TET$*/

	enum { CLI, SRV } _where;
	actor* _srv;
	actor* _cli;
	int _client_id;
	int _server_id;
};

#pragma templet *sorter(p!mes,t.tasksort)+

struct sorter : actor{
	enum tag{START,TAG_p,TAG_t};

	sorter(my_engine&e):p(this, &e, TAG_p),t(*(e._teng)){
		TEMPLET::init(this, &e, sorter_recv_adapter);
		TEMPLET::init(&_start, this, &e);
		TEMPLET::send(&_start, this, START);
		t.set_on_ready([&]() { t_handler(t); resume(); });
/*$TET$sorter$sorter*/
/*$TET$*/
	}

	bool access(message*m){ return TEMPLET::access(m, this); }
	bool access(message&m){ return TEMPLET::access(&m, this); }

	void at(int _at){ TEMPLET::at(this, _at); }
	void delay(double t){ TEMPLET::delay(this, t); }
	double time(){ return TEMPLET::time(this); }
	void stop(){ TEMPLET::stop(this); }

	mes p;
	tasksort t;
	void t_submit() { t.submit(); suspend(); };

	static void sorter_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_p: ((sorter*)a)->p_handler(*((mes*)m)); break;
			case START: ((sorter*)a)->start(); break;
		}
	}

	void start(){
/*$TET$sorter$start*/
		t_submit();
/*$TET$*/
	}

	void p_handler(mes&m){
/*$TET$sorter$p*/
/*$TET$*/
	}

	void t_handler(tasksort&t){
/*$TET$sorter$t*/
		p.send();
/*$TET$*/
	}

/*$TET$sorter$$code&data*/
/*$TET$*/
	message _start;
};

#pragma templet *merger(p1?mes,p2?mes,t.taskmerge)

struct merger : actor{
	enum tag{START,TAG_p1,TAG_p2,TAG_t};

	merger(my_engine&e):t(*(e._teng)){
		TEMPLET::init(this, &e, merger_recv_adapter);
		t.set_on_ready([&]() { t_handler(t); resume(); });
/*$TET$merger$merger*/
/*$TET$*/
	}

	bool access(message*m){ return TEMPLET::access(m, this); }
	bool access(message&m){ return TEMPLET::access(&m, this); }

	void at(int _at){ TEMPLET::at(this, _at); }
	void delay(double t){ TEMPLET::delay(this, t); }
	double time(){ return TEMPLET::time(this); }
	void stop(){ TEMPLET::stop(this); }

	void p1(mes&m){m._server_id=TAG_p1; m._srv=this;}
	void p2(mes&m){m._server_id=TAG_p2; m._srv=this;}
	taskmerge t;
	void t_submit() { t.submit(); suspend(); };

	static void merger_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_p1: ((merger*)a)->p1_handler(*((mes*)m)); break;
			case TAG_p2: ((merger*)a)->p2_handler(*((mes*)m)); break;
		}
	}

	void p1_handler(mes&m){
/*$TET$merger$p1*/
		_p1 = &m;
		on_sort_end();
/*$TET$*/
	}

	void p2_handler(mes&m){
/*$TET$merger$p2*/
		_p2 = &m;
		on_sort_end();
/*$TET$*/
	}

	void t_handler(taskmerge&t){
/*$TET$merger$t*/
		stop();
/*$TET$*/
	}

/*$TET$merger$$code&data*/
	void on_sort_end() {
		if (access(_p1) && access(_p2)) {
			t_submit();
		}
	}
	mes* _p1 = 0;
	mes* _p2 = 0;
/*$TET$*/
};

/*$TET$code&data*/
/////////////////////////////////////////////////////
/*$TET$*/

int main(int argc, char *argv[])
{
	my_engine e(argc, argv);
/*$TET$footer*/
	taskengine eng("", "");
	e.set_task_engine(eng);

	sorter s1(e);
	sorter s2(e);
	merger m(e);

	s1.t.sort(0, 0);
	s2.t.sort(1, 0);
	m.t.merge(0, 1, 1, 1);

	m.p1(s1.p);
	m.p2(s2.p);

	e.run();

	cout << endl << " sorter #1 " << endl;
	s1.t.sort_time(cout);
	cout << endl << " sorter #2 " << endl;
	s2.t.sort_time(cout);
	cout << endl << " merger " << endl;
	m.t.merge_time(cout);

	/*
	tasksort sort1(eng), sort2(eng);
	taskmerge merge(eng);

	sort1.sort(0, 0);
	sort2.sort(1, 0);
	merge.merge(0, 1, 1, 1);

	sort1.submit();
	sort2.submit();

	eng.wait_all();

	merge.submit();

	eng.wait_all();

	cout << endl << " sorter #1 " << endl;
	sort1.sort_time(cout);
	cout << endl << " sorter #2 " << endl;
	sort2.sort_time(cout);
	cout << endl << " merger " << endl;
	merge.merge_time(cout);
	*/

/*$TET$*/
}
