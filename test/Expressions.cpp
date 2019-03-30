#include "stdafx.h"
#include "CppUnitTest.h"
#include "Matrix.h"


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

		cMM lhs1c = {
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

		rMM bigger = {
			{1, 2, 3, 4, 1},
			{3, 4, 4, 5, 1},
			{5, 6, 6, 7, 1},
			{5, 6, 6, 7, 1},
			{5, 6, 6, 7, 1}
		};

		cMM biggerc = {
			{1, 2, 3, 4, 1},
			{3, 4, 4, 5, 1},
			{5, 6, 6, 7, 1},
			{5, 6, 6, 7, 1},
			{5, 6, 6, 7, 1}
		};

		template<class Mat>
		void addImpl(const Mat& lhs, const Mat& rhs)
		{
			Mat res1 = rhs + rhs;
			Mat res2 = (lhs + lhs) + lhs;

			Mat res1v = { {2, 4, 6}, {8, 10, 12} };
			Mat res2v = { {3, 6}, {9, 12}, {15, 18} };

			Assert::IsTrue(res1 == res1v);
			Assert::IsTrue(res2 == res2v);
		}

		TEST_METHOD(Add)
		{
			addImpl(lhs, rhs);
			addImpl(lhsc, rhsc);
		}

		template<class Mat>
		void subImpl(const Mat& lhs, const Mat& rhs)
		{
			Mat res1 = rhs - rhs;
			Mat res2 = (lhs - lhs) - lhs;

			Mat res1v = { {0, 0, 0}, {0, 0, 0} };
			Mat res2v = { {-1, -2}, {-3, -4}, {-5, -6} };

			Assert::IsTrue(res1 == res1v);
			Assert::IsTrue(res2 == res2v);
		}

		TEST_METHOD(Sub)
		{
			subImpl(lhs, rhs);
			subImpl(lhsc, rhsc);
		}

		template<class Mat>
		void mulImpl(const Mat& lhs, const Mat& rhs, const Mat& rhs1, const Mat& big)
		{
			using Type = typename Mat::value_type;
			enum { MajorOrder = Mat::MajorOrder };
			using OMat = Matrix<Type, !MajorOrder>;

			Mat res1 = (lhs * rhs) * lhs;
			Mat res2 = (lhs * rhs) * lhs * rhs1;
			Mat res3 = big * big;
			//Mat res4 =

			Mat res1v = { {120, 156}, {262, 340}, {404, 524} };
			Mat res2v = { {900, 1176, 1452, 1728},
			{1962, 2564, 3166, 3768}, {3024, 3952, 4880, 5808} };
			Mat res3v = { {47, 58, 59, 70, 11},
			{65, 82, 85, 102, 17}, {93, 118, 123, 148, 25},
			{93, 118, 123, 148, 25}, {93, 118, 123, 148, 25} };

			Assert::IsTrue(res1 == res1v);
			Assert::IsTrue(res2 == res2v);
			Assert::IsTrue(res3 == res3v);
		}

		TEST_METHOD(Mul)
		{
			mulImpl(lhs, rhs, rhs1, bigger);
			mulImpl(lhsc, rhsc, rhs1c, biggerc);
		}

		template<class Mat>
		void constantImpl(const Mat& lhs, const Mat& rhs)
		{
			Mat res1 = lhs + 2;
			Mat res2 = rhs + 2 - 3;
			Mat res3 = rhs * 3;
			Mat res4 = 2 + rhs * 3;

			Mat res1v = { {3, 4}, {5, 6}, {7, 8} };
			Mat res2v = { {0, 1, 2}, {3, 4, 5} };
			Mat res3v = { {3, 6, 9}, {12, 15, 18} };
			Mat res4v = { {5, 8, 11}, {14, 17, 20} };

			Assert::IsTrue(res1 == res1v);
			Assert::IsTrue(res2 == res2v);
			Assert::IsTrue(res3 == res3v);
			Assert::IsTrue(res4 == res4v);
		}

		TEST_METHOD(Constant)
		{
			constantImpl(lhs, rhs);
			constantImpl(lhsc, rhsc);
		}

		template<class Mat>
		void transposeImpl(const Mat& lhs, const Mat& lhs1)
		{
			Mat res1 = lhs.transpose();
			Mat res2 = lhs1.transpose();

			Mat res1v = { {1, 3, 5}, {2, 4, 6} };
			Mat res2v = { {1, 4, 7}, {2, 5, 8}, {3, 6, 9} };

			Assert::IsTrue(res1 == res1v);
			Assert::IsTrue(res2 == res2v);
		}

		TEST_METHOD(Transpose)
		{
			transposeImpl(lhs, lhs1);
			transposeImpl(lhsc, lhs1c);
		}

		template<class Mat>
		void mixedImpl(const Mat& lhs, const Mat& lhs1, const Mat& rhs, const Mat& big)
		{
			Mat res1 = lhs.transpose() + rhs;
			Mat res2 = rhs.transpose() * lhs.transpose();
			Mat res3 = (rhs.transpose() * lhs.transpose()) * lhs1;
			Mat res4 = (rhs.transpose() * lhs.transpose()) - lhs1;
			Mat res5 = ((rhs.transpose() * lhs.transpose()) - lhs1) * 2;
			Mat res6 = ((rhs.transpose() * lhs.transpose()) - lhs1) + (lhs * rhs - lhs1);
			Mat res7 = big * big + big.transpose();
			Mat res8 = (big * big + big.transpose()) + (big * big + big.transpose());

			Mat res1v = { {2, 5, 8}, {6, 9, 12} };
			Mat res2v = { {9, 19, 29}, {12, 26, 40}, {15, 33, 51} };
			Mat res3v = { {288, 345, 402}, {396, 474, 552}, {504, 603, 702} };
			Mat res4v = { {8, 17, 26}, {8, 21, 34}, {8, 25, 42} };
			Mat res5v = { {16, 34, 52}, {16, 42, 68}, {16, 50, 84} };
			Mat res6v = { {16, 27, 38}, {23, 42, 61}, {30, 57, 84} };
			Mat res7v = { {48, 61, 64, 75, 16}, {67, 86, 91, 108, 23},
			{96, 122, 129, 154, 31}, {97, 123, 130, 155, 32}, {94, 119, 124, 149, 26} };
			Mat res8v = 2 * res7v;

			Assert::IsTrue(res1 == res1v, L"res1");
			Assert::IsTrue(res2 == res2v, L"res2");
			Assert::IsTrue(res3 == res3v, L"res3");
			Assert::IsTrue(res4 == res4v, L"res4");
			Assert::IsTrue(res5 == res5v, L"res5");
			Assert::IsTrue(res6 == res6v, L"res6");
			Assert::IsTrue(res7 == res7v, L"res7");
			Assert::IsTrue(res8 == res8v, L"res8");
		}

		TEST_METHOD(Mixed)
		{
			mixedImpl(lhs, lhs1, rhs, bigger);
			mixedImpl(lhsc, lhs1c, rhsc, biggerc);
		}

		template<class Mat>
		void equalityMathImpl(const Mat& lhs, const Mat& lhs1, const Mat& rhs, const Mat& big)
		{
			Mat res1 = lhs;
			res1 += res1;

			Mat res2 = rhs;
			res2 -= rhs + rhs;

			Mat res3 = big;
			res3 += big;

			Mat res1v = lhs * 2;
			Mat res2v = rhs * -1;
			Mat res3v = big * 2;

			Assert::IsTrue(res1 == res1v, L"res1");
			Assert::IsTrue(res2 == res2v, L"res2");
			Assert::IsTrue(res3 == res3v, L"res3");
		}

		TEST_METHOD(EqualityMath)
		{
			equalityMathImpl(lhs, lhs1, rhs, bigger);
			equalityMathImpl(lhsc, lhs1c, rhsc, biggerc);
		}
	};
}