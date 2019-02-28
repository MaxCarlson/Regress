#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Regress/Linear/ExprOperators.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test
{		
	TEST_CLASS(Expressions)
	{
	public:

		MatrixT<int> lhs = {
			{1, 2},
			{3, 4},
			{5, 6},
		};

		MatrixT<int> rhs = {
			{1, 2, 3},
			{4, 5, 6}
		};
		
		TEST_METHOD(Mul)
		{
			
		}

	};
}