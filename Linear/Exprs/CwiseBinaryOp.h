#pragma once
#include "ForwardDeclarations.h"
#include "ExprIterator.h"

template<class Op, class Lhs, class Rhs>
struct Traits<CwiseBinaryOp<Op, Lhs, Rhs>>
{
	using value_type = typename Lhs::value_type;
	using ExprType = Lhs;
	static constexpr bool MajorOrder = ExprType::MajorOrder;
};

template<class Op, class Lhs, class Rhs>
class CwiseBinaryOp : public MatrixBase<CwiseBinaryOp<Op, Lhs, Rhs>>
{
public:
	enum
	{
		MajorOrder = Traits<Lhs>::MajorOrder
	};

	using Base				= MatrixBase<CwiseBinaryOp<Op, Lhs, Rhs>>;
	using ThisType			= CwiseBinaryOp<Op, Lhs, Rhs>;
	using size_type			= typename Base::size_type;
	using value_type		= typename Traits<ThisType>::value_type;
	using Type				= typename Op::value_type;
	using LhsT				= typename RefSelector<Lhs>::type;
	using RhsT				= typename RefSelector<Rhs>::type;


private:

	LhsT	lhs;
	RhsT	rhs;
	Op		op;

public:

	CwiseBinaryOp(Op op, const Lhs& lhsa, const Rhs& rhsa) :
		op{ op },
		lhs{ lhsa },
		rhs{ rhsa }
	{}

	const Op& getOp() const { return op; }
	const Lhs& getLhs() const { return lhs; }
	const Rhs& getRhs() const { return rhs; }

	inline size_type rows()			const noexcept { return lhs.rows(); }
	inline size_type cols()			const noexcept { return rhs.cols(); }
	inline size_type resultRows()	const noexcept { return rows(); }
	inline size_type resultCols()	const noexcept { return cols(); }
};

// Different CwiseBinaryOp's
namespace impl
{
template<class Type>
class AddOp
{
public:
	using value_type = Type;

	AddOp() {}

	template<class Lit, class Rit>
	Type operator()(Lit lit, Rit rit) const
	{
		return lit + rit;
	}
};

template<class Type>
class SubOp
{
public:
	using value_type = Type;

	SubOp() {}

	template<class Lit, class Rit>
	Type operator()(Lit lit, Rit rit) const
	{
		return lit - rit;
	}
};

template<class Type>
class CwiseProductOp
{
public:
	using value_type = Type;

	CwiseProductOp() {}

	template<class Lit, class Rit>
	Type operator()(Lit lit, Rit rit) const
	{
		return lit * rit;
	}
};

template<class Type>
class CwiseQuotientOp
{
public:
	using value_type = Type;

	CwiseQuotientOp() {}

	template<class Lit, class Rit>
	Type operator()(Lit lit, Rit rit) const
	{
		return lit / rit;
	}
};

// A wrapper for constant expressions 
template<class Type, class Val, class Expr>
class Constant
{
	const Val&	constant;
	const Expr&	expr;
public:
	static constexpr bool IsExpr = true;

	Constant(const Val& constant, const Expr& expr) :
		constant{ constant }, 
		expr{ expr }
	{}

	struct const_iterator
	{
		using Ref		= const_iterator&;
		using size_type = impl::size_type;

		const Val& constant;

		const_iterator(const Val& constant) :
			constant{ constant }
		{}

		Ref operator++()			{ return *this; }
		Ref operator+=(size_type)	{ return *this; }
		Ref operator--()			{ return *this; }
		Ref operator-=(size_type)	{ return *this; }
		const Val& operator*() const noexcept { return constant; }
	};

	size_type rows() const noexcept { return expr.rows(); }
	size_type cols() const noexcept { return expr.cols(); }

	const_iterator begin() const noexcept { return const_iterator{ constant }; }
};

// Scalar Op's for CwiseBinaryOp
template<class Scalar, class Type>
class ScalarAddOp
{
public:
	using value_type = Type;

	template<class Lit, class Rit>
	Type operator()(Lit lit, Rit rit) const
	{
		return static_cast<Type>(*lit + *rit);
	}
};

template<class Scalar, class Type>
class ScalarSubOp
{
public:
	using value_type = Type;

	template<class Lit, class Rit>
	Type operator()(Lit lit, Rit rit) const
	{
		return static_cast<Type>(*lit - *rit);
	}
};

template<class Scalar, class Type>
class ScalarProductOp
{
public:
	using value_type = Type;

	template<class Lit, class Rit>
	Type operator()(Lit lit, Rit rit) const
	{
		return static_cast<Type>(*lit * *rit);
	}
};

template<class Scalar, class Type>
class ScalarQuotientOp
{
public:
	using value_type = Type;

	template<class Lit, class Rit>
	Type operator()(Lit lit, Rit rit) const
	{
		return static_cast<Type>(*lit / *rit);
	}
};

}
