#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Linear/Matrix.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test
{
TEST_CLASS(MatrixTest)
{
public:

	using rMM		= Matrix<int, false>;
	using cMM		= Matrix<int, true>;

	rMM lhsR = {
		{1, 2},
		{3, 4},
		{5, 6},
	};

	rMM rhsR = {
		{1, 2, 3},
		{4, 5, 6}
	};

	cMM lhsC = {
		{1, 2},
		{3, 4},
		{5, 6},
	};

	cMM rhsC = {
		{1, 2, 3},
		{4, 5, 6}
	};

	template<class T, bool O>
	void indexingImpl(Matrix<T, O>& lhs, Matrix<T, O>& rhs)
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

	TEST_METHOD(Indexing)
	{
		indexingImpl(lhsR, rhsR);
		indexingImpl(lhsC, rhsC);
	}

	template<class T, bool O>
	void iteratorsImpl(Matrix<T, O>& lhs, Matrix<T, O>& rhs)
	{
		using Mat	= Matrix<T, O>;
		using It	= typename Mat::iterator;
		using cIt	= typename Mat::col_iterator;
		using rIt	= typename Mat::row_iterator;

		auto test = [&](const auto& ans, auto s, auto e)
		{
			auto aIt = std::begin(ans);
			for (auto sv = s; sv != e; ++sv, ++aIt)
				Assert::IsTrue(*sv == *aIt);

			--aIt;
			for(auto sv = e - 1; sv > s; --sv, --aIt)
				Assert::IsTrue(*sv == *aIt);
		};

		// Test row iterators
		std::vector<int> co = { 1, 2, 3, 4, 5, 6 };
		test(co, lhs.row_begin(), lhs.row_end());
		test(co, rhs.row_begin(), rhs.row_end());

		// And column iterators
		co = { 1, 3, 5, 2, 4, 6 };
		test(co, lhs.col_begin(), lhs.col_end());
		co = { 1, 4, 2, 5, 3, 6 };
		test(co, rhs.col_begin(), rhs.col_end());

	}

	// TODO: For this test to compile both row and col iterators
	// msut take the same init args. Fix tomorrow!
	TEST_METHOD(Iterators)
	{
		iteratorsImpl(lhsR, rhsR);
		iteratorsImpl(lhsC, rhsC);
	}

	template<class T, bool O>
	void addColumnImpl(Matrix<T, O>& lhs, Matrix<T, O>& rhs)
	{
		using Mat = Matrix<T, O>;

		auto v1 = lhs;
		v1.addColumn(v1.cols(), 3); // TODO: Issue here
		Mat v1r = { {1, 2, 3}, {3, 4, 3}, {5, 6, 3} };

		auto v2 = rhs;
		v2.addColumn(0);
		Mat v2r = { {0, 1, 2, 3}, {0, 4, 5, 6} };

	
		Assert::IsTrue(v1 == v1r);
		Assert::IsTrue(v2 == v2r);
	}

	TEST_METHOD(AddColumn)
	{
		addColumnImpl(lhsR, rhsR);
		addColumnImpl(lhsC, rhsC);
	}
};

}