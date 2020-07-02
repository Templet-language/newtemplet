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

namespace templet{
	
	class message;
	class actor;
	class engine;
	class base_engine;
	class base_task;

	class mutex_stub {
	public:
		bool try_lock(){}
		void lock(){}
		void unlock(){}
	};

	typedef void(*message_adaptor)(actor*, message*);
	typedef void(*task_adaptor)(actor*, base_task*);

	class message {
	public:
		message(actor*, message_adaptor){}
		message() {}
		void send(){}
		void bind(actor*, message_adaptor){}
	};

	class actor {
	public:
		actor(engine&){}
		bool access(message&) const { return false; }
		bool access(message*) const { return false; }
		void suspend(){}
		void resume(){}
	};
	
	class engine {
	public:
		engine(){}
		~engine(){}
		bool dispatch(){}
	};

	class base_task {
	public:
		base_task(base_engine&, actor*, task_adaptor){}
		virtual void run(){}
		void submit(){}
	};

	class base_engine {
		base_engine(){}
		~base_engine(){}
		void run(){}
	};
}