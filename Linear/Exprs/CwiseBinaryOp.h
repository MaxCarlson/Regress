#pragma once
#include "ForwardDeclarations.h"
#include "ExprIterator.h"
#include <xmmintrin.h>

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

	template<class Lhs, class Rhs>
	Type operator()(const Lhs& lit, const Rhs& rit) const
	{
		return lit + rit;
	}

	template<class Packet>
	Packet packetOp(const Packet& p1, const Packet& p2) const
	{
		return padd(p1, p2);
	}
};

template<class Type>
class SubOp
{
public:
	using value_type = Type;

	template<class Lhs, class Rhs>
	Type operator()(const Lhs& lit, const Rhs& rit) const
	{
		return lit - rit;
	}
};

template<class Type>
class MulOp
{
public:
	using value_type = Type;

	template<class Lhs, class Rhs>
	Type operator()(const Lhs& lit, const Rhs& rit) const
	{
		return lit * rit;
	}
};

template<class Type>
class DivOp
{
public:
	using value_type = Type;

	template<class Lhs, class Rhs>
	Type operator()(const Lhs& lit, const Rhs& rit) const
	{
		return lit / rit;
	}
};

template<class Type>
class CwiseProductOp
{
public:
	using value_type = Type;

	CwiseProductOp() {}

	template<class Lhs, class Rhs>
	Type operator()(const Lhs& lit, const Rhs& rit) const
	{
		return lit * rit;
	}
};

// Simple wrapper so we can perform 
// Matrix m; m operator(x) constant 
// (e.g.) m + 2; m / 2; etc
template<class Type, class Expr>
struct Constant
{
	using size_type = typename Expr::size_type;

	Constant(const Type& t, const Expr& expr) :
		t{ t },
		expr{ expr }
	{}

	enum
	{
		IsExpr = true
	};

	inline size_type rows()	const noexcept { return expr.rows(); }
	inline size_type cols()	const noexcept { return expr.cols(); }

	const Type& getValue() const { return t; }

private:
	const Type& t;
	const Expr& expr;
};

} // End impl::
