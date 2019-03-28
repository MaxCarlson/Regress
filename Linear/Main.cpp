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

	Eigen::Matrix<double, -1, -1, Eigen::RowMajor> mat1;
	mat1.resize(1000, 1000);

	Stopwatch s;
	s.start();
	mat = mat * mat;
	s.printCurrent();
	s.start();
	mat1 = mat1 * mat1;

	s.printCurrent();
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

	Matrix<float> med = {
		{ 1,  2,  3,  4},	
		{ 5,  6,  7,  8},  
		{ 9, 10, 11, 12},
		{13, 14, 15, 16}
	};



	//Stopwatch<std::chrono::nanoseconds> s;
	//s.start();

	Matrix<float> vv = med * med;
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