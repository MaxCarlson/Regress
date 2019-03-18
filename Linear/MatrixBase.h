#pragma once
#include "ForwardDeclarations.h"

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
												// (TODO: handle in traits?)

	Derived& derived()				{ return static_cast<Derived&>(*this); }
	const Derived& derived() const	{ return static_cast<const Derived&>(*this); }


	template<class Matrix>
	void assign(Matrix& to)
	{
		/*
		auto rit	= derived().begin();
		to.resize(derived().resultRows(), derived().resultCols());
		for (auto lit = to.begin(); lit != to.end(); ++lit, ++rit)
			*lit = *rit;
		*/
	}

	void analyze() {}

	// Matrix Operators

	template<class OtherDerived>
	const CwiseBinaryOp<impl::AddOp<value_type>, Derived, OtherDerived>
		operator+(const MatrixBase<OtherDerived>& other) const
	{
		return CwiseBinaryOp{ impl::AddOp<value_type>{}, derived(), other.derived() };
	}

	template<class OtherDerived>
	const CwiseBinaryOp<impl::SubOp<value_type>, Derived, OtherDerived>
		operator-(const MatrixBase<OtherDerived>& other) const
	{
		return CwiseBinaryOp{ impl::SubOp<value_type>{}, derived(), other.derived() };
	}

	template<class OtherDerived>
	const ProductOp<Derived, OtherDerived>
		operator*(const MatrixBase<OtherDerived>& other) const
	{
		return ProductOp{ derived(), other.derived() };
	}


	// Scalar Operators 
	// (enabled on objects that aren't derived from MatrixBase)

	template<class Scalar,
		class = std::enable_if_t<!std::is_base_of_v<MatrixBase<Scalar>, Scalar>>>
		const CwiseBinaryOp<impl::ScalarAddOp<Scalar, value_type>,
		Derived,
		impl::Constant<value_type, Scalar, Derived>>
		operator+(const Scalar& scalar) const
	{
		return CwiseBinaryOp{ impl::ScalarAddOp<Scalar, value_type>{}, derived(),
			impl::Constant<value_type, Scalar, Derived>{scalar, derived()} };
	}

	template<class Scalar,
		class = std::enable_if_t<!std::is_base_of_v<MatrixBase<Scalar>, Scalar>>>
		const CwiseBinaryOp<impl::ScalarSubOp<Scalar, value_type>,
		Derived,
		impl::Constant<value_type, Scalar, Derived>>
		operator-(const Scalar& scalar) const
	{
		return CwiseBinaryOp{ impl::ScalarSubOp<Scalar, value_type>{}, derived(),
			impl::Constant<value_type, Scalar, Derived>{scalar, derived()} };
	}

	template<class Scalar,
		class = std::enable_if_t<!std::is_base_of_v<MatrixBase<Scalar>, Scalar>>>
		const CwiseBinaryOp<impl::ScalarProductOp<Scalar, value_type>,
		Derived,
		impl::Constant<value_type, Scalar, Derived>>
		operator*(const Scalar& scalar) const
	{
		return CwiseBinaryOp{ impl::ScalarProductOp<Scalar, value_type>{}, derived(),
			impl::Constant<value_type, Scalar, Derived>{scalar, derived()} };
	}

	template<class Scalar,
		class = std::enable_if_t<!std::is_base_of_v<MatrixBase<Scalar>, Scalar>>>
		const CwiseBinaryOp<impl::ScalarQuotientOp<Scalar, value_type>,
		Derived,
		impl::Constant<value_type, Scalar, Derived>>
		operator/(const Scalar& scalar) const
	{
		return CwiseBinaryOp{ impl::ScalarQuotientOp<Scalar, value_type>{}, derived(),
			impl::Constant<value_type, Scalar, Derived>{scalar, derived()} };
	}

	const TransposeOp<Derived> transpose() const
	{
		return TransposeOp<Derived>{ derived() };
	}
};