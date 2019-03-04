#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Regress/Linear/Matrix.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test
{		
	TEST_CLASS(MatrixExpressions)
	{
	public:

		using T = int;
		using MatrixT = Matrix<T>;

		MatrixT lhs = {
			{1, 2},
			{3, 4},
			{5, 6},
		};

		MatrixT rhs = {
			{1, 2, 3},
			{4, 5, 6}
		};
		
		MatrixT lhs1 = {
			{1, 2, 3},
			{4, 5, 6},
			{7, 8, 9},
		};

		MatrixT rhs1 = {
			{1, 2, 3, 4},
			{5, 6, 7, 8}
		};

		TEST_METHOD(Add)
		{
			MatrixT res1 = rhs + rhs;
			MatrixT res2 = (lhs + lhs) + lhs;

			MatrixT res1v = { {2, 4, 6}, {8, 10, 12} };
			MatrixT res2v = { {3, 6}, {9, 12}, {15, 18} };

			Assert::IsTrue(res1 == res1v);
			Assert::IsTrue(res2 == res2v);
		}

		TEST_METHOD(Sub)
		{
			MatrixT res1 = rhs - rhs;
			MatrixT res2 = (lhs - lhs) - lhs;

			MatrixT res1v = { {0, 0, 0}, {0, 0, 0} };
			MatrixT res2v = { {-1, -2}, {-3, -4}, {-5, -6} };

			Assert::IsTrue(res1 == res1v);
			Assert::IsTrue(res2 == res2v);
		}

		TEST_METHOD(Mul)
		{
			MatrixT res1	= (lhs * rhs) * lhs;
			MatrixT res2	= (lhs * rhs) * lhs * rhs1;

			MatrixT res1v	= { {120, 156}, {262, 340}, {404, 524} };
			MatrixT res2v	= { {900, 1176, 1452, 1728},
			{1962, 2564, 3166, 3768}, {3024, 3952, 4880, 5808} };

			Assert::IsTrue(res1 == res1v);
			Assert::IsTrue(res2 == res2v);
		}

		TEST_METHOD(Transpose)
		{
			MatrixT res1 = ~lhs;
			MatrixT res2 = ~lhs1;

			MatrixT res1v = { {1, 3, 5}, {2, 4, 6} };
			MatrixT res2v = { {1, 4, 7}, {2, 5, 8}, {3, 6, 9} };

			Assert::IsTrue(res1 == res1v);
			Assert::IsTrue(res2 == res2v);
		}

		TEST_METHOD(Mixed)
		{

		}
	};
}