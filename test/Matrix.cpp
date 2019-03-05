#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Regress/Linear/Matrix.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test
{
TEST_CLASS(MatrixTest)
{
public:

	using MatrixT = Matrix<int>;

	MatrixT lhs = {
		{1, 2},
		{3, 4},
		{5, 6},
	};

	MatrixT rhs = {
		{1, 2, 3},
		{4, 5, 6}
	};

	TEST_METHOD(Indexing)
	{
		auto v1 = lhs(0, 0);
		auto v2 = lhs(0, 1);
		auto v3 = lhs(2, 1);

		auto v4 = rhs(0, 0);
		auto v5 = rhs(0, 2);
		auto v6 = rhs(1, 2);

		Assert::IsTrue(v1 == 1);
		Assert::IsTrue(v2 == 2);
		Assert::IsTrue(v3 == 6);
		Assert::IsTrue(v4 == 1);
		Assert::IsTrue(v5 == 3);
		Assert::IsTrue(v6 == 6);

		Assert::IsTrue(lhs.size() == 6);
		Assert::IsTrue(rhs.size() == 6);
		Assert::IsTrue(lhs.rows() == 3);
		Assert::IsTrue(rhs.rows() == 2);
		Assert::IsTrue(lhs.cols() == 2);
		Assert::IsTrue(rhs.cols() == 3);
	}

	TEST_METHOD(Iterators)
	{
		
	}

	TEST_METHOD(AddColumn)
	{
		auto v1 = lhs;
		v1.addColumn(2, 3); // TODO: Issue here
		MatrixT v1r = { {1, 2, 3}, {3, 4, 3}, {5, 6, 3} };

		auto v2 = rhs;
		v2.addColumn(0);
		MatrixT v2r = { {0, 1, 2, 3}, {0, 4, 5, 6} };

		Assert::IsTrue(v1 == v1r);
		Assert::IsTrue(v2 == v2r);
	}
};

}