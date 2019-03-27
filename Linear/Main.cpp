#include "Matrix.h"
#include "Stopwatch.h"
#include "Matrix.h" 
#include <Eigen\Dense>
#include "Instructions\Packets.h"

using Eigen::MatrixXd;

#include <immintrin.h>

int main()
{
	int aa[] = { 1, 2, 3, 4 };
	__m128i rr = impl::pload<__m128i>(aa);
	auto aaa = impl::pbroadcast<__m128i, int>(aa);
	/*
	MatrixXd mat;
	mat.resize(1000, 1000);
	mat(0, 0) = 1;
	mat(0, 1) = 2;
	mat(1, 0) = 3;
	mat(1, 1) = 4;

	auto mm = mat * mat * mat.transpose();
	mat = mm;
	//*/

	Matrix<int> lhs = {
		{1, 2},
		{3, 4},
		{5, 6},
	};

	Matrix<int> rhs = {
		{1, 2, 3},
		{4, 5, 6}
	};

	Matrix<int> bigger = {
		{1, 2, 3, 4, 1},
		{3, 4, 4, 5, 1},
		{5, 6, 6, 7, 1},
		{5, 6, 6, 7, 1},
		{5, 6, 6, 7, 1}
	};

	// lhs * rhs * lhs
	// {1, 2},					{9,  12, 15}
	// {3, 4}, * {1, 2, 3}, =	{19, 26, 33}
	// {5, 6},   {4, 5, 6}		{29, 40, 51}
	// 
	// {9,  12, 15}		{1, 2},		{120, 156}
	// {19, 26, 33} *	{3, 4}, =	{262, 340}
	// {29, 40, 51}		{5, 6},		{404, 524}

	// Assuming doubles with SSE
	// 
	// 1 3   1 2   11 33 22 44
	// 2 4   4 5   12 12 45 45
	//		

	//
	// 1 2  1 2		11 22 33 44
	// 3 4  4 5		12 45 12 45
	//

	//Stopwatch<std::chrono::nanoseconds> s;
	//s.start();

	Matrix<int> vv = bigger * bigger;
	//s.printCurrent();

	std::cout << vv << '\n';

	/*
	Eigen::MatrixXi t;
	t.resize(5, 5);
	s.start();

	Eigen::MatrixXi tt = t + t + t + t;
	s.printCurrent();
	s.start();
	//*/

	return 0;
}