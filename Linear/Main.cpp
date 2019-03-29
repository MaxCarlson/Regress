#include "Matrix.h"
#include "Stopwatch.h"
#include "Matrix.h" 
#include <Eigen\Dense>
#include "Instructions\Packets.h"

using Eigen::MatrixXd;

#include <immintrin.h>

int main()
{
	Stopwatch<std::chrono::milliseconds> s;

	/*
	MatrixXd mat;
	mat.resize(200, 200);
	s.start();
	mat = mat * mat;
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


	Matrix<int> large(200, 200);

	s.start();
	Matrix<int> vv = huge * huge;
	s.printCurrent();

	std::cout << vv << "\n\n";

	Matrix<int> res3v = { {47, 58, 59, 70, 11},
		{65, 82, 85, 102, 17}, {93, 118, 123, 148, 25},
		{93, 118, 123, 148, 25}, {93, 118, 123, 148, 25} };
	std::cout << res3v << '\n';

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