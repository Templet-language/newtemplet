// taskemul.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <cmath>
#include <iostream>
#include "..\..\lib\taskemul.hpp"

using namespace std;
using TEMPLET::taskengine;
using TEMPLET::task;

const double _sin2_duration = 1.0;
const double _cos2_duration = 1.0;
const double _sum_duration = 1.0;

int main()
{
	taskengine eng;

	task do_sin_2(eng);
	task do_cos_2(eng);
	task do_sum(eng);

	double x = 123.456, _sin2 = 0.0, _cos2 = 0.0;

	do_sin_2.submit([x, &_sin2, &eng]() {_sin2 = sin(x)*sin(x); eng.delay(_sin2_duration); });
	do_cos_2.submit([x, &_cos2, &eng]() {_cos2 = cos(x)*cos(x); eng.delay(_cos2_duration); });

	eng.wait_for(do_sin_2);
	eng.wait_for(do_cos_2);

	do_sum.submit([&]() { cout << "sin2("<<x<<") + cos2("<<x<<") = " << _sin2 + _cos2 << endl;
		eng.delay(_sum_duration);
	});

	eng.wait_all();

	cout << "T1 = " << eng.T1() << endl;
	cout << "Tp = " << eng.Tp() << endl;
	cout << "Pmax = " << eng.Pmax() << endl;

    return 0;
}

