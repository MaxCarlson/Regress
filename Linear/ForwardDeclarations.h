#pragma once
#include <memory>

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

// Wrapper for non-MatrixBase op's (e.g. Matrix a = a * 2;)
template<class Type, class Val, class Expr>
class Constant;

template<class Scalar, class Type>
class ScalarAddOp;

template<class Scalar, class Type>
class ScalarSubOp;

template<class Scalar, class Type>
class ScalarProductOp;

template<class Scalar, class Type>
class ScalarQuotientOp;
}

// End Types of CwiseBinaryOp's

template<class Lhs, class Rhs>
class ProductOp;

template<class Expr>
class TransposeOp;

// TODO: Move to an impl
template<class T>
struct RefSelector
{
	// If T is an expression type will be by value, otherwise (e.g. for Matrix) 
	// it type is const T&
	using type = std::conditional_t<T::IsExpr, T, const T&>;
};

template<class T>
struct IteratorSelector
{
	using type = std::conditional_t<T::IsExpr, typename T::ExprIterator, typename T::const_iterator>;
};

namespace impl
{
// Evaluators
template<class Args>
struct BinaryEvaluator;

template<class Args>
struct ProductEvaluator;

// Assignment
template<class... Args>
struct Assignment;
}
