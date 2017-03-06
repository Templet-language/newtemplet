/*--------------------------------------------------------------------------*/
/*  Copyright 2010-2016 Sergei Vostokin                                     */
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

// one more way to implement DSL in C++ instead of DSL in #pragma templet ...

#pragma once

namespace TEMPLET{ 
namespace dsl{ // domain-specific language interface

		class message{
		public:
			message(std::string name);
			message();
			~message();
		public:
			message& name(std::string name);
			message& duplex();
			message& serializable();
			message& in(std::string name, bool content = true);
			message& out(std::string name, bool content = true);
		};

		class actor{
		public:
			actor(std::string name);
			actor();
			~actor();
		public:
			actor& name(std::string name);
			actor& serializable();
			actor& startable();
			actor& in(std::string port_name, std::string message_name);
			actor& out(std::string port_name, std::string message_name);
			actor& any();
		};
}
}
///    DSL block for the tridentity sample 
void dsl()
{
/*$TET$dsl*/
#pragma templet ~value_message$=
	TEMPLET::dsl::message m("value_message"); m.serializable().duplex();

#pragma templet *master$(sin2_port!value_message,cos2_port!value_message)+	
	TEMPLET::dsl::actor mst("master"); mst.serializable().startable().
		out("sin2_port", "value_message").out("cos2_port", "value_message");
		
#pragma templet *worker(master_port?value_message)
	TEMPLET::dsl::actor wkr("worker");
		wkr.in("master_port", "value_message");
/*$TET$*/
}

