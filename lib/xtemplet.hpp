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
#include <cstdlib>
#include <cassert>

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
		inline void lock() { while(!omp_test_lock(&a_lock)); }
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
	public:
		message(actor*, message_adaptor){}
		message() {}
		void send(){}
		void bind(actor*, message_adaptor){}
	};

	class actor {
	public:
		actor(bool active=false){}
	protected:
		void engine(engine&);
		virtual void start() {};
	public:
		bool access(message&) const { return false; }
		bool access(message*) const { return false; }
		void stop(){}
		void suspend(){}
		void resume(){}
	};
	
	class engine {
	public:
		engine(){}
		~engine(){}
		void dispatch(){}
		bool graceful_shutdown() { return false; }
	private:
		mutex_mock _lock1;
		mutex_mock _lock2;
	};

	class base_task: task {
		friend class base_engine;
	public:
		base_task(actor*a,task_adaptor ta) :_actor(a),_tsk_adaptor(ta),_engine(0),_submitted(false) {}
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