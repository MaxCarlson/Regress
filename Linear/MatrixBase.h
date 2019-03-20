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
		impl::Assignment<Matrix, Derived, value_type>::run(to, derived());
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

	const TransposeOp<Derived> transpose() const
	{
		return TransposeOp<Derived>{ derived() };
	}

	// Scalar Operators 
	// (enabled on objects that aren't derived from MatrixBase)
	// TODO: Add operators so we can do -> Matrix m; Matrix t = 2 * m;

	template<class Scalar,
		class = std::enable_if_t<!std::is_base_of_v<MatrixBase<Scalar>, Scalar>>>
	const CwiseBinaryOp<impl::AddOp<value_type>, Derived, impl::Constant<Scalar, Derived>>
		operator+(const Scalar& scalar) const
	{
		return CwiseBinaryOp{ impl::AddOp<value_type>{}, derived(), 
			impl::Constant<Scalar, Derived>{scalar, derived()} };
	}

	template<class Scalar,
		class = std::enable_if_t<!std::is_base_of_v<MatrixBase<Scalar>, Scalar>>>
		const CwiseBinaryOp<impl::SubOp<value_type>, Derived, impl::Constant<Scalar, Derived>>
		operator-(const Scalar& scalar) const
	{
		return CwiseBinaryOp{ impl::SubOp<value_type>{}, derived(),
			impl::Constant<Scalar, Derived>{scalar, derived()} };
	}

	template<class Scalar,
		class = std::enable_if_t<!std::is_base_of_v<MatrixBase<Scalar>, Scalar>>>
		const CwiseBinaryOp<impl::MulOp<value_type>, Derived, impl::Constant<Scalar, Derived>>
		operator*(const Scalar& scalar) const
	{
		return CwiseBinaryOp{ impl::MulOp<value_type>{}, derived(),
			impl::Constant<Scalar, Derived>{scalar, derived()} };
	}

	template<class Scalar,
		class = std::enable_if_t<!std::is_base_of_v<MatrixBase<Scalar>, Scalar>>>
		const CwiseBinaryOp<impl::DivOp<value_type>, Derived, impl::Constant<Scalar, Derived>>
		operator/(const Scalar& scalar) const
	{
		return CwiseBinaryOp{ impl::DivOp<value_type>{}, derived(),
			impl::Constant<Scalar, Derived>{scalar, derived()} };
	}
};