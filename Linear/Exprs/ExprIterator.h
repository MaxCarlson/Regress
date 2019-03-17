#pragma once
#include "ForwardDeclarations.h"

namespace impl
{
template<class MatrixExpr, bool MajorOrder>
class ExprIterator
{
	MatrixExpr expr;

public:
	using MatrixExprNoRef	= std::remove_reference_t<MatrixExpr>;
	using Type				= typename MatrixExprNoRef::value_type;

	static constexpr bool MajorOrder = MajorOrder;

	ExprIterator(MatrixExprNoRef* expr) noexcept :
		expr{ *expr }
	{}
	ExprIterator(const ExprIterator& other) noexcept :
		expr{ other.expr }
	{}

	MatrixExpr& getCont() noexcept { return expr; }

	inline bool operator==(const ExprIterator& other) const noexcept
	{
		return &expr == &other.expr;
	}

	inline bool operator!=(const ExprIterator& other) const noexcept
	{
		return !(*this == other);
	}

	inline Type operator*() const
	{
		return expr.evaluate();
	}

	inline ExprIterator& operator++() noexcept
	{
		++expr;
		return *this;
	}

	inline ExprIterator& operator+=(size_type i) noexcept
	{
		expr += i;
		return *this;
	}

	inline ExprIterator& operator--() noexcept
	{
		--expr;
		return *this;
	}

	inline ExprIterator& operator-=(size_type i) noexcept
	{
		expr -= i;
		return *this;
	}
};
}
