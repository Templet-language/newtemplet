#pragma once

#include <cassert>
#include <functional>

namespace TEMPLET {

	class task;

	struct event{
		task* _task;
	};

	class taskengine {
	public:
		taskengine() {
		}

		void init();

		void wait_all() {
			assert(++_recount == 1);
			//_wait_loop_body(ev);
			assert(--_recount == 0);
		}

		void wait_for(task& t) {
			assert(++_recount == 1);
			//_wait_loop_body(ev);
			assert(--_recount == 0);
		}

		task* wait_some() {
			assert(++_recount == 1);
			//_wait_loop_body(ev);
			assert(--_recount == 0);
			return 0;
		}

		void submit(task&t);
		bool is_idle(task&t);

	private:
		inline void _wait_loop_body(event&ev);

	private:
		int    _recount;
		bool   _task_mode;
		task*  _cur_task;
	};

	class task {
		friend	class taskengine;
	public:
		task() :_eng(0), _is_idle(true), _on_start([]() {}), _on_ready([]() {}) {} // only for compatibility with preprocessor's design mode
		task(taskengine&e) :_eng(&e), _is_idle(true), _on_start([]() {}), _on_ready([]() {}) {}

		void set_on_start(std::function<void(void)> callee) { _on_start = callee; }
		void set_on_ready(std::function<void(void)> callee) { _on_ready = callee; }

		void submit() { _eng->submit(*this); }

		void submit(std::function<void(void)> on_start) {
			_on_start = on_start; _eng->submit(*this);
		}

		void submit(std::function<void(void)> on_start, std::function<void(void)> on_ready) {
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
	}

	void taskengine::submit(task&t) {
		assert(t._eng == this && is_idle(t) && !_task_mode);
	}

	bool taskengine::is_idle(task&t) {
		assert(!_task_mode);
		return t._is_idle;
	}

	void taskengine::_wait_loop_body(event&ev) {
	}
}
