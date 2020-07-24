#include "diamond.hpp"
#include <iostream>

int main()
{
	engine eng;
	base_engine teng;

	A a(eng,teng);
	B b(eng, teng);
	C c(eng, teng);
	D d(eng, teng);

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