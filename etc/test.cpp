#include "pch.h"
#include <iostream>

struct A{};

struct B{
	void (A::*mref)();
	A* oref;

	template <class T>
	void setup(void (T::*m)(), A*o) {
		mref = reinterpret_cast<void(A::*)()>(m); oref = o;
	}

	void call() {
		(oref->*mref)();
	}
};

struct C:A{
	void f(B&b) {
		b.setup(&C::xxx,this);
	}

	void xxx() {
		std::cout << "hello";
	}
};

int main()
{
	C c; B b;
	c.f(b);
	b.call();
}