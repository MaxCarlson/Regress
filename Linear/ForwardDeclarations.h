#pragma once
#include <memory>
#include <xmmintrin.h>
#include <emmintrin.h>

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
class Matrix;

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
class MulOp;

template<class Type>
class DivOp;

template<class Type>
class CwiseProductOp;

// Wrapper for non-MatrixBase op's (e.g. Matrix a = a * 2;)
template<class Type, class Expr>
class Constant;
} // End Types of CwiseBinaryOp's

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
	using type = std::conditional_t<std::is_base_of_v<MatrixBase<T>, T>, const T&, T>;
};

namespace impl
{
// Evaluators
template<class Arg>
struct ConstantEvaluator;

template<class Args>
struct BinaryEvaluator;

template<class Args>
struct ProductEvaluator;

template<class Args>
struct TransposeEvaluator;

// Assignment
template<class... Args>
struct Assignment;
}

// Instrinsic wrappers
namespace impl
{

template<class Type>
struct PacketTraits;

using Packet4f = __m128;
using Packet4i = __m128i;
using Packet2d = __m128d;

template<class Packet, class Type>
Packet pload(const Type* ptr);

template<class Packet, class Type>
void pstore(Type* to, const Packet& ptr);

template<class Packet>
Packet padd(const Packet& p1, const Packet& p2);
}