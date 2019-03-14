#pragma once
#include "ForwardDeclarations.h"

template<class Op, class Lhs, class Rhs>
struct Traits<CwiseBinaryOp<Op, Lhs, Rhs>>
{
	using value_type = typename Lhs::value_type;
	using MatrixType = Lhs;
	static constexpr bool MajorOrder = MatrixType::MajorOrder;
};

template<class Op, class Lhs, class Rhs>
class CwiseBinaryOp : public MatrixBase<CwiseBinaryOp<Op, Lhs, Rhs>>
{
public:
	using Base		= MatrixBase<CwiseBinaryOp<Op, Lhs, Rhs>>;
	using ThisType	= CwiseBinaryOp<Op, Lhs, Rhs>;
	using size_type = typename Base::size_type;
	using Type		= typename Op::value_type;
	using Lit		= typename Lhs::const_iterator;
	using Rit		= typename Rhs::const_iterator;


private:
	using LhsT = typename RefSelector<Lhs>::type;
	using RhsT = typename RefSelector<Rhs>::type;

	LhsT	lhs;
	RhsT	rhs;
	Op		op;
	Lit		lit;
	Rit		rit;

public:

	static constexpr bool MajorOrder = Traits<Lhs>::MajorOrder;

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

	// This iterator is const only in the sense that we're not modifying
	// and matricies with it. The expressions do get modified, but we need to match naming convension
	// with the Matrix class. TODO: figure out how to do this more cleanly
	class const_iterator
	{
		using MatrixExpr = ThisType;
		MatrixExpr& expr;

	public:
		static constexpr bool MajorOrder = MajorOrder;

		const_iterator(MatrixExpr* expr) noexcept :
			expr{ *expr }
		{}

		MatrixExpr& getCont() noexcept { return expr; }

		inline bool operator==(const const_iterator& other) const noexcept
		{
			return &expr == &other.expr;
		}

		inline bool operator!=(const const_iterator& other) const noexcept
		{
			return !(*this == other);
		}

		inline Type operator*() const
		{
			return expr.evaluate();
		}

		inline const_iterator& operator++() noexcept
		{
			++expr;
			return *this;
		}

		inline const_iterator& operator+=(size_type i) noexcept
		{
			expr += i;
			return *this;
		}
	};

	const_iterator begin() noexcept { return const_iterator{ this }; }
};

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
