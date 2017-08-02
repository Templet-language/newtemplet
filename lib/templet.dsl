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

void def()
{
	message m1("m1"), m2("m2");
	m1.duplex().serializable();

	actor a("a");
	a.serializable().startable();
	a.out("out_port","m1").in("in_port","m2");
}

#pragma templet ~m1$=
struct m1:message{
	void send();
	//
	void save(saver*s){/*--*/}
	void restore(restorer*r){/*--*/}
	/*--*/
};

#pragma templet ~m2
struct m2:message{
	void send();
	/*--*/
};

#pragma templet *a$(out_port!m1,in_port?m2)+
struct a:actor{
	a(engine&);
	access(message&);
	//
	void save(saver*s){/*--*/}
	void restore(restorer*r){/*--*/}
	//
	void start(){/*--*/}
	void out_port(m1& m){/*--*/}
	void in_port(m2& m){/*--*/}
	//
	m1* out_port();
	void in_port(m2*);
	//
	m1 _out_port;
	/*--*/
};