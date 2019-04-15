#include "stdafx.h"
#include "CppUnitTest.h"
#include "Matrix.h"

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

		template<class Dest, class Lhs, class Rhs>
		void testLargeImpl(Dest& dest, const Lhs& lhs, const Rhs& rhs)
		{
			using EvaluatorD = impl::Evaluator<Dest>;
			using EvaluatorL = impl::Evaluator<Lhs>;
			using EvaluatorR = impl::Evaluator<Rhs>;
			using LoopCoeff	 = impl::ProductLoop<EvaluatorD, EvaluatorL, EvaluatorR, impl::GEMMType::COEFF>;
			using LoopVector = impl::ProductLoop<EvaluatorD, EvaluatorL, EvaluatorR, impl::GEMMType::VECTORIZED>;

			EvaluatorL lhsEval{ lhs };
			EvaluatorR rhsEval{ rhs };

			dest.resize(lhs.rows(), rhs.cols());
			Dest destCopy = dest;
			
			EvaluatorD destEval{ dest };
			EvaluatorD destCEval{ destCopy };

			// Multiply matrix with slow but proven method
			LoopCoeff::run(destEval, lhsEval, rhsEval);

			// Multiply matrix with vectorization enabled
			LoopVector::run(destCEval, lhsEval, rhsEval);

			// Test equality of results
			Assert::IsTrue(dest == destCopy);
		}

		template<class Dest, class Lhs, class Rhs, class Vec>
		void generateTestMatrixes(const Vec& lhsVec, const Vec& rhsVec, int lrows, int lcols, int rrows, int rcols)
		{
			Dest dest;
			Lhs lhs{ lrows, lcols };
			Rhs rhs{ rrows, rcols };

			// TODO: make this faster, we don't really even need to fill the entire matrix
			std::copy(lhsVec.begin(), lhsVec.end(), lhs.begin());
			std::copy(rhsVec.begin(), rhsVec.end(), rhs.begin());

			testLargeImpl(dest, lhs, rhs);
		}

		template<class Type>
		void matrixMajorOrderPermutations(int lrows, int lcols, int rrows, int rcols)
		{
			using MatR = Matrix<Type, RowMajor>;
			using MatC = Matrix<Type, ColMajor>;

			std::vector<Type> lhsV(lrows * lcols);
			std::vector<Type> rhsV(lrows * lcols);
			std::generate(lhsV.begin(), lhsV.end(), std::rand);
			std::generate(rhsV.begin(), rhsV.end(), std::rand);

			generateTestMatrixes<MatR, MatR, MatR>(lhsV, rhsV, lrows, lcols, rrows, rcols);
			generateTestMatrixes<MatR, MatR, MatC>(lhsV, rhsV, lrows, lcols, rrows, rcols);
			generateTestMatrixes<MatR, MatC, MatR>(lhsV, rhsV, lrows, lcols, rrows, rcols);
			generateTestMatrixes<MatR, MatC, MatC>(lhsV, rhsV, lrows, lcols, rrows, rcols);

			generateTestMatrixes<MatC, MatC, MatC>(lhsV, rhsV, lrows, lcols, rrows, rcols);
			generateTestMatrixes<MatC, MatC, MatR>(lhsV, rhsV, lrows, lcols, rrows, rcols);
			generateTestMatrixes<MatC, MatR, MatC>(lhsV, rhsV, lrows, lcols, rrows, rcols);
			generateTestMatrixes<MatC, MatR, MatR>(lhsV, rhsV, lrows, lcols, rrows, rcols);
		}

		void generateTestMatrixTypes(int lrows, int lcols, int rrows, int rcols)
		{
			matrixMajorOrderPermutations<int>(lrows, lcols, rrows, rcols);
			matrixMajorOrderPermutations<float>(lrows, lcols, rrows, rcols);
			matrixMajorOrderPermutations<double>(lrows, lcols, rrows, rcols);
		}

		TEST_METHOD(TestLarge)
		{
			generateTestMatrixTypes(10,		10,		10,	10);
			generateTestMatrixTypes(10,		5,		5,	15);
			generateTestMatrixTypes(23,		13,		13, 42);
			generateTestMatrixTypes(3,		1,		1,	13);

		}
	};
}