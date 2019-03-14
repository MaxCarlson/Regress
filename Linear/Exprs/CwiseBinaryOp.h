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
	static constexpr bool MajorOrder = Traits<Lhs>::MajorOrder;


	using Base				= MatrixBase<CwiseBinaryOp<Op, Lhs, Rhs>>;
	using ThisType			= CwiseBinaryOp<Op, Lhs, Rhs>;
	using size_type			= typename Base::size_type;
	using value_type		= typename Traits<ThisType>::value_type;
	using Type				= typename Op::value_type;
	using Lit				= typename Lhs::const_iterator;
	using Rit				= typename Rhs::const_iterator;
	using const_iterator	= impl::ExprIterator<ThisType&, MajorOrder>;


private:
	using LhsT = typename RefSelector<Lhs>::type;
	using RhsT = typename RefSelector<Rhs>::type;

	LhsT	lhs;
	RhsT	rhs;
	Op		op;
	Lit		lit;
	Rit		rit;

public:


	CwiseBinaryOp(Op op, const Lhs& lhsa, const Rhs& rhsa) :
		op{ op },
		lhs{ lhsa },
		rhs{ rhsa },
		lit{ lhs.begin() },
		rit{ rhs.begin() }
	{}

	inline size_type lhsRows()		const noexcept { return lhs.rows(); }
	inline size_type rhsRows()		const noexcept { return rhs.rows(); }
	inline size_type rhsCols()		const noexcept { return rhs.cols(); }
	inline size_type rows()			const noexcept { return lhsRows(); }
	inline size_type cols()			const noexcept { return rhsCols(); }
	inline size_type resultRows()	const noexcept { return lhsRows(); }
	inline size_type resultCols()	const noexcept { return rhsCols(); }

	inline void lhsInc(size_type i)  noexcept { lit += i; }
	inline void lhsDec(size_type i)  noexcept { lit -= i; }
	inline void rhsInc(size_type i)  noexcept { rit += i; }
	inline void rhsDec(size_type i)  noexcept { rit -= i; }

	Type evaluate() const noexcept
	{
		return op(lit, rit);
	}

	ThisType& operator++() noexcept
	{
		lhsInc(1);
		rhsInc(1);
		return *this;
	}

	ThisType& operator+=(size_type i) noexcept
	{
		lhsInc(i);
		rhsInc(i);
		return *this;
	}

	ThisType& operator--() noexcept
	{
		lhsDec(1);
		rhsDec(1);
		return *this;
	}

	ThisType& operator-=(size_type i) noexcept
	{
		lhsDec(i);
		rhsDec(i);
		return *this;
	}

	const_iterator begin() noexcept { return const_iterator{ this }; }
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
		return *lit + *rit;
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
		return *lit - *rit;
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
		return *lit * *rit;
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
		return *lit / *rit;
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
