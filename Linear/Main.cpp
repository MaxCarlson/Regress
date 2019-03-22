#include "Matrix.h"
#include "Stopwatch.h"
#include "Matrix.h" 
#include <Eigen\Dense>
#include "Evaluators\Packets.h"

using Eigen::MatrixXd;



int main()
{
	float ar1[4] = { 1, 2, 3, 4 };
	float ar2[4] = { 5, 6, 7, 8 };

	__m128 first;
	__m128 second;
	first	= _mm_load_ps(ar1);
	second	= _mm_load_ps(ar2);

	///*
	MatrixXd mat;
	mat.resize(2, 2);
	auto mm = mat * mat;
	auto mmm = mat - mat;
	auto mmmm = mat / 2;

	mat = mm;
	//*/

	Matrix<int> lhst = {
		{1, 2},
		{3, 4},
		{5, 6},
	};

	Matrix<int> rhst = {
		{1, 2, 3},
		{4, 5, 6}
	};

	Matrix<int> bigger = {
		{1, 2, 3, 4, 1},
		{3, 4, 4, 5, 1},
		{5, 6, 6, 7, 1},
		{5, 6, 6, 7, 1},
	};

	// lhs * rhs * lhs
	// {1, 2},					{9,  12, 15}
	// {3, 4}, * {1, 2, 3}, =	{19, 26, 33}
	// {5, 6},   {4, 5, 6}		{29, 40, 51}
	// 
	// {9,  12, 15}		{1, 2},		{120, 156}
	// {19, 26, 33} *	{3, 4}, =	{262, 340}
	// {29, 40, 51}		{5, 6},		{404, 524}

	//Stopwatch s;
	//s.start();

	auto v = 2 * rhst;
	auto vvv = 2 + lhst;
	auto vvvv = 2 - lhst;
	auto vvvvv = 2 / lhst;

	Matrix<int> vv = v;
	//s.printCurrent();

	return 0;
}