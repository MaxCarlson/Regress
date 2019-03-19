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
		m{ m }
	{}

	value_type evaluate(size_type row, size_type col) const
	{
		return m(row, col);
	}

private:
	const MatrixType& m;
};

template<class Op, class Lhs, class Rhs>
struct Evaluator<CwiseBinaryOp<Op, Lhs, Rhs>>
	: public ProductEvaluator<CwiseBinaryOp<Op, Lhs, Rhs>>
{
	using Expr = CwiseBinaryOp<Op, Lhs, Rhs>;
	using Base = BinaryEvaluator<Expr>;
	explicit Evaluator(const Expr& expr) : Base(expr) {}
};

template<class Op, class Lhs, class Rhs>
struct BinaryEvaluator<CwiseBinaryOp<Op, Lhs, Rhs>>
	: public EvaluatorBase<CwiseBinaryOp<Op, Lhs, Rhs>>
{
	using Expr			= CwiseBinaryOp<Op, Lhs, Rhs>;
	using size_type		= typename Lhs::size_type;
	using value_type	= decltype(typename Lhs::value_type{} + typename Lhs::value_type{});

	explicit BinaryEvaluator(const Expr& expr) :
		op{ expr.getOp() },
		lhs{ expr.getLhs() },
		rhs{ expr.getRhs() }
	{}

	value_type evaluate(size_type row, size_type col) const
	{
		return m(row, col);
	}

protected:
	const Op op;
	Evaluator<Lhs> lhs;
	Evaluator<Rhs> rhs;
};

enum class ProductOption
{

};

// Evaluates the entire expression
// TODO: Add an impl that only evaluates for an index
template<class Dest, class Lhs, class Rhs, class Type>
void productImpl(Dest& dest, const Lhs& lhs, const Rhs& rhs)
{
	Evaluator<Lhs> lhsE{ lhs };
	Evaluator<Rhs> rhsE{ rhs };

	for (int i = 0; i < lhs.rows(); ++i)
		for (int j = 0; j < rhs.cols(); ++j)
		{
			Type sum = 0;
			for(int h = 0; h < rhs.rows(); ++h)
				sum += lhsE.evaluate(i, h) * rhsE.evaluate(h, j); 
			dest(i, j) = sum;
		}
}

template<class Lhs, class Rhs>
struct Evaluator<ProductOp<Lhs, Rhs>>
	: public ProductEvaluator<ProductOp<Lhs, Rhs>>
{
	using Expr = ProductOp<Lhs, Rhs>;
	using Base = ProductEvaluator<Expr>;
	explicit Evaluator(const Expr& expr) : Base(expr) {}
};

template<class Lhs, class Rhs>
struct ProductEvaluator<ProductOp<Lhs, Rhs>>
	: public EvaluatorBase<ProductOp<Lhs, Rhs>>
{
	using Expr = ProductOp<Lhs, Rhs>;
	using value_type = decltype(typename Lhs::value_type{} + typename Lhs::value_type{});

	explicit ProductEvaluator(const Expr& expr) :
		m(expr.resultRows(), expr.resultCols())
	{
	
		productImpl<decltype(m), Lhs, Rhs, value_type>(m, expr.getLhs(), expr.getRhs());
	}

private:
	MatrixT<value_type, false> m;
};

} // End impl::