#pragma once
#include "ForwardDeclarations.h"
#include "ExprIterator.h"

template<class Lhs, class Rhs>
struct Traits<ProductOp<Lhs, Rhs>>
{
	using value_type = typename Lhs::value_type;
	using ExprType = Lhs;
	static constexpr bool MajorOrder = ExprType::MajorOrder;
};

//     +---+---+
//     | A | B |
//     +---+---+
// n = | C | D |
//     +---+---+
//     | E | F |
//     +---+---+
//
//     +---+---+---+---+
//     | Z | Y | X | W |
// m = +---+---+---+---+
//     | V | U | T | S |
//     +---+---+---+---+
//
//       +---------------+---------------+---------------+---------------+
//       | (A*Z) + (B*V) | (A*Y) + (B*U) | (A*X) + (B*T) | (A*W) + (B*S) |
//       +---------------+---------------+---------------+---------------+
// n*m = | (C*Z) + (D*V) | (C*Y) + (D*U) | (C*X) + (D*T) | (C*W) + (D*S) |
//       +---------------+---------------+---------------+---------------+
//       | (E*Z) + (F*V) | (E*Y) + (F*U) | (E*X) + (F*T) | (E*W) + (F*S) |
//       +---------------+---------------+---------------+---------------+
//
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

template<class Lhs, class Rhs>
class ProductOp : public MatrixBase<ProductOp<Lhs, Rhs>>
{
public:
	static constexpr bool MajorOrder = Traits<Lhs>::MajorOrder;

	using Base				= MatrixBase<ProductOp<Lhs, Rhs>>;
	using ThisType			= ProductOp<Lhs, Rhs>;
	using size_type			= typename Base::size_type;
	using Type				= typename Traits<ThisType>::value_type;
	using Lit				= typename Lhs::const_iterator;
	using Rit				= typename Rhs::const_iterator;

	// Note: This expr's iterator takes ThisType by VALUE 
	// (I think we can do without this eventually, and should)
	using const_iterator	= impl::ExprIterator<ThisType, MajorOrder>;


private:
	using LhsT = typename RefSelector<Lhs>::type;
	using RhsT = typename RefSelector<Rhs>::type;

	LhsT		lhs;
	RhsT		rhs;
	Lit			lit;
	Rit			rit;
	MulOp<Type>	op;
	size_type	multiCount;
public:


	ProductOp(const Lhs& lhsa, const Rhs& rhsa) :
		lhs{ lhsa },
		rhs{ rhsa },
		lit{ lhs.begin() },
		rit{ rhs.begin() },
		op{ 0 },
		multiCount{ 0 }
	{}

	inline size_type lhsRows()	const noexcept { return lhs.rows(); }
	inline size_type rhsRows()	const noexcept { return rhs.rows(); }
	inline size_type rhsCols()	const noexcept { return rhs.cols(); }
	inline size_type rows()		const noexcept { return lhsRows(); }
	inline size_type cols()		const noexcept { return rhsCols(); }
	inline size_type resultRows() const noexcept { return lhsRows(); }
	inline size_type resultCols() const noexcept { return rhsCols(); }

	inline void lhsInc(size_type i)  noexcept { lit += i; }
	inline void lhsDec(size_type i)  noexcept { lit -= i; }
	inline void rhsInc(size_type i)  noexcept { rit += i; }
	inline void rhsDec(size_type i)  noexcept { rit -= i; }

	Type evaluate() const noexcept
	{
		return op(lit, rit, lhsRows(), rhsRows(), rhsCols());
	}

private:

	void incrementSelf(size_type i)
	{
		if (MajorOrder)
			lhsInc(i);
		else
			rhsInc(i);

		size_type idx			= std::abs(i);
		const size_type sign	= (0 < i) - (i < 0);
		const size_type modVal	= MajorOrder ? lhsRows() : rhsCols();

		while (idx--)
		{
			multiCount += sign;
			if (multiCount % modVal == 0)
			{
				if (MajorOrder)
				{
					lhsDec(lhsRows() * sign);
					rhsInc(rhsRows() * sign);
				}
				else
				{
					rhsDec(rhsCols() * sign);
					lhsInc(rhsRows() * sign); // Same as lhsCols
				}
				break;
			}
		}
	}

public:
	ThisType& operator++()
	{
		incrementSelf(1);
		return *this;
	}

	ThisType& operator+=(size_type i)
	{
		incrementSelf(i);
		return *this;
	}

	ThisType& operator--()
	{
		incrementSelf(-1);
		return *this;
	}

	ThisType& operator-=(size_type i)
	{
		incrementSelf(-i);
		return *this;
	}

	const_iterator begin() noexcept { return const_iterator{ this }; }
};