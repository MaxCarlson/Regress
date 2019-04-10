#include "Matrix.h"
#include "Stopwatch.h"
#include "Matrix.h" 
#include <Eigen\Dense>
#include "Instructions\Packets.h"
#include <numeric>
using Eigen::MatrixXd;

int main()
{
	Stopwatch<std::chrono::milliseconds> s;

	/*
	Eigen::Matrix<int, -1, -1, Eigen::RowMajor> mat1;
	Eigen::Matrix<int, -1, -1, Eigen::RowMajor> mat2;
	mat1.resize(25, 25);
	mat2.resize(25, 25);
	int idx = 1;
	auto fillMat = [&](auto& mat)
	{
		for (int i = 0; i < 25; ++i)
		{
			for (int j = 0; j < 25; ++j)
			{
				mat(i, j) = idx;
				std::cout << idx++ << ", ";
			}
			std::cout << '\n';
		}
		std::cout << "\n";
	};
	fillMat(mat1);
	fillMat(mat2);

	//Eigen::Matrix<int, -1, -1, Eigen::RowMajor>mm = mat1 * mat2;
	mat1 = mat1 * mat2;
	//*/

	Matrix<int> lhs = {
		{1, 2},
		{3, 4},
		{5, 6},
	};
	Matrix<int, true> lhsC = lhs;

	Matrix<int> rhs = {
		{1, 2, 3},
		{4, 5, 6}
	};
	Matrix<int> rhsC = rhs;

	Matrix<int> med = {
		{ 1,  2,  3,  4,  5 },	// 1  6 11 16 21
		{ 6,  7,  8,  9,  10},	// 2  7 12 17 22
		{ 11, 12, 13, 14, 15},	// 3  8 13 18 23
		{ 16, 17, 18, 19, 20},	// 4  9 14 19 24
		{ 21, 22, 23, 24, 25}	// 5 10 15 20 25
	};
	Matrix<double, true> medC = med;

	//Matrix<int> large(10, 10);
	//std::iota(large.begin(), large.end(), 1);
	//std::cout << large;
	//Matrix<int> largeRes = large * large;
	//Matrix<int> res2 = lhs * rhs;
	//Matrix<int> res1 = lhsC * rhs;
	//Matrix<int> res3 = lhs * rhsC;
	//std::cout << res2 << "\n\n";
	//std::cout << res1 << "\n\n";
	//std::cout << res3 << "\n\n";
	
	Matrix<int> resMed = med * med;
	Matrix<double> resMed1 = medC * med; 
	Matrix<double> resMed2 = med * medC;
	std::cout << resMed << "\n\n";
	//std::cout << resMed1 << "\n\n";
	//std::cout << resMed2 << "\n\n";

	//Matrix<int> res = lhs * rhs * lhs;
	//std::cout << res << "\n\n";

	s.start();
	//Matrix<int> vv = lhs * rhs;
	Matrix<double> vv = med * med;
	//Matrix<int> vv = medL * medR;
	s.printCurrent();

	std::cout << vv << "\n\n";

	return 0;
}