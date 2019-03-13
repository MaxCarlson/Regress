#pragma once

namespace impl
{
using size_type = int;
}

template<class Derived>
struct BaseExpr
{
	using size_type = typename impl::size_type;
};

template<class Derived>
class MatrixBase;

template<class, bool>
class MatrixT;

template<class T>
struct Traits
{};

template<class Op, class Lhs, class Rhs>
class CwiseBinaryOp;

namespace impl
{
// Types of CwiseBinaryOp's
template<class Type>
class AddOp;

template<class Type>
class SubOp;

template<class Type>
class CwiseProductOp;

template<class Type>
class CwiseQuotientOp;

// Wrapper for numerical op's
template<class Type>
class Constant;

template<class Scalar, class Type>
class ScalarProductOp;
}

// End Types of CwiseBinaryOp's

template<class Lhs, class Rhs>
class ProductOp;

// TODO: Move to an impl
template<class T>
struct RefSelector
{
	// If T is an expression type will be by value, otherwise (e.g. for Matrix) 
	// it type is const T&
	using type = std::conditional_t<T::IsExpr, T, const T&>;
};