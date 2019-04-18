#include "stdafx.h"
#include "CppUnitTest.h"
#include "Matrix.h"
#include <numeric>
#include <sstream>
/*

This file containts white-box tests for matrix multiplication
specifically testing more complex cases than in Expressions.cpp

*/

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test
{
	TEST_CLASS(MatrixMul)
	{
	public:

		template<class Dest, class Lhs, class Rhs, class Vec>
		void testLargeImpl(const Vec& lhsVec, const Vec& rhsVec, int m, int k, int n)
		{
			using value_type = typename Dest::value_type;
			using EvaluatorD = impl::Evaluator<Dest>;
			using EvaluatorL = impl::Evaluator<Lhs>;
			using EvaluatorR = impl::Evaluator<Rhs>;
			using LoopCoeff	 = impl::ProductLoop<EvaluatorD, EvaluatorL, EvaluatorR, impl::GEMMType::COEFF>;
			using LoopVector = impl::ProductLoop<EvaluatorD, EvaluatorL, EvaluatorR, impl::GEMMType::VECTORIZED>;

			Lhs lhs{ m, k };
			Rhs rhs{ k, n };
			Dest dest(lhs.rows(), rhs.cols());
			Dest destCopy = dest;

			// TODO: make this faster, we don't really even need to fill the entire matrix
			std::copy(lhsVec.begin(), lhsVec.end(), lhs.begin());
			std::copy(rhsVec.begin(), rhsVec.end(), rhs.begin());

			EvaluatorL lhsEval{ lhs };
			EvaluatorR rhsEval{ rhs };
			EvaluatorD destEval{ dest };
			EvaluatorD destCEval{ destCopy };

			// Multiply matrix with slow but proven method
			LoopCoeff::run(destEval, lhsEval, rhsEval);

			// Multiply matrix with vectorization enabled
			LoopVector::run(destCEval, lhsEval, rhsEval);

			std::stringstream ss;
			ss << typeid(value_type).name() << ' ';
			ss << m << 'x' << k << ", " << k << 'x' << n;
			std::string str = ss.str();
			std::wstring failStr{ str.begin(), str.end() };

			// TODO: Need to calculate the total amount of acceptable error (based on matrix sizes)
			// and use that as epsilon
			// Test equality of results
			if (std::is_floating_point_v<typename Dest::value_type>)
			{
				static constexpr double epsilon = 5.0;
				auto it1 = dest.begin();
				auto it2 = destCopy.begin();
				for (; it1 < dest.end(); ++it1, ++it2)
				{
					auto diff = *it1 - *it2;
					Assert::IsTrue(diff < epsilon, failStr.c_str());
				}
			}
			else
				Assert::IsTrue(dest == destCopy, failStr.c_str());
		}

		// Test matrix multiplications between matrixes of all 
		// storage order combinations. 
		// E.g. RowMajor = RowMajor * RowMajor, 
		// ColMajor = RowMajor * ColMajor, etc.
		template<class Type>
		void matrixMajorOrderPermutations(int m, int k, int n)
		{
			using MatR = Matrix<Type, RowMajor>;
			using MatC = Matrix<Type, ColMajor>;

			static constexpr int MAX_VAL = 200;
			std::vector<Type> lhsV(m * k);
			std::vector<Type> rhsV(n * k);

			// We need small numbers so the float values don't become wildly off
			int startLhsV = rand() % MAX_VAL;
			int startRhsV = rand() % MAX_VAL;
			auto tformLhs = [&](auto v) {return startLhsV > MAX_VAL ? startLhsV = 0 : startLhsV % MAX_VAL; };
			auto tformRhs = [&](auto v) {return startRhsV > MAX_VAL ? startRhsV = 0 : startRhsV % MAX_VAL; };

			std::transform(lhsV.begin(), lhsV.end(), lhsV.begin(), tformLhs);
			std::transform(rhsV.begin(), rhsV.end(), rhsV.begin(), tformRhs);

			testLargeImpl<MatR, MatR, MatR>(lhsV, rhsV, m, k, n);
			testLargeImpl<MatR, MatR, MatC>(lhsV, rhsV, m, k, n);
			testLargeImpl<MatR, MatC, MatR>(lhsV, rhsV, m, k, n);
			testLargeImpl<MatR, MatC, MatC>(lhsV, rhsV, m, k, n);

			testLargeImpl<MatC, MatC, MatC>(lhsV, rhsV, m, k, n);
			testLargeImpl<MatC, MatC, MatR>(lhsV, rhsV, m, k, n);
			testLargeImpl<MatC, MatR, MatC>(lhsV, rhsV, m, k, n);
			testLargeImpl<MatC, MatR, MatR>(lhsV, rhsV, m, k, n);
		}

		void generateTestMatrixTypes(int m, int k, int n)
		{
			matrixMajorOrderPermutations<int>(m, k, n);
			matrixMajorOrderPermutations<float>(m, k, n);
			matrixMajorOrderPermutations<double>(m, k, n);
		}

		// Test a matrix multiplication between many
		// different MxK * KxN matrixes
		TEST_METHOD(TestLarge)
		{
			static constexpr auto NUM_TESTS = 33;
			static constexpr int minDim = 1;
			static constexpr int maxDim = 45;

			//*
			for (int i = 0; i < NUM_TESTS; ++i)
			{
				int m = std::rand() % (maxDim - minDim) + minDim;
				int k = std::rand() % (maxDim - minDim) + minDim;
				int n = std::rand() % (maxDim - minDim) + minDim;

				generateTestMatrixTypes(m, k, n);
			}
			//*/
			generateTestMatrixTypes(2,		69,		2);
			generateTestMatrixTypes(55,		1300,	1);
			generateTestMatrixTypes(42,		18,		35);
			generateTestMatrixTypes(1024,	257,	512);
			generateTestMatrixTypes(200,	5,		2048);
			generateTestMatrixTypes(230,	999,	420); // TODO: Why does this crash it? Large k?
			generateTestMatrixTypes(1500,	111,	1393);

			

		}
	};
}