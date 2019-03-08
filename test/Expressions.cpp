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
		using cMM = Matrix<T, true>;
		using rMM = Matrix<T, false>;

		rMM lhs = {
			{1, 2},
			{3, 4},
			{5, 6},
		};

		rMM rhs = {
			{1, 2, 3},
			{4, 5, 6}
		};

		cMM lhsc = {
			{1, 2},
			{3, 4},
			{5, 6},
		};

		cMM rhsc = {
			{1, 2, 3},
			{4, 5, 6}
		};
		
		rMM lhs1 = {
			{1, 2, 3},
			{4, 5, 6},
			{7, 8, 9},
		};

		rMM rhs1 = {
			{1, 2, 3, 4},
			{5, 6, 7, 8}
		};

		cMM rhs1c = {
			{1, 2, 3, 4},
			{5, 6, 7, 8}
		};

		TEST_METHOD(Add)
		{
			rMM res1 = rhs + rhs;
			rMM res2 = (lhs + lhs) + lhs;

			rMM res1v = { {2, 4, 6}, {8, 10, 12} };
			rMM res2v = { {3, 6}, {9, 12}, {15, 18} };

			Assert::IsTrue(res1 == res1v);
			Assert::IsTrue(res2 == res2v);
		}

		TEST_METHOD(Sub)
		{
			rMM res1 = rhs - rhs;
			rMM res2 = (lhs - lhs) - lhs;

			rMM res1v = { {0, 0, 0}, {0, 0, 0} };
			rMM res2v = { {-1, -2}, {-3, -4}, {-5, -6} };

			Assert::IsTrue(res1 == res1v);
			Assert::IsTrue(res2 == res2v);
		}

		template<class Mat>
		void mulImpl(const Mat& lhs, const Mat& rhs, const Mat& rhs1)
		{
			Mat res1 = (lhs * rhs) * lhs;
			Mat res2 = (lhs * rhs) * lhs * rhs1;

			Mat res1v = { {120, 156}, {262, 340}, {404, 524} };
			Mat res2v = { {900, 1176, 1452, 1728},
			{1962, 2564, 3166, 3768}, {3024, 3952, 4880, 5808} };

			Assert::IsTrue(res1 == res1v);
			Assert::IsTrue(res2 == res2v);
		}

		TEST_METHOD(Mul)
		{
			mulImpl(lhs, rhs, rhs1);
			mulImpl(lhsc, rhsc, rhs1c);
		}

		TEST_METHOD(Constant)
		{
			rMM res1 = lhs + 2;
			rMM res2 = rhs + 2 - 3;
			rMM res3 = rhs * 3;
			rMM res4 = 2 + rhs * 3;

			rMM res1v = { {3, 4}, {5, 6}, {7, 8} };
			rMM res2v = { {0, 1, 2}, {3, 4, 5} };
			rMM res3v = { {3, 6, 9}, {12, 15, 18} };
			rMM res4v = { {5, 8, 11}, {14, 17, 20} };

			Assert::IsTrue(res1 == res1v);
			Assert::IsTrue(res2 == res2v);
			Assert::IsTrue(res3 == res3v);
			Assert::IsTrue(res4 == res4v);
		}


		TEST_METHOD(Transpose)
		{
			rMM res1 = ~lhs;
			rMM res2 = ~lhs1;

			rMM res1v = { {1, 3, 5}, {2, 4, 6} };
			rMM res2v = { {1, 4, 7}, {2, 5, 8}, {3, 6, 9} };

			Assert::IsTrue(res1 == res1v);
			Assert::IsTrue(res2 == res2v);
		}

		TEST_METHOD(Mixed)
		{
			rMM res1 = ~lhs + rhs;
			rMM res2 = ~rhs * ~lhs;
			rMM res3 = (~rhs * ~lhs) * lhs1;
			rMM res4 = (~rhs * ~lhs) - lhs1;
			rMM res5 = ((~rhs * ~lhs) - lhs1) * 2;
			rMM res6 = ((~rhs * ~lhs) - lhs1) + (lhs * rhs - lhs1);

			rMM res1v = { {2, 5, 8}, {6, 9, 12} };
			rMM res2v = { {9, 19, 29}, {12, 26, 40}, {15, 33, 51} };
			rMM res3v = { {288, 345, 402}, {396, 474, 552}, {504, 603, 702} };
			rMM res4v = { {8, 17, 26}, {8, 21, 34}, {8, 25, 42} };
			rMM res5v = { {16, 34, 52}, {16, 42, 68}, {16, 50, 84} };
			rMM res6v = { {16, 27, 38}, {23, 42, 61}, {30, 57, 84} };


			Assert::IsTrue(res1 == res1v);
			Assert::IsTrue(res2 == res2v);
			Assert::IsTrue(res3 == res3v);
			Assert::IsTrue(res4 == res4v);
			Assert::IsTrue(res5 == res5v);
			Assert::IsTrue(res6 == res6v);
		}
	};
}