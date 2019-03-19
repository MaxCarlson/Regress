#pragma once
#include "ForwardDeclarations.h"

namespace impl
{

template<class Expr>
struct EvaluatorBase 
{};

template<class Expr>
struct Evaluator : public EvaluatorBase<Expr>
{
	//explicit Evaluator(const Expr& expr) {}
};

// Specialization for Matrix
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

	value_type& evaluate(size_type row, size_type col)
	{
		return m(row, col);
	}

	value_type evaluate(size_type row, size_type col) const
	{
		return m(row, col);
	}

	size_type rows() const noexcept { return m.rows(); }
	size_type cols() const noexcept { return m.cols(); }

protected:
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
		return op(lhs.evaluate(row, col), rhs.evaluate(row, col));
	}

	size_type rows() const noexcept { return lhs.rows(); }
	size_type cols() const noexcept { return rhs.cols(); }

protected:
	const Op op;
	Evaluator<Lhs> lhs;
	Evaluator<Rhs> rhs;
};

enum class ProductOption // TODO: Options for product evaluation
{

};

// Evaluates the entire expression
// TODO: Add an impl that only evaluates for an index
template<class Dest, class LhsE, class RhsE, class Type>
void productEntireImpl(Dest& dest, const LhsE& lhsE, const RhsE& rhsE)
{
	for (int i = 0; i < lhsE.rows(); ++i)
		for (int j = 0; j < rhsE.cols(); ++j)
		{
			Type sum = 0;
			for(int h = 0; h < rhsE.rows(); ++h)
				sum += lhsE.evaluate(i, h) * rhsE.evaluate(h, j); 
			dest.evaluate(i, j) = sum;
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
	using ThisType		= ProductEvaluator<ProductOp<Lhs, Rhs>>;
	using Expr			= ProductOp<Lhs, Rhs>;
	using LhsE			= Evaluator<Lhs>;
	using RhsE			= Evaluator<Rhs>;
	using value_type	= typename Expr::value_type;

	enum Info
	{
		MajorOrder = Lhs::MajorOrder
	};

	using MatrixType	= MatrixT<value_type, MajorOrder>;


	explicit ProductEvaluator(const Expr& expr) :
		lhsE{ expr.getLhs() },
		rhsE{ expr.getRhs() },
		m( expr.resultRows(), expr.resultCols() )
	{
		productEntireImpl<ThisType, LhsE, RhsE, value_type>(*this, lhsE, rhsE);
	}

	MatrixType&& moveMatrix() { return std::move(m); }

	size_type rows() const noexcept { return lhsE.rows(); }
	size_type cols() const noexcept { return rhsE.cols(); }

	value_type& evaluate(size_type row, size_type col)
	{
		return m(row, col);
	}

	value_type evaluate(size_type row, size_type col) const
	{
		return m(row, col);
	}

protected:
	LhsE lhsE;
	RhsE rhsE;
	MatrixType m;
};

} // End impl::