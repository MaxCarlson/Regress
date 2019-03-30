#include "Matrix.h"
#include "Stopwatch.h"
#include "Matrix.h" 
#include <Eigen\Dense>
#include "Instructions\Packets.h"

using Eigen::MatrixXd;

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

	Matrix<int, true> lhs = {
		{1, 2},
		{3, 4},
		{5, 6},
	};

	Matrix<int> rhs = {
		{1, 2, 3},
		{4, 5, 6}
	};


	Matrix<int, true> bigger = {
		{ 1,  2,  3,  4,  5 },	
		{ 6,  7,  8,  9,  10},
		{ 11, 12, 13, 14, 15},
		{ 16, 17, 18, 19, 20},
		{ 21, 22, 23, 24, 25}
	};



	// TODO: Why does this cause illegal void erros??
	//Matrix<int64_t> gg = { {1} }; 
	//gg = gg * gg;

	//Matrix<int> large(200, 200);
	s.start();
	Matrix<int, true> vv = lhs * rhs;
	s.printCurrent();

	std::cout << vv << "\n\n";

	Matrix<int> bigRes = { 
		{215, 230, 245, 260, 275}, 
		{490, 530, 570, 610, 650},
		{765, 830, 895, 960, 1025}, 
		{1040, 1130, 1220, 1310, 1400}, 
		{1315, 1430, 1545, 1660, 1775} 
	};
	std::cout << bigRes << '\n';

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