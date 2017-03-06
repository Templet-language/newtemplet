/*$TET$header*/
/*--------------------------------------------------------------------------*/
/*  Copyright 2016 Sergei Vostokin                                          */
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

#include <iostream>
#include <templet.hpp>

using namespace std;

/*$TET$*/

using namespace TEMPLET;

class my_engine{
public:
	my_engine(int argc, char *argv[]);
	void run();
	void map();
};

#pragma templet ~mes=

struct mes : message{
	bool access(actor*);
	void send();

/*$TET$mes$$data*/
	char _mes[100];
/*$TET$*/
};

#pragma templet *ping(p!mes)+

struct ping : actor{
	ping(my_engine&){
/*$TET$ping$ping*/
/*$TET$*/
	}

	void delay(double);
	void at(int);
	void stop();

	mes* p();

	void start(){
/*$TET$ping$start*/
		strcpy(_p._mes, "Hello PONG!!!");
		_p.send();
/*$TET$*/
	}

	void p(mes&m){
/*$TET$ping$p*/
		cout << m._mes;
		stop();
/*$TET$*/
	}

/*$TET$ping$$code&data*/
/*$TET$*/

	mes _p;
};

#pragma templet *pong(p?mes)

struct pong : actor{
	pong(my_engine&){
/*$TET$pong$pong*/
/*$TET$*/
	}

	void delay(double);
	void at(int);
	void stop();

	void p(mes*);

	void p(mes&m){
/*$TET$pong$p*/
		cout << m._mes << endl;
		strcpy(m._mes, "Hello PING!!!");
		m.send();
/*$TET$*/
	}

/*$TET$pong$$code&data*/
/*$TET$*/

};

/*$TET$footer*/
void main(int argc, char *argv[])
{
	my_engine e(argc, argv);

	ping a_ping(e);
	pong a_pong(e);

	a_pong.p(a_ping.p());

	e.run();
}
/*$TET$*/
