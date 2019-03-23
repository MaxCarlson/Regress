#include "Matrix.h"
#include "Stopwatch.h"
#include "Matrix.h" 
#include <Eigen\Dense>
#include "Evaluators\Packets.h"

using Eigen::MatrixXd;


int main()
{
	/*
	MatrixXd mat;
	mat.resize(2, 2);
	auto mm = mat * mat;
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

	//Stopwatch<std::chrono::nanoseconds> s;
	//s.start();

	//auto v = 2 * rhst;
	Matrix<int> vv = (bigger * bigger + bigger.transpose()) + (bigger * bigger + bigger.transpose());
	Matrix<int> res7v = { {48, 61, 64, 75, 16}, {67, 86, 91, 108, 23},
			{96, 122, 129, 154, 31}, {97, 123, 130, 155, 32}, {94, 119, 124, 149, 26} };

	Matrix<int> ll = res7v * 2;


	//s.printCurrent();
	//s.start();

	std::cout << vv << '\n';
	std::cout << ll << '\n';
	bool b = vv == ll;

	/*
	Eigen::MatrixXi t;
	t.resize(5, 5);
	Eigen::MatrixXi tt;
	tt.noalias() = t * t * t * t;
	s.printCurrent();
	s.start();
	*/



	return 0;
}