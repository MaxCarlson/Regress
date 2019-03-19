#include "Matrix.h"
#include "Stopwatch.h"

#include "MatrixT.h" // Testing
#include <Eigen\Dense>
using Eigen::MatrixXd;

int main()
{
	///*
	MatrixXd mat;
	mat.resize(2, 2);
	auto mm = mat * mat + mat;
	mat = mm;
	//*/

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


	// lhs * rhs * lhs
	// {1, 2},					{9,  12, 15}
	// {3, 4}, * {1, 2, 3}, =	{19, 26, 33}
	// {5, 6},   {4, 5, 6}		{29, 40, 51}
	// 
	// {9,  12, 15}		{1, 2},		{120, 156}
	// {19, 26, 33} *	{3, 4}, =	{262, 340}
	// {29, 40, 51}		{5, 6},		{404, 524}

	//MatrixT<int> bb = lhst / 2.06;

	//auto cc = (lhst + lhst) * rhst;
	//auto cc = lhst * rhst * lhst;
	//MatrixT<int> c = cc;

	//auto ll = (lhst * rhst).transpose() * lhst;
	//MatrixT<int> tt = ll;

	auto v = lhst * rhst * lhst;
	MatrixT<int> vv = v;
	//impl::ProductEvaluator<decltype(v)> be{ v };
	
	//auto a = lhst * rhst * lhst + lhst * rhst * lhst;
	//MatrixT<int> val = a;
	//Matrix<int> val2 = lhs * rhs * lhs + lhs * rhs * lhs;




	return 0;
}