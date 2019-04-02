#pragma once
#include "ForwardDeclarations.h"
#include "ProductImpl.h"
#include <algorithm>

namespace impl
{

enum class GEMMType { COEFF, VECTORIZED };

template<class Dest, class LhsE, class RhsE, GEMMType arg>
struct ProductLoop{};

template<class Dest, class LhsE, class RhsE>
struct ProductLoop<Dest, LhsE, RhsE, GEMMType::COEFF>
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
struct ProductLoop<Dest, LhsE, RhsE, GEMMType::VECTORIZED>
{
	using size_type		= typename Dest::size_type;
	using value_type	= typename Dest::value_type;
	using Traits		= PacketTraits<value_type>;
	using PacketType	= typename Traits::type;
	using SALW			= StackAlignedWrapper<value_type>;

	enum
	{
		DestTranspose	= Dest::MajorOrder,
	};

	// Paper:
	// https://www.cs.utexas.edu/users/pingali/CS378/2008sp/papers/gotoPaper.pdf
	//
	// Note: gemm is written from a RowMajor Order perspective. In order to get
	// a ColumnMajor result the matricies are transposed
	//
	// Matrix Model:
	//   dest = lhs  * rhs
	//  m x n = m x k * k x n
	template<class Lhs, class Rhs, bool DestTranspose>
	static void gemm(Dest& dest, const Lhs& lhs, const Rhs& rhs, size_type lRows, size_type lCols, size_type rCols)
	{
		using IndexWrapperLhs	= IndexWrapper<const Lhs, size_type, DestTranspose>;
		using IndexWrapperRhs	= IndexWrapper<const Rhs, size_type, DestTranspose>;
		using IndexWrapperDest	= IndexWrapper<Dest, size_type, DestTranspose>;

		// TODO: Switch input blocks so it works just as well for Col MajorOrder 
		// (rhs becomes lhs, lhs -> rhs)
		// TODO: Revisit main loop order below
		// TODO: Calculate mc/kc/nc from type size/l2 cache size
		// TODO: Benchmark allocating A/B on heap/thread_local/stack

		// Blocksize along direction 
		const size_type mc = 50; // along m (rows of dest/lhs)
		const size_type kc = 50; // along k (columns of lhs, rows of rhs)
		const size_type nc = 25; // along n (columns of rhs)

		SALW blockA{ mc * kc }; // LhsBlock
		SALW blockB{ kc * nc }; // RhsBlock

		// Height (in rows) of lhs's vertical panel
		for (size_type m = 0; m < lRows; m += mc)
		{
			const size_type endM = std::min(m + mc, lRows) - m;

			// Width (in cols) of lhs's veritcal panel
			// Also height (in rows) of rhs's horizontal panel 
			for (size_type k = 0; k < lCols; k += kc)
			{
				const size_type endK = std::min(k + kc, lCols) - k;

				IndexWrapperLhs lhsW( lhs, m, k );
				packLhs(blockA.ptr, lhsW, endM, endK);

				// For each kc x nc horizontal panel of rhs
				for (size_type n = 0; n < rCols; n += nc)
				{
					const size_type endN = std::min(n + nc, rCols) - n;

					IndexWrapperRhs rhsW{ rhs, k, n };
					packRhs(blockB.ptr, rhsW, endK, endN);

					IndexWrapperDest idxWrapper{ dest, m, n };
					gepb(idxWrapper, blockA.ptr, blockB.ptr, endM, endN, endK);
				}
			}
		}
	}

	static void run(Dest& dest, const LhsE& lhs, const RhsE& rhs)
	{
		if (DestTranspose)
			gemm<RhsE, LhsE, DestTranspose>(dest, rhs, lhs, dest.cols(), lhs.cols(), dest.rows());
		else
			gemm<LhsE, RhsE, DestTranspose>(dest, lhs, rhs, dest.rows(), lhs.cols(), dest.cols());
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
	//using PackingInfoT	= PackingInfo<value_type>;

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

	static constexpr auto GemmType = Packetable 
		? GEMMType::VECTORIZED : GEMMType::COEFF;

	using MatrixType	= Matrix<value_type, MajorOrder>;

	explicit ProductEvaluator(const Op& expr) :
		lhsE{ expr.getLhs() },
		rhsE{ expr.getRhs() },
		matrix(expr.resultRows(), expr.resultCols()),
		matrixEval{ matrix }
	{
		using LoopType = ProductLoop<Evaluator<MatrixType>, LhsE, RhsE, GemmType>;
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