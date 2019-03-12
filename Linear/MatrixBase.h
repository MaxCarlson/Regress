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

template<class Type>
class AddOp
{
public:
	using value_type = Type;

	AddOp(Type) {}

	template<class Lit, class Rit>
	Type operator()(Lit lit, Rit rit) const
	{
		return *lit + *rit;
	}
};

template<class Derived>
class MatrixBase : public BaseExpr<Derived>
{
public:
	using Base			= BaseExpr<Derived>;
	using ThisType		= MatrixBase<Derived>;
	using value_type	= typename Traits<Derived>::value_type;
	using size_type		= typename Base::size_type;
	static constexpr bool MajorOrder	= Traits<Derived>::MajorOrder;
	static constexpr bool IsExpr		= true; // We will override this in anything not an expression


	template<class Matrix>
	void assign(Matrix& to)
	{
		auto rit = static_cast<const Derived&>(*this);
		to.resize(rit.resultRows(), rit.resultCols());

		for (auto lit = to.begin(); lit != to.end(); ++lit, ++rit)
			*lit = rit.evaluate();
	}

	template<class OtherDerived>
	const CwiseBinaryOp<AddOp<value_type>, Derived, OtherDerived>
		operator+(const MatrixBase<OtherDerived>& other) const
	{
		return CwiseBinaryOp{ AddOp<value_type>{0}, 
			static_cast<const Derived&>(*this), 
			static_cast<const OtherDerived&>(other) };
	}

	template<class OtherDerived>
	const ProductOp<Derived, OtherDerived>
		operator*(const MatrixBase<OtherDerived>& other) const
	{
		return ProductOp{ 
			static_cast<const Derived&>(*this),
			static_cast<const OtherDerived&>(other) };
	}
};