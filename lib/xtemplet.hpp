/*--------------------------------------------------------------------------*/
/*  Copyright 2020 Sergei Vostokin                                     */
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
#include <list>
#include <cstdlib>
#include <cassert>

#define TEMPLET_OMP_SYNC

#if   defined(TEMPLET_CPP_SYNC)
#include <mutex>
#elif defined(TEMPLET_OMP_SYNC)
#include <omp.h>
#endif

namespace templet{
	
	class message;
	class actor;
	class engine;
	class task{};

	class base_engine;
	class base_task;

#if   defined(TEMPLET_CPP_SYNC)
	class mutex_mock : public std::mutex {};
#elif defined(TEMPLET_OMP_SYNC)
	class mutex_mock {
	public:
		mutex_mock() { omp_init_lock(&a_lock); }
		~mutex_mock() { omp_destroy_lock(&a_lock); }
		inline bool try_lock() { return omp_test_lock(&a_lock)!=0; }
		inline void lock() { omp_set_lock(&a_lock); }
		inline void unlock() { omp_unset_lock(&a_lock); }
	private:
		omp_lock_t a_lock;
	};
#else
	class mutex_mock {
	public:
		inline bool try_lock() { return true; }
		inline void lock(){}
		inline void unlock(){}
	};
#endif

	typedef void(*message_adaptor)(actor*, message*);
	typedef void(*task_adaptor)(actor*, task*);

	class message {
		friend class engine;
		friend class actor;
	public:
		message(actor*a, message_adaptor ma) :_forward(true), 
			_cli_actor(a), _cli_adaptor(ma),
			_srv_actor(0), _srv_adaptor(0) {}
		message() :_forward(true), 
			_cli_actor(0), _cli_adaptor(0),
			_srv_actor(0), _srv_adaptor(0) {}
		void send();
		void bind(actor*a, message_adaptor ma) {
			_forward = true; _srv_actor = a; _srv_adaptor = ma;
		}
	private:
		bool  _forward;
		bool  _idle;
		actor* _cli_actor;
		actor* _srv_actor;
		message_adaptor _cli_adaptor;
		message_adaptor _srv_adaptor;
	};

	class actor {
	public:
		actor(bool active=false) :_suspended(false), _active(active), _engine(0) {}
	protected:
		void engine(templet::engine&e);
		void engine(templet::engine*e);
		virtual void start() {};
	public:
		bool access(message&m) const { return m._idle && (m._forward ? m._cli_actor==this : m._srv_actor == this); }
		bool access(message*m) const { return m->_idle && (m->_forward ? m->_cli_actor == this : m->_srv_actor == this); }
		void stop();
		void suspend() { _suspended = true; }
		void resume();
	private:
		bool _suspended;
		bool _active;
		std::list<message*> _wait_queue;
		templet::engine*    _engine;
	};
	
	class engine {
		friend class message;
		friend class actor;
	public:
		engine() : _stopped(true) {}
		~engine(){}

		void dispatch(){
			_stopped = false;
			///////////////////////////////////////
		}

		bool graceful_shutdown() { return (_stopped && _ready.empty()); }

	private:
		mutex_mock _lock1;
		mutex_mock _lock2;
		bool _stopped;
		std::list<message*> _ready;
		std::list<actor*>  _active;

	private:
		inline static void send(message*m, actor*a) {
			////////////////////////////////////////
		}

		inline static void resume(actor*) {
			////////////////////////////////////////
		}
	};

	void message::send() {
		if (_forward) engine::send(this, _srv_actor);
		else engine::send(this, _cli_actor);
	}

	void actor::engine(templet::engine&e) { _engine = &e; _engine->_active.push_back(this); }
	void actor::engine(templet::engine*e) { _engine = e; _engine->_active.push_back(this);	}
	void actor::stop() { _engine->_stopped = true;	}
	void actor::resume() { _suspended = false; engine::resume(this); } /////////////////////////////////// ???

	class base_task: task {
		friend class base_engine;
	public:
		base_task(actor*a,task_adaptor ta) :_actor(a), _tsk_adaptor(ta), _engine(0), _submitted(false) {}
		void engine(base_engine&e) { assert(_engine==0); _engine = &e; }
		void submit();
	protected:
		virtual void run(){}
	private:
		bool         _submitted;
		actor*       _actor;
		base_engine* _engine;
		task_adaptor _tsk_adaptor;
	};

	class base_engine {
		friend class base_task;
	public:
		void run(){
			_task_queue.clear();
			size_t rsize;
			while ((rsize = _task_queue.size())) {
				int n = rand() % rsize;	std::vector<base_task*>::iterator it = _task_queue.begin() + n;
				base_task* tsk = *it; _task_queue.erase(it);
				tsk->_submitted = false;
				tsk->run();
				(*tsk->_tsk_adaptor)(tsk->_actor, tsk);
				tsk->_actor->resume();
			}
		}
	private:
		std::vector<base_task*> _task_queue;
	};

	void base_task::submit() { assert(_engine && !_submitted); _engine->_task_queue.push_back(this); _submitted = true; }
}