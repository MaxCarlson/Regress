#pragma once
#include "ForwardDeclarations.h"

template<class Type, bool MOrder>
class ExprAnalyzer;

template<class Type>
class MulOp
{
public:
	using value_type = Type;

	MulOp(Type) {}

	template<class Lit, class Rit>
	Type operator()(Lit lit, Rit rit, int lhsRows, int rhsRows, int rhsCols) const
	{
		Type result = 0;
		for (;; --rhsRows)
		{
			result += *lit * *rit;
			if (rhsRows <= 1)
				break;

			if (Lit::MajorOrder)
			{
				lit += lhsRows;
				++rit;
			}
			else
			{
				++lit;
				rit += rhsCols;
			}
		}

		return result;
	}
};

template<class Op, class Lhs, class Rhs>
class BinaryOp;

template<class Op, class Lhs, class Rhs>
struct Traits<BinaryOp<Op, Lhs, Rhs>>
{
	using value_type = typename Lhs::value_type;
	using MatrixType = Lhs;
	static constexpr bool MajorOrder = MatrixType::MajorOrder;
};

template<class Op, class Lhs, class Rhs>
class BinaryOp : public MatrixBase<BinaryOp<Op, Lhs, Rhs>>
{
public:
	using Base		= MatrixBase<BinaryOp<Op, Lhs, Rhs>>;
	using ThisType	= BinaryOp<Op, Lhs, Rhs>;
	using size_type = typename Base::size_type;
	using Type		= typename Op::value_type;
	using Lit		= typename Lhs::const_iterator;
	using Rit		= typename Rhs::const_iterator;


private:
	const Lhs& lhs;
	const Rhs& rhs;
	Op op;
	Lit lit;
	Rit rit;

public:

	static constexpr bool MajorOrder = Traits<Lhs>::MajorOrder;

	BinaryOp(Op op, const Lhs& lhs, const Rhs& rhs) :
		op{ op },
		lhs{ lhs },
		rhs{ rhs },
		lit{ lhs.cbegin() },
		rit{ rhs.cbegin() }
	{
	}

	inline size_type lhsRows()	const noexcept { return lhs.rows(); }
	inline size_type rhsRows()	const noexcept { return rhs.rows(); }
	inline size_type rhsCols()	const noexcept { return rhs.cols(); }
	inline size_type rows() const noexcept { return lhsRows(); }
	inline size_type cols() const noexcept { return rhsCols(); }
	inline size_type resultRows() const noexcept { return lhsRows(); }
	inline size_type resultCols() const noexcept { return rhsCols(); }

	inline void lhsInc(size_type i)  noexcept { lit += i; }
	inline void lhsDec(size_type i)  noexcept { lit -= i; }
	inline void rhsInc(size_type i)  noexcept { rit += i; }
	inline void rhsDec(size_type i)  noexcept { rit -= i; }

	Type evaluate() const noexcept
	{
		return op(lit, rit, lhs.rows(), rhs.rows(), rhs.cols());
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

	class const_iterator
	{
		using MatrixExpr = ThisType;
		MatrixExpr expr;

	public:
		static constexpr bool MajorOrder = MajorOrder;

		const_iterator(const MatrixExpr* expr) noexcept :
			expr{*expr}
		{}

		inline bool operator==(const const_iterator& other) const noexcept
		{
			return &expr == &other.expr;
		}

		inline bool operator!=(const const_iterator& other) const noexcept
		{
			return !(*this == other);
		}

		inline Type operator*() const noexcept
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

	const_iterator cbegin() const noexcept { return const_iterator{ this }; }
};

template<class Derived>
class MatrixBase : public BaseExpr<Derived>
{
public:
	using Base			= BaseExpr<Derived>;
	using ThisType		= MatrixBase<Derived>;
	using value_type	= typename Traits<Derived>::value_type;
	using size_type		= typename Base::size_type;
	static constexpr bool MajorOrder = Traits<Derived>::MajorOrder;


	template<class Matrix>
	void assign(Matrix& to)
	{
		auto rit = static_cast<const Derived&>(*this);
		to.resize(rit.resultRows(), rit.resultCols());

		for (auto lit = to.begin(); lit != to.end(); ++lit, ++rit)
			*lit = rit.evaluate();
	}

	template<class OtherDerived>
	const BinaryOp<MulOp<value_type>, Derived, OtherDerived>
		operator*(const MatrixBase<OtherDerived>& other) const
	{
		return BinaryOp{ MulOp<value_type>{0}, 
			static_cast<const Derived&>(*this), 
			static_cast<const OtherDerived&>(other) };
	}

};