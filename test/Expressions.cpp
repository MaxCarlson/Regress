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
			MatrixT res1 = ~lhs + rhs;
			MatrixT res2 = ~rhs * ~lhs;
			MatrixT res3 = (~rhs * ~lhs) * lhs1;
			MatrixT res4 = (~rhs * ~lhs) - lhs1;
			MatrixT res5 = ((~rhs * ~lhs) - lhs1) * 2;
			MatrixT res6 = ((~rhs * ~lhs) - lhs1) + (lhs * rhs - lhs1);

			MatrixT res1v = { {2, 5, 8}, {6, 9, 12} };
			MatrixT res2v = { {9, 19, 29}, {12, 26, 40}, {15, 33, 51} };
			MatrixT res3v = { {288, 345, 402}, {396, 474, 552}, {504, 603, 702} };
			MatrixT res4v = { {8, 17, 26}, {8, 21, 34}, {8, 25, 42} };
			MatrixT res5v = { {16, 34, 52}, {16, 42, 68}, {16, 50, 84} };
			MatrixT res6v = { {16, 27, 38}, {23, 42, 61}, {30, 57, 84} };


			Assert::IsTrue(res1 == res1v);
			Assert::IsTrue(res2 == res2v);
			Assert::IsTrue(res3 == res3v);
			Assert::IsTrue(res4 == res4v);
			Assert::IsTrue(res5 == res5v);
			Assert::IsTrue(res6 == res6v);
		}
	};
}