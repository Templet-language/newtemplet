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
#pragma once

#include <vector>
#include <queue>
#include <functional>
#include <cassert>

namespace TEMPLET {

	class task;

	struct event{
		enum { TASK_BEGIN, TASK_END } _type;
		double _time;
		task* _task;
	};

	class cmp { public: bool operator()(const event&t1, const event&t2) { return t1._time > t2._time; } };

	class taskengine {
	public:
		taskengine() {
			_recount = 0; _task_mode = false; init();
		}

		void wait_all() {
			assert(++_recount == 1);
			while (!_calendar.empty()) {
				event ev = _calendar.top();	_calendar.pop();
				_wait_loop_body(ev);
			}
			assert(--_recount== 0);
		}

		void wait_for(task& t) {
			assert(++_recount == 1);
			while (!is_idle(t)) {
				event ev = _calendar.top();	_calendar.pop();
				_wait_loop_body(ev);
			}
			assert(--_recount == 0);
		}

		task* wait_some(){
			assert(++_recount == 1);
			event ev; ev._task = 0;
			while (!_calendar.empty()) {
				ev = _calendar.top();	_calendar.pop();
				_wait_loop_body(ev);
				if (ev._type == event::TASK_END) break;
			}
			assert(--_recount == 0);
			return ev._task;
		}
		
		void delay(double t) { assert(_task_mode); _cur_delay += t;	_T1 += t; }

		void submit(task&t);
		bool is_idle(task&t);

		double T1() {  return _T1; }
		double Tp() {  return _Tp; }
		int Pmax()  {  return _Pmax; }

	private:
		inline void _wait_loop_body(event&ev);
		inline void init();

	private:
		std::priority_queue<event, std::vector<event>, cmp> _calendar;
		double _Tp;
		double _T1;
		int _Pmax;

		int    _recount;
		bool   _task_mode;
		task*  _cur_task;
		double _cur_delay;
		int    _cur_P;
	};

	class task {
		friend	class taskengine;
	public:
		task() {} // only for compatibility with preprocessor's design mode
		task(taskengine&e) :_eng(&e), _is_idle(true), _on_start([](){}), _on_ready([](){}) {}

		void set_on_start(std::function<void(void)> callee) { _on_start = callee; }
		void set_on_ready(std::function<void(void)> callee) { _on_ready = callee; }
		
		void delay(double t) { _eng->delay(t); }
		
		void submit() {	_eng->submit(*this);	}

		void submit(std::function<void(void)> on_start) {
			_on_start = on_start; _eng->submit(*this);
		}

		void submit(std::function<void(void)> on_start, std::function<void(void)> on_ready ) {
			_on_start = on_start; _on_ready = on_ready; _eng->submit(*this);
		}

		bool is_idle() { return _eng->is_idle(*this); }

	private:
		std::function<void(void)> _on_start;
		std::function<void(void)> _on_ready;
		taskengine* _eng;
		bool _is_idle;
	};

	void taskengine::init() {
		assert(_recount == 0);
		_Tp = 0; _T1 = 0; _Pmax = 0; _cur_P = 0;
		while (!_calendar.empty()) { 
			_calendar.top()._task->_is_idle = true;
			_calendar.pop(); 
		}
	}

	void taskengine::submit(task&t) {
		assert(t._eng == this && is_idle(t) && !_task_mode);
	
		event ev;
		ev._time = _Tp;
		ev._type = event::TASK_BEGIN;
		ev._task = &t; t._is_idle = false;
		_calendar.push(ev);
	}

	bool taskengine::is_idle(task&t) {
		assert(!_task_mode);
		return t._is_idle;
	}

	void taskengine::_wait_loop_body(event&ev) {
		task*t = ev._task;  _Tp = ev._time;

		if (ev._type == event::TASK_BEGIN) {
			
			if (++_cur_P > _Pmax) _Pmax = _cur_P;

			_task_mode = true;
			_cur_delay = 0.0;
			t->_on_start();
			_task_mode = false;

			event new_ev;
			new_ev._time = _Tp + _cur_delay;
			new_ev._type = event::TASK_END;
			new_ev._task = t; t->_is_idle = false;
			
			_calendar.push(new_ev);
		}
		else if (ev._type == event::TASK_END) {

			_cur_P--;

			t->_is_idle = true;
			t->_on_ready();
		}
	}
}
