#pragma once
#include "ForwardDeclarations.h"

namespace impl
{

// Evaluates the entire expression
// TODO: Look into FMA instructions!

enum class ProductLoopTraits { COEFF, PACKET, INDEX, SLICED };

template<class Dest, class LhsE, class RhsE, ProductLoopTraits arg>
struct ProductLoop{};

template<class Dest, class LhsE, class RhsE>
struct ProductLoop<Dest, LhsE, RhsE, ProductLoopTraits::COEFF>
{
	using size_type		= typename Dest::size_type;
	using value_type	= typename Dest::value_type;

	// TODO: Get this working using outer/inner (so that column order matrix's can be mul'd as effeciently)
	inline static void run(Dest& dest, const LhsE& lhsE, const RhsE& rhsE)
	{
		const size_type lOuterSize = lhsE.rows();
		const size_type rInnerSize = rhsE.cols();
		const size_type rOuterSize = rhsE.rows();

		for (size_type i = 0; i < lOuterSize; ++i)
			for (size_type j = 0; j < rInnerSize; ++j)
			{
				value_type sum = 0;
				for (size_type h = 0; h < rOuterSize; ++h)
					sum += lhsE.evaluate(i, h) * rhsE.evaluate(h, j);
				dest.evaluateRef(i, j) = sum;
			}
	}
};

template<class Dest, class LhsE, class RhsE>
struct ProductLoop<Dest, LhsE, RhsE, ProductLoopTraits::PACKET>
{
	using size_type		= typename Dest::size_type;
	using value_type	= typename Dest::value_type;

	inline static void run(Dest& dest, const LhsE& lhsE, const RhsE& rhsE)
	{
		const size_type lOuterSize = lhsE.rows();
		const size_type rInnerSize = rhsE.cols();
		const size_type rOuterSize = rhsE.rows();
	}
};

template<class Lhs, class Rhs>
struct Evaluator<ProductOp<Lhs, Rhs>>
	: public ProductEvaluator<ProductOp<Lhs, Rhs>>
{
	using Op	= ProductOp<Lhs, Rhs>;
	using Base	= ProductEvaluator<Op>;
	explicit Evaluator(const Op& op) : Base(op) {}
};

//
// TODO: In matrix mul chains look into reordering for max effeciency
//
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
		using LoopType = ProductLoop<Evaluator<MatrixType>, LhsE, RhsE, ProductLoopTraits::COEFF>;
		LoopType::run(matrixEval, lhsE, rhsE);
	}

	MatrixType&& moveMatrix() { return std::move(matrix); }

	size_type size() const noexcept { return matrix.size(); } // Sizes do match here
	size_type rows() const noexcept { return lhsE.rows(); }
	size_type cols() const noexcept { return rhsE.cols(); }
	size_type outer() const noexcept { return matrixEval.outer(); }
	size_type inner() const noexcept { return matrixEval.inner(); }

	size_type numOps() const noexcept { return lhsE.rows() * lhsE.cols() * rhsE.cols(); }

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

} // End impl::