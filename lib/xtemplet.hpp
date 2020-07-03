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

#if defined(TEMPLET_CPP_SYNC)
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
		inline bool try_lock(){}
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
		actor(engine&,bool active=false){}
		bool access(message&) const { return false; }
		bool access(message*) const { return false; }
		virtual void start() {};
		void suspend(){}
		void resume(){}
	};
	
	class engine {
	public:
		engine(){}
		~engine(){}
		bool dispatch(){}
	private:
		mutex_mock lock1;
		mutex_mock lock2;
	};

	class base_task: task {
	public:
		base_task(base_engine&, actor*, task_adaptor){}
		virtual void run(){}
		void submit(){}
	};

	class base_engine {
	public:
		base_engine(){}
		~base_engine(){}
		void run(){}
	};
}