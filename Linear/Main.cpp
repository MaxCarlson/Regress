#include "Matrix.h"
#include "Stopwatch.h"


#include "MatrixT.h" // Testing

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


	MatrixT<int> lhst = {
		{1, 2},
		{3, 4},
		{5, 6},
	};

	MatrixT<int> rhst = {
		{1, 2, 3},
		{4, 5, 6}
	};
	auto a = lhst * rhst * lhst;
	MatrixT<int> val = a;



	//Matrix<int> tp = (tl * tr) * tl;

	Matrix<int> tl(90, 90);
	Matrix<int> tr(90, 90);
	Stopwatch<std::chrono::milliseconds> w;
	w.start();
	Matrix<int> v = (lhs * rhs) * lhs;
	w.printCurrent();




	return 0;
}