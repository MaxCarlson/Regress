#pragma once
#include "ForwardDeclarations.h"
#include "ProductHelpers.h"
#include <algorithm>

namespace impl
{

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
	using Traits		= PacketTraits<value_type>;
	using PacketType	= typename Traits::type;

	enum { Alignment = Traits::Alignment };

	template<class Index>
	static void testGepb(Dest& dest, const LhsE& lhs, const RhsE& rhs, Index ii, Index jj, Index pp, Index endI, Index endJ, Index endP)
	{
		for (Index p = pp; p < pp + endP; ++p)
			for (Index i = ii; i < ii + endI; ++i)
				for (Index j = jj; j < jj + endJ; ++j)
				{
					dest.evaluateRef(i, j) += lhs.evaluate(i, p) * rhs.evaluate(p, j);
				}
	}

	inline static void run(Dest& dest, const LhsE& lhsE, const RhsE& rhsE)
	{
		const size_type lRows = lhsE.rows();
		const size_type lCols = lhsE.cols();
		const size_type rRows = rhsE.rows();
		const size_type rCols = rhsE.cols();

		// Matrix Model:
		//   dest = lhsE  * rhsE
		//  m x n = m x k * k x n
		// 


		// TODO: Calculate from type size/l2 cache size

		// Blocksize along direction 
		size_type mc = 4; // along m (rows of dest/lhs)
		size_type kc = 4; // along k (columns of lhs, rows of rhs)
		size_type nc = 4; // along n (columns of rhs)

		// TODO: Benchmark allocating on heap/thread_local/stack
		StackAlignedWrapper<value_type> blockA{ mc * kc }; // LhsBlock
		StackAlignedWrapper<value_type> blockB{ kc * nc }; // RhsBlock

		// Height (in rows) of lhs's vertical panel
		for (size_type m = 0; m < lRows; m += mc)
		{
			const size_type endM = std::min(m + mc, lRows) - m;

			// Width (in cols) of lhs's veritcal panel
			// Also height (in rows) of rhs's horizontal panel 
			for (size_type k = 0; k < lCols; k += kc)
			{
				const size_type endK = std::min(k + kc, lCols) - k;

				// Pack lhs block
				packLhs(blockA.ptr, lhsE, m, endM, k, endK);

				// For each kc x nc vertical panel of rhs
				for (size_type n = 0; n < rCols; n += nc)
				{
					const size_type endN = std::min(n + nc, rCols) - n;

					// Pack Rhs
					packRhs(blockB.ptr, rhsE, k, endK, n, endN);

					testGepb(dest, lhsE, rhsE, m, n, k, endM, endN, endK);
				}
			}
		}
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
		using LoopType = ProductLoop<Evaluator<MatrixType>, LhsE, RhsE, ProductLoopTraits::PACKET>; // TODO: Just for testing, switch back 
		LoopType::run(matrixEval, lhsE, rhsE);
	}

	MatrixType&& moveMatrix() { return std::move(matrix); }

	size_type size() const noexcept { return matrix.size(); } // Sizes do match here
	size_type rows() const noexcept { return lhsE.rows(); }
	size_type cols() const noexcept { return rhsE.cols(); }
	size_type outer() const noexcept { return matrixEval.outer(); }
	size_type inner() const noexcept { return matrixEval.inner(); }


	//
	// TODO: In matrix mul chains look into reordering for max effeciency
	//
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