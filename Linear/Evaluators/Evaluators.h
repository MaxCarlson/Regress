#pragma once
#include "ForwardDeclarations.h"

namespace impl
{

template<class Expr>
struct EvaluatorBase 
{

};


template<class Expr>
struct Evaluator : public EvaluatorBase<Expr>
{
	//explicit Evaluator(const Expr& expr) {}
};

// Specilzation for Matrix
template<class Type, bool MajorOrder>
struct Evaluator<MatrixT<Type, MajorOrder>>
	: public EvaluatorBase<MatrixT<Type, MajorOrder>>
{
	using MatrixType	= MatrixT<Type, MajorOrder>;
	using size_type		= typename MatrixType::size_type;
	using value_type	= Type;
	
	explicit Evaluator(const MatrixType& m) :
		m{m}
	{}

	value_type evaluate(size_type row, size_type col) const
	{
		return m(row, col);
	}

private:
	const MatrixType& m;
};

template<class Op, class Lhs, class Rhs>
struct BinaryEvaluator<CwiseBinaryOp<Op, Lhs, Rhs>>
	: public EvaluatorBase<CwiseBinaryOp<Op, Lhs, Rhs>>
{
	using Expr			= CwiseBinaryOp<Op, Lhs, Rhs>;
	using size_type		= typename Lhs::size_type;
	using value_type	= decltype(typename Lhs::value_type{} * typename Lhs::value_type{});

	explicit BinaryEvaluator(const Expr& expr) :
		op{ expr.getOp() },
		lhs{ expr.getLhs() },
		rhs{ expr.getRhs() }
	{}



private:
	const Op op;
	Evaluator<Lhs> lhs;
	Evaluator<Rhs> rhs;
};

// This Product Evaluator creates a temporary
template<class Lhs, class Rhs>
struct ProductEvaluator<ProductOp<Lhs, Rhs>>
	: public EvaluatorBase<ProductOp<Lhs, Rhs>>
{
	using Expr = ProductOp<Lhs, Rhs>;

	explicit ProductEvaluator(const Expr& expr) 
	{
	
	}

private:
	//MatrixT result;
};

} // End impl::