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
// Note: Capable of wrapping a non-const matrix and still using evaluateRef for assignment
template<class Type, bool MajorOrder>
struct Evaluator<MatrixT<Type, MajorOrder>>
	: public EvaluatorBase<MatrixT<Type, MajorOrder>>
{
	using MatrixType	= MatrixT<Type, MajorOrder>;
	using size_type		= typename MatrixType::size_type;
	using value_type	= Type;

	enum
	{
		Packetable = true
	};
	
	explicit Evaluator(const MatrixType& m) :
		m{ m }
	{}

	value_type& evaluateRef(size_type row, size_type col)
	{
		return const_cast<MatrixType&>(m)(row, col);
	}

	value_type evaluate(size_type row, size_type col) const
	{
		return m(row, col);
	}

	template<class Packet>
	Packet packet(size_type row, size_type col) const
	{
		return pload<Packet, value_type>(&m(row, col));
	}

	template<class Packet>
	Packet packet(size_type index) const
	{
		return pload<Packet>(&m.index(index));
	}

	template<class Packet>
	void writePacket(size_type row, size_type col, const Packet& p) const
	{
		return pstore(const_cast<value_type*>(&m(row, col)), p);
	}

	template<class Packet>
	void writePacket(size_type index, const Packet& p) const
	{
		return pstore(const_cast<value_type*>(&m.index(index)), p);
	}

	size_type rows() const noexcept { return m.rows(); }
	size_type cols() const noexcept { return m.cols(); }

	void set(MatrixType&& other)
	{
		const_cast<MatrixType&>(m) = std::move(other);
	}

protected:
	const MatrixType& m;
};

template<class Func, class Lhs, class Rhs>
struct Evaluator<CwiseBinaryOp<Func, Lhs, Rhs>>
	: public BinaryEvaluator<CwiseBinaryOp<Func, Lhs, Rhs>>
{
	using Op	= CwiseBinaryOp<Func, Lhs, Rhs>;
	using Base	= BinaryEvaluator<Op>;
	explicit Evaluator(const Op& op) : Base(op) {}
};

template<class Func, class Lhs, class Rhs>
struct BinaryEvaluator<CwiseBinaryOp<Func, Lhs, Rhs>>
	: public EvaluatorBase<CwiseBinaryOp<Func, Lhs, Rhs>>
{
	using Expr			= CwiseBinaryOp<Func, Lhs, Rhs>;
	using size_type		= typename Lhs::size_type;
	using value_type	= typename Lhs::value_type;
	using LhsE			= Evaluator<Lhs>;
	using RhsE			= Evaluator<Rhs>;


	enum
	{
		MajorOrder = Lhs::MajorOrder,
		Packetable = std::is_same_v<typename Lhs::value_type, typename Rhs::value_type>
			&& LhsE::Packetable && RhsE::Packetable
		// TODO: Indexable ? Lhs::MajorOrder == Rhs::MajorOrder && Lhs::Indexable && Rhs::Indexable
	};

	explicit BinaryEvaluator(const Expr& expr) :
		op{ expr.getOp() },
		lhs{ expr.getLhs() },
		rhs{ expr.getRhs() }
	{}

	value_type evaluate(size_type row, size_type col) const
	{
		return op(lhs.evaluate(row, col), rhs.evaluate(row, col));
	}

	template<class Packet>
	Packet packet(size_type row, size_type col) const
	{
		return op.packetOp<Packet>(lhs.template packet<Packet>(row, col), rhs.template packet<Packet>(row, col));
	}

	size_type rows() const noexcept { return lhs.rows(); }
	size_type cols() const noexcept { return rhs.cols(); }

protected:
	const Func op;
	LhsE lhs;
	RhsE rhs;
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
	using Op	= ProductOp<Lhs, Rhs>;
	using Base	= ProductEvaluator<Op>;
	explicit Evaluator(const Op& op) : Base(op) {}
};

// Basic ProductEvaluator that creates a temporary
// TODO: ProductEvaluator with no temporary
template<class Lhs, class Rhs>
struct ProductEvaluator<ProductOp<Lhs, Rhs>>
	: public EvaluatorBase<ProductOp<Lhs, Rhs>>
{
	using ThisType		= ProductEvaluator<ProductOp<Lhs, Rhs>>;
	using Expr			= ProductOp<Lhs, Rhs>;
	using LhsE			= Evaluator<Lhs>;
	using RhsE			= Evaluator<Rhs>;
	using value_type	= typename Expr::value_type;
	using MatrixType	= MatrixT<value_type, MajorOrder>;

	enum 
	{
		MajorOrder = Lhs::MajorOrder,
		Packetable = std::is_same_v<typename Lhs::value_type, typename Rhs::value_type>
			&& LhsE::Packetable && RhsE::Packetable
	};

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

template<class Expr>
struct Evaluator<TransposeOp<Expr>>
	: public TransposeEvaluator<TransposeOp<Expr>>
{
	using Op	= TransposeOp<Expr>;
	using Base	= TransposeEvaluator<Op>;
	explicit Evaluator(const Op& op) : Base(op) {}
};

template<class Expr>
struct TransposeEvaluator<TransposeOp<Expr>>
	: public EvaluatorBase<TransposeOp<Expr>>
{
	using ThisType		= TransposeEvaluator<TransposeOp<Expr>>;
	using Op			= TransposeOp<Expr>;
	using ExprE			= Evaluator<Op>;
	using size_type		= typename Expr::size_type;
	using value_type	= typename Expr::value_type;

	enum
	{
		Packetable = ExprE::Packetable
	};

	explicit TransposeEvaluator(const Op& expr) :
		exprE{ expr }
	{}

	size_type rows() const noexcept { return exprE.cols(); }
	size_type cols() const noexcept { return exprE.rows(); }

	value_type& evaluate(size_type row, size_type col)
	{
		return exprE.evaluate(col, row);
	}

	value_type evaluate(size_type row, size_type col) const
	{
		return exprE.evaluate(col, row);
	}

protected:
	ExprE exprE;
};

template<class Type, class Expr>
struct Evaluator<Constant<Type, Expr>>
	: public ConstantEvaluator<Constant<Type, Expr>>
{
	using Op	= Constant<Type, Expr>;
	using Base	= ConstantEvaluator<Op>;
	explicit Evaluator(const Op& op) : Base(op) {}
};

template<class Type, class Expr>
struct ConstantEvaluator<Constant<Type, Expr>>
	: public EvaluatorBase<Constant<Type, Expr>>
{
	using ThisExpr		= Constant<Type, Expr>;
	using size_type		= int;
	using value_type	= Type;

	enum
	{
		Packetable = true
	};

	explicit ConstantEvaluator(const ThisExpr& expr) :
		expr{ expr }
	{}

	value_type evaluate(size_type, size_type) const
	{
		return expr.getValue();
	}

protected:
	const ThisExpr& expr;
};

} // End impl::