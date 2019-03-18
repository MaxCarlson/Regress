#pragma once
#include "ForwardDeclarations.h"

template<class Expr>
struct EvaluatorBase 
{

};

template<class Derived>
struct Evaluator : public EvaluatorBase<Derived>
{
	Evaluator() {}
};

// Specilzation for Matrix
template<class Derived>
struct Evaluator<MatrixBase<Derived>>
	: public EvaluatorBase<Derived>
{
	using MatrixType = MatrixBase<Derived>;

	Evaluator(const MatrixType& m) :
		m{m}
	{}

private:
	const MatrixType& m;
};

template<class Op>
struct BinaryEvaluator {};

template<class Op, class Lhs, class Rhs>
struct BinaryEvaluator<CwiseBinaryOp<Op, Lhs, Rhs>>
	: public EvaluatorBase<CwiseBinaryOp<Op, Lhs, Rhs>>
{
	using Expr = CwiseBinaryOp<Op, Lhs, Rhs>;

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