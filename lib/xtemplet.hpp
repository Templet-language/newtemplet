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
		message(actor*a=0, message_adaptor ma=0) :_from_cli_to_srv(true),
			_cli_actor(a),  _cli_adaptor(ma),
			_srv_actor(0),  _srv_adaptor(0),
			_active(false), _actor(a) {}
		void actor(templet::actor*a) { assert(a == 0); _actor = a; }
		void send();
		void bind(templet::actor*a, message_adaptor ma) {
			_from_cli_to_srv = true; _srv_actor = a; _srv_adaptor = ma;
		}
	private:
		bool _active;
		templet::actor* _actor;

		bool _from_cli_to_srv;
		templet::actor* _cli_actor;
		templet::actor* _srv_actor;
		message_adaptor _cli_adaptor;
		message_adaptor _srv_adaptor;
	};

	class actor {
		friend class engine;
	public:
		actor(bool start=false) :_suspended(false), _do_start(start), _engine(0) {}
	protected:
		void engine(templet::engine&e);
		void engine(templet::engine*e);
		virtual void start() {};
	public:
		bool access(message&m) const { return !m._active  && m._actor == this; }
		bool access(message*m) const { return m && !m->_active && m->_actor == this; }
		void stop();
		void suspend() { _suspended = true; }
		void resume();
	private:
		bool _suspended;
		bool _do_start;
		std::list<message*> _inbound;
		std::list<message*> _outbound;
		templet::engine*    _engine;
	};
	
	class engine {
		friend class message;
		friend class actor;
	public:
		engine() : _stopped(false), _started(false) {}
		~engine(){}

		void dispatch(){
			if (!_started) {
				for (std::list<actor*>::iterator it = _start_list.begin(); it != _start_list.end(); it++)
					(*it)->start();
				_started = true;
			}
			///////////////////////////////////////
		}

		bool graceful_shutdown() { return _stopped; }

	private:
		mutex_mock _lock1;
		mutex_mock _lock2;
		bool _stopped;
		bool _started;
		std::list<message*> _on_delivery;
		std::list<actor*>   _start_list;

	private:
		inline static void send(message*m) {
			////////////////////////////////////////
		}

		inline static void resume(actor*a) {
/*			lock(lock_1);

			put_local_messages_to_queue();

		try_to_be_the_master:
			if (try_lock(lock_2)) {

				unlock(lock_1);

				while (message_in_queue())process_message();

				unlock(lock_2);
			}
			else {
				unlock(lock_1);
				return;
			}

			lock(lock_1);

			if (!message_in_queue()) {
				unlock(lock_1);
				return;
			}
			else goto try_to_be_the_master;
*/
		}
	};

	void message::send() {	engine::send(this); }
	void actor::engine(templet::engine&e) { _engine = &e; if(this->_do_start)_engine->_start_list.push_back(this); }
	void actor::engine(templet::engine*e) { _engine = e; if(this->_do_start)_engine->_start_list.push_back(this);	}
	void actor::stop() { _engine->_stopped = true;	}
	void actor::resume() { engine::resume(this); }

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