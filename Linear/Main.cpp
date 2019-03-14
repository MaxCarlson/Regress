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


	//
	// {1, 2},					{9,  12, 15}
	// {3, 4}, * {1, 2, 3}, =	{19, 26, 33}
	// {5, 6},   {4, 5, 6}		{29, 40, 51}
	// 
	// {9,  12, 15}		{1, 2},		{120, 156}
	// {19, 26, 33} *	{3, 4}, =	{262, 340}
	// {29, 40, 51}		{5, 6},		{404, 524}

	MatrixT<int> bb = lhst / 2.06;
	
	auto a = lhst * rhst * lhst + lhst * rhst * lhst;
	MatrixT<int> val = a;
	//Matrix<int> val2 = lhs * rhs * lhs;



	//Matrix<int> tp = (tl * tr) * tl;

	Matrix<int> tl(90, 90);
	Matrix<int> tr(90, 90);
	Stopwatch<std::chrono::milliseconds> w;
	w.start();
	Matrix<int> v = (lhs * rhs) * lhs;
	w.printCurrent();




	return 0;
}