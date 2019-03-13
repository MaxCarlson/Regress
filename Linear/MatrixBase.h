#pragma once
#include "ForwardDeclarations.h"

template<class Type, bool MOrder>
class ExprAnalyzer;

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
	const CwiseBinaryOp<impl::AddOp<value_type>, Derived, OtherDerived>
		operator+(const MatrixBase<OtherDerived>& other) const
	{
		return CwiseBinaryOp{ impl::AddOp<value_type>{},
			static_cast<const Derived&>(*this), 
			static_cast<const OtherDerived&>(other) };
	}

	template<class OtherDerived>
	const CwiseBinaryOp<impl::SubOp<value_type>, Derived, OtherDerived>
		operator-(const MatrixBase<OtherDerived>& other) const
	{
		return CwiseBinaryOp{ impl::SubOp<value_type>{},
			static_cast<const Derived&>(*this),
			static_cast<const OtherDerived&>(other) };
	}

	template<class OtherDerived>
	const CwiseBinaryOp<impl::CwiseQuotientOp<value_type>, Derived, OtherDerived>
		operator/(const MatrixBase<OtherDerived>& other) const
	{
		return CwiseBinaryOp{ impl::CwiseQuotientOp<value_type>{},
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

	template<class Scalar>
	const CwiseBinaryOp<impl::ScalarProductOp<Scalar, value_type>, Derived, impl::Constant<value_type>>
		operator*(const Scalar& scalar) const
	{
		return CwiseBinaryOp{ impl::ScalarProductOp<Scalar, value_type>{scalar},
			static_cast<const Derived&>(*this),
			impl::Constant<value_type>{} };
	}
};