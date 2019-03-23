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
struct Evaluator<Matrix<Type, MajorOrder>>
	: public EvaluatorBase<Matrix<Type, MajorOrder>>
{
	using MatrixType	= Matrix<Type, MajorOrder>;
	using size_type		= typename MatrixType::size_type;
	using value_type	= Type;

	enum
	{
		MajorOrder	= MajorOrder,
		Packetable	= PacketTraits<Type>::Packetable,
		Indexable	= Packetable
	};
	
	explicit Evaluator(const MatrixType& m) noexcept :
		m{ m }
	{}

	size_type size() const noexcept { return m.size(); }
	size_type rows() const noexcept { return m.rows(); }
	size_type cols() const noexcept { return m.cols(); }

	value_type& evaluateRef(size_type row, size_type col)
	{
		return const_cast<MatrixType&>(m)(row, col);
	}

	value_type& evaluateRef(size_type idx)
	{
		return const_cast<MatrixType&>(m).index(idx);
	}

	value_type evaluate(size_type row, size_type col) const
	{
		return m(row, col);
	}

	value_type evaluate(size_type idx) const
	{
		return m.index(idx);
	}

	template<class Packet>
	Packet packet(size_type row, size_type col) const
	{
		return pload<Packet, value_type>(&m(row, col));
	}

	template<class Packet>
	Packet packet(size_type idx) const
	{
		return pload<Packet>(&m.index(idx));
	}

	template<class Packet>
	void writePacket(size_type row, size_type col, const Packet& p) 
	{
		return pstore(const_cast<value_type*>(&m(row, col)), p);
	}

	template<class Packet>
	void writePacket(size_type idx, const Packet& p)
	{
		return pstore(const_cast<value_type*>(&m.index(idx)), p);
	}

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
		MajorOrder		= Lhs::MajorOrder,
		LhsMajorOrder	= Lhs::MajorOrder,
		RhsMajorOrder	= Rhs::MajorOrder,
		Packetable		= std::is_same_v<typename Lhs::value_type, typename Rhs::value_type>
			&& LhsE::Packetable && RhsE::Packetable && Func::Packetable,
		Indexable		= LhsMajorOrder == RhsMajorOrder && Packetable 
			&& LhsE::Indexable && RhsE::Indexable
	};

	explicit BinaryEvaluator(const Expr& expr) :
		op{ expr.getOp() },
		lhs{ expr.getLhs() },
		rhs{ expr.getRhs() }
	{}

	size_type size() const noexcept { return lhs.size(); } // Sizes do match here
	size_type rows() const noexcept { return lhs.rows(); }
	size_type cols() const noexcept { return rhs.cols(); }

	value_type evaluate(size_type row, size_type col) const
	{
		return op(lhs.evaluate(row, col), rhs.evaluate(row, col));
	}

	value_type evaluate(size_type idx) const
	{
		return op(lhs.evaluate(idx), rhs.evaluate(idx));
	}

	template<class Packet>
	Packet packet(size_type row, size_type col) const
	{
		return op.packetOp<Packet>(lhs.template packet<Packet>(row, col), rhs.template packet<Packet>(row, col));
	}

	template<class Packet>
	Packet packet(size_type idx) const
	{
		return op.packetOp<Packet>(lhs.template packet<Packet>(idx), rhs.template packet<Packet>(idx));
	}

protected:
	const Func op;
	LhsE lhs;
	RhsE rhs;
};

