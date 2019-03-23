#pragma once
#include "ForwardDeclarations.h"
#include "RegressAssert.h"
#include "Evaluators\AssignmentFunctors.h"

template<class Derived>
class MatrixBase : public BaseExpr<Derived>
{
public:
	using Base			= BaseExpr<Derived>;
	using ThisType		= MatrixBase<Derived>;
	using value_type	= typename Traits<Derived>::value_type;
	using size_type		= typename Base::size_type;

	enum
	{
		MajorOrder	= Traits<Derived>::MajorOrder,
		IsExpr		= true // We will override this in anything not an expression 
	};

	//MatrixBase() = default;
	//MatrixBase(MatrixBase&& other) = delete;
	//MatrixBase(const MatrixBase& other) = delete;

	Derived& derived()				{ return static_cast<Derived&>(*this); }
	const Derived& derived() const	{ return static_cast<const Derived&>(*this); }

	template<class OtherDerived, class Func>
	void assign(const OtherDerived& from, const Func& func)
	{
		impl::Assignment<Derived, OtherDerived, value_type, Func>::run(derived(), from, func);
	}

	//template<class OtherDerived>
	//Derived& operator+=(const MatrixBase<OtherDerived>& other)
	//{
	//	assign(other, impl::PlusEquals<value_type>{});
	//	return derived();
	//}

	// Matrix Operators
	template<class OtherDerived>
	const CwiseBinaryOp<impl::AddOp<value_type>, Derived, OtherDerived>
		operator+(const MatrixBase<OtherDerived>& other) const
	{
		regress_assert(derived().rows() == other.derived().rows() && derived().cols() == other.derived().cols());
		return CwiseBinaryOp{ impl::AddOp<value_type>{}, derived(), other.derived() };
	}

	template<class OtherDerived>
	const CwiseBinaryOp<impl::SubOp<value_type>, Derived, OtherDerived>
		operator-(const MatrixBase<OtherDerived>& other) const
	{
		regress_assert(derived().rows() == other.derived().rows() && derived().cols() == other.derived().cols());
		return CwiseBinaryOp{ impl::SubOp<value_type>{}, derived(), other.derived() };
	}

	template<class OtherDerived>
	const ProductOp<Derived, OtherDerived>
		operator*(const MatrixBase<OtherDerived>& other) const
	{
		regress_assert(derived().cols() == other.derived().rows());
		return ProductOp{ derived(), other.derived() };
	}

	// Other Matrix Functions that return Expressions
	template<class OtherDerived>
	const CwiseBinaryOp<impl::CwiseProductOp<value_type>, Derived, OtherDerived>
		cwiseProduct(const MatrixBase<OtherDerived>& other) const
	{
		regress_assert(derived().rows() == other.derived().rows() && derived().cols() == other.derived().cols());
		return CwiseBinaryOp{ impl::CwiseProductOp<value_type>{}, derived(), other.derived() };
	}

	const TransposeOp<Derived> transpose() const
	{
		return TransposeOp<Derived>{ derived() };
	}

	// Scalar Operators 
	// (enabled on objects that aren't derived from MatrixBase)
	// Right hand scalar matrix operators
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

	// Left hand scalar matrix operators
	// TODO: Should these even exist?
	template<class Scalar, class DerivedT, class>
	friend const CwiseBinaryOp<impl::AddOp<typename DerivedT::value_type>, impl::Constant<Scalar, DerivedT>, DerivedT>
		operator+(const Scalar& scalar, const MatrixBase<DerivedT>& d);

	template<class Scalar, class DerivedT, class>
	friend const CwiseBinaryOp<impl::AddOp<typename DerivedT::value_type>, impl::Constant<Scalar, DerivedT>, DerivedT>
		operator-(const Scalar& scalar, const MatrixBase<DerivedT>& d);

	template<class Scalar, class DerivedT, class>
	friend const CwiseBinaryOp<impl::AddOp<typename DerivedT::value_type>, impl::Constant<Scalar, DerivedT>, DerivedT>
		operator*(const Scalar& scalar, const MatrixBase<DerivedT>& d);

	template<class Scalar, class DerivedT, class>
	friend const CwiseBinaryOp<impl::AddOp<typename DerivedT::value_type>, impl::Constant<Scalar, DerivedT>, DerivedT>
		operator/(const Scalar& scalar, const MatrixBase<DerivedT>& d);
};

template<class Scalar, class DerivedT,
	class = std::enable_if_t<!std::is_base_of_v<MatrixBase<Scalar>, Scalar>>>
inline const CwiseBinaryOp<impl::AddOp<typename DerivedT::value_type>, impl::Constant<Scalar, DerivedT>, DerivedT>
operator+(const Scalar& scalar, const MatrixBase<DerivedT>& d)
{
	return CwiseBinaryOp{ impl::AddOp<typename DerivedT::value_type>{}, 
		impl::Constant<Scalar, DerivedT>{scalar, d.derived()}, d.derived() };
}

template<class Scalar, class DerivedT, 
	class = std::enable_if_t<!std::is_base_of_v<MatrixBase<Scalar>, Scalar>>>
inline const CwiseBinaryOp<impl::SubOp<typename DerivedT::value_type>, impl::Constant<Scalar, DerivedT>, DerivedT>
operator-(const Scalar& scalar, const MatrixBase<DerivedT>& d)
{
	return CwiseBinaryOp{ impl::SubOp<typename DerivedT::value_type>{}, 
		impl::Constant<Scalar, DerivedT>{scalar, d.derived()}, d.derived() };
}

template<class Scalar, class DerivedT, 
	class = std::enable_if_t<!std::is_base_of_v<MatrixBase<Scalar>, Scalar>>>
inline const CwiseBinaryOp<impl::MulOp<typename DerivedT::value_type>, impl::Constant<Scalar, DerivedT>, DerivedT>
operator*(const Scalar& scalar, const MatrixBase<DerivedT>& d)
{
	return CwiseBinaryOp{ impl::MulOp<typename DerivedT::value_type>{}, 
		impl::Constant<Scalar, DerivedT>{scalar, d.derived()}, d.derived() };
}

template<class Scalar, class DerivedT, 
	class = std::enable_if_t<!std::is_base_of_v<MatrixBase<Scalar>, Scalar>>>
inline const CwiseBinaryOp<impl::DivOp<typename DerivedT::value_type>, impl::Constant<Scalar, DerivedT>, DerivedT>
operator/(const Scalar& scalar, const MatrixBase<DerivedT>& d)
{
	return CwiseBinaryOp{ impl::DivOp<typename DerivedT::value_type>{}, 
		impl::Constant<Scalar, DerivedT>{scalar, d.derived()}, d.derived() };
}
