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
	Eigen::Matrix<float, -1, -1, Eigen::RowMajor> mat1;
	Eigen::Matrix<int, -1, -1, Eigen::RowMajor> mat2;
	mat1.resize(2500, 2500);
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
	//fillMat(mat1);
	//fillMat(mat2);

	//Eigen::Matrix<int, -1, -1, Eigen::RowMajor>mm = mat1 * mat2;
	static constexpr int numTests = 20;
	s.start();
	for (int i = 0; i < numTests; ++i)
		mat1 = mat1 * mat1;
	s.printCurrent(numTests);
	Matrix<float> tt(2500, 2500);
	s.start();
	for (int i = 0; i < numTests; ++i)
		tt = tt* tt;
	s.printCurrent(numTests);
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

	Matrix<double> med = {
		{ 1,  2,  3,  4,  5 },	// 1  6 11 16 21
		{ 6,  7,  8,  9,  10},	// 2  7 12 17 22
		{ 11, 12, 13, 14, 15},	// 3  8 13 18 23
		{ 16, 17, 18, 19, 20},	// 4  9 14 19 24
		{ 21, 22, 23, 24, 25}	// 5 10 15 20 25
	};
	Matrix<double, true> medC = med;
	Matrix<int> test1(42, 32);
	Matrix<int> test2(32, 43);
	std::iota(test1.begin(), test1.end(), 1);
	std::iota(test2.begin(), test2.end(), 1);
	std::cout << test1 << "\n\n" << test2 << "\n\n";
	Matrix<int> testRes = test1 * test2;
	std::cout << testRes << "\n\n";

	constexpr int m = 42, k = 32, n = 32;
	constexpr int packedN3 = n - n % (4 * 3);
	constexpr int packedN2 = n - (n - packedN3) % (4 * 2);
	constexpr int packedN  = n - (n - packedN2) % 4;


	//Matrix<int> reslr = lhs * rhs; // BUG:
	Matrix<double> resMed = med * med;
	//std::cout << reslr << "\n\n";
	std::cout << resMed << "\n\n";

	return 0;
}