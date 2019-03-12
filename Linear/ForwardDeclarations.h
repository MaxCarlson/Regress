#pragma once

template<class Derived>
struct BaseExpr
{
	using size_type = int;
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