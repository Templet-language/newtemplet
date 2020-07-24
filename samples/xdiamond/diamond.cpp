/*--------------------------------------------------------------------------*/
/*  Copyright 2020 Sergei Vostokin                                          */
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

#include "diamond.hpp"
#include <iostream>

int main()
{
	engine eng;
	base_engine teng;

	A a(eng,teng);
	B b(eng,teng);
	C c(eng,teng);
	D d(eng,teng);

	b.a(a.b); c.a(a.c);
	d.b(b.d); d.c(c.d);

	eng.start();
	teng.run();

	if (eng.stopped()) {
		std::cout << "success!!!" << std::endl;
		return EXIT_SUCCESS;
	}

	std::cout << "failure..." << std::endl;
	return EXIT_FAILURE;
}