// Evaluates the entire expression
// TODO: Look into FMA instructions!
// TODO: Move to assignment / integrate with Evaluators better
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
			dest.evaluateRef(i, j) = sum;
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
	using Op			= ProductOp<Lhs, Rhs>;
	using LhsE			= Evaluator<Lhs>;
	using RhsE			= Evaluator<Rhs>;
	using value_type	= typename Op::value_type;

	enum 
	{
		MajorOrder		= Lhs::MajorOrder,
		LhsMajorOrder	= Lhs::MajorOrder,
		RhsMajorOrder	= Rhs::MajorOrder,

		// TODO: We need two traits here, (Packetable, Indexable) and (Packetable/Indexable after evaluation)
		// due to the fact that we could encounter a situation where we could not packet/index
		// to evaluate this expression, but after evaluation we could packet

		Packetable		= std::is_same_v<typename Lhs::value_type, typename Rhs::value_type>
			&& LhsE::Packetable && RhsE::Packetable,
		Indexable		= LhsMajorOrder == RhsMajorOrder && Packetable
			&& LhsE::Indexable && RhsE::Indexable
	};

	using MatrixType	= Matrix<value_type, MajorOrder>;


	explicit ProductEvaluator(const Op& expr) :
		lhsE{ expr.getLhs() },
		rhsE{ expr.getRhs() },
		matrix(expr.resultRows(), expr.resultCols()),
		matrixEval{ matrix }
	{
		productEntireImpl<ThisType, LhsE, RhsE, value_type>(*this, lhsE, rhsE);
	}

	MatrixType&& moveMatrix() { return std::move(matrix); }

	size_type size() const noexcept { return matrix.size(); } // Sizes do match here
	size_type rows() const noexcept { return lhsE.rows(); }
	size_type cols() const noexcept { return rhsE.cols(); }

	value_type& evaluateRef(size_type row, size_type col)
	{
		return matrixEval.evaluateRef(row, col);
	}

	value_type& evaluateRef(size_type idx)
	{
		return matrixEval.evaluateRef(idx);
	}

	value_type evaluate(size_type row, size_type col) const
	{
		return matrixEval.evaluate(row, col);
	}

	value_type evaluate(size_type idx) const
	{
		return matrixEval.evaluate(idx);
	}

	template<class Packet>
	Packet packet(size_type row, size_type col) const
	{
		return matrixEval.template packet<Packet>(row, col);
	}

	template<class Packet>
	Packet packet(size_type idx) const
	{
		return matrixEval.template packet<Packet>(idx);
	}

	template<class Packet>
	void writePacket(size_type row, size_type col, const Packet& p)
	{
		return matrixEval.template writePacket<Packet>(row, col);
	}

	template<class Packet>
	void writePacket(size_type idx, const Packet& p)
	{
		return matrixEval.template writePacket<Packet>(idx);
	}

protected:
	LhsE lhsE;
	RhsE rhsE;
	MatrixType matrix;
	Evaluator<MatrixType> matrixEval;
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
	using ExprE			= Evaluator<Expr>;
	using size_type		= typename Expr::size_type;
	using value_type	= typename Expr::value_type;

	enum
	{
		MajorOrder	= Expr::MajorOrder,
		Packetable	= false,
		Indexable	= false
	};

	// TODO: This should probably be derived from UnaryEvaluator

	explicit TransposeEvaluator(const Op& expr) :
		exprE{ expr.getExpr() }
	{}


	size_type size() const noexcept { return exprE.size(); }
	size_type rows() const noexcept { return exprE.cols(); }
	size_type cols() const noexcept { return exprE.rows(); }

	value_type& evaluateRef(size_type row, size_type col)
	{
		return exprE.evaluateRef(col, row);
	}

	value_type& evaluateRef(size_type idx)
	{
		return exprE.evaluateRef(idx);
	}

	value_type evaluate(size_type row, size_type col) const
	{
		return exprE.evaluate(col, row);
	}

	value_type evaluate(size_type idx) const
	{
		return exprE.evaluate(idx);
	}

	// TODO: This definitely does NOT work
	template<class Packet>
	Packet packet(size_type row, size_type col) const
	{
		static_assert("Cannot Packet TransposeOp");
		return exprE.template packet<Packet>(col, row);
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
		Packetable	= ThisExpr::Packetable,
		Indexable	= Packetable
	};

	explicit ConstantEvaluator(const ThisExpr& expr) :
		expr{ expr }
	{}

	value_type evaluate(size_type, size_type) const
	{
		return expr.evaluate();
	}

	value_type evaluate(size_type) const
	{
		return expr.evaluate();
	}

	template<class Packet>
	Packet packet(size_type, size_type) const
	{
		return expr.packet();
	}

	template<class Packet>
	Packet packet(size_type) const
	{
		return expr.packet();
	}

protected:
	const ThisExpr& expr;
};

} // End impl::