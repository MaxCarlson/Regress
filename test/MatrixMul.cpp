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

		template<class Dest, class Lhs, class Rhs>
		void generateTestMatrixes(int lrows, int lcols, int rrows, int rcols)
		{
			Dest dest;
			Lhs lhs{ lrows, lcols };
			Rhs rhs{ rrows, rcols };

			std::generate(lhs.begin(), lhs.end(), std::rand);
			std::generate(rhs.begin(), rhs.end(), std::rand);

			testLargeImpl(dest, lhs, rhs);
		}

		TEST_METHOD(TestLarge)
		{
			using Mati = Matrix<int>;
			generateTestMatrixes<Mati, Mati, Mati>(25, 25, 25, 5);
		}
	};
}