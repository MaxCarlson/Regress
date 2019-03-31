#pragma once
#include "ForwardDeclarations.h"
#include "ProductEvaluators.h"

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
	size_type outer() const noexcept { return MajorOrder ? m.cols() : m.rows(); }
	size_type inner() const noexcept { return MajorOrder ? m.rows() : m.cols(); }

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

	template<class Packet, bool IsAligned = true>
	Packet packet(size_type row, size_type col) const
	{
		if (IsAligned)
			return pload<Packet, value_type>(&m(row, col));
		else
			return impl::ploadu<Packet, value_type>(&m(row, col));
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
	size_type outer() const noexcept { return lhs.outer(); } 
	size_type inner() const noexcept { return lhs.inner(); } // Lhs/Rhs Outer/inner match

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
	size_type outer() const noexcept { return exprE.inner(); }
	size_type inner() const noexcept { return exprE.outer(); }

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