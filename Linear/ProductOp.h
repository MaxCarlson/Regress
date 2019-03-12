#pragma once
#include "ForwardDeclarations.h"

template<class Lhs, class Rhs>
struct Traits<ProductOp<Lhs, Rhs>>
{
	using value_type = typename Lhs::value_type;
	using MatrixType = Lhs;
	static constexpr bool MajorOrder = MatrixType::MajorOrder;
};

template<class Lhs, class Rhs>
class ProductOp : public MatrixBase<ProductOp<Lhs, Rhs>>
{
public:
	using Base		= MatrixBase<ProductOp<Lhs, Rhs>>;
	using ThisType	= ProductOp<Lhs, Rhs>;
	using size_type = typename Base::size_type;
	using Type		= typename Traits<ThisType>::value_type;
	using Lit		= typename Lhs::const_iterator;
	using Rit		= typename Rhs::const_iterator;


private:
	using LhsT = typename RefSelector<Lhs>::type;
	using RhsT = typename RefSelector<Rhs>::type;

	LhsT	lhs;
	RhsT	rhs;
	Lit		lit;
	Rit		rit;
	size_type multiCount;
public:

	static constexpr bool MajorOrder = Traits<Lhs>::MajorOrder;

	ProductOp(const Lhs& lhsa, const Rhs& rhsa) :
		lhs{ lhsa },
		rhs{ rhsa },
		lit{ lhs.begin() },
		rit{ rhs.begin() },
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
		return Type{};
	}

	ThisType& operator++()
	{
		return *this;
	}

	ThisType& operator+=(size_type i)
	{
		return *this;
	}

	ThisType& operator--()
	{
		return *this;
	}

	ThisType& operator-=(size_type i)
	{
		return *this;
	}
};