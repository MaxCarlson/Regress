#include "Matrix.h"
#include "Stopwatch.h"

int main()
{
	///*
	Matrix<int> lhs = {
		{1, 2},
		{3, 4},
		{5, 6},
	};

	Matrix<int> rhs = {
		{1, 2, 3},
		{4, 5, 6}
	};
	//*/

	Matrix<int, true> lhsr = {
		{1, 2},
		{3, 4},
		{5, 6},
	};

	Matrix<int, true> rhsr = {
		{1, 2, 3},
		{4, 5, 6}
	};

	Matrix<int> tl(90, 90);
	Matrix<int> tr(90, 90);

	auto a = lhs / lhs;

	Stopwatch<std::chrono::milliseconds> w;
	w.start();

	//Matrix<int> tp = (tl * tr) * tl;
	w.printCurrent();

	Matrix<int> v = (lhs * rhs) * lhs;




	return 0;
}