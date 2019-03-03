#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Regress/Linear/Matrix.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test
{		
	TEST_CLASS(Expressions)
	{
	public:

		Matrix<int> lhs = {
			{1, 2},
			{3, 4},
			{5, 6},
		};

		Matrix<int> rhs = {
			{1, 2, 3},
			{4, 5, 6}
		};
		
		TEST_METHOD(Mul)
		{
			
		}

	};
}