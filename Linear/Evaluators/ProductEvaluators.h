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
		Alignment		= Traits::Alignment,
		DestTranspose	= Dest::MajorOrder,
	};

	// Paper:
	// https://www.cs.utexas.edu/users/pingali/CS378/2008sp/papers/gotoPaper.pdf
	//
	// Note: gemm is written from a RowMajor Order perspective. In order to get
	// a ColumnMajor result the matricies lhs and rhs swap places and
	// coordinates transposed (rhs becomes lhs, lhs(1, 2) becomes lhs(2, 1))
	//
	// Matrix Model:
	//   dest = lhs * rhs
	//  m x n = m x k * k x n
	template<class Lhs, class Rhs>
	static void gemm(Dest& dest, const Lhs& lhs, const Rhs& rhs, size_type lRows, size_type lCols, size_type rCols)
	{
		using IndexWrapperLhs	= IndexWrapper<const Lhs, size_type, DestTranspose>;
		using IndexWrapperRhs	= IndexWrapper<const Rhs, size_type, DestTranspose>;
		using IndexWrapperDest	= IndexWrapper<Dest, size_type, DestTranspose>;
		// TODO: Calculate mc/kc/nc from type size & l1/l2 cache size
		// TODO: Benchmark allocating A/B on heap/thread_local/stack
		static const BlockSizes<value_type, size_type> sizing;

		// Blocksize along direction 
		// (roughly set to 200x200x650 atm)
		const size_type mc = std::min(sizing.mc, lRows); // along m (rows of dest/lhs)
		const size_type kc = std::min(sizing.kc, lCols); // along k (columns of lhs, rows of rhs)
		const size_type nc = std::min(sizing.nc, rCols); // along n (columns of rhs)
		
		//const size_type mc = std::min(testBs.mc, lRows); // Just for testing
		//const size_type kc = std::min(testBs.kc, lCols); // best values for gemm
		//const size_type nc = std::min(testBs.nc, rCols); // TODO: Integrate better!

		SALW blockA{ mc * kc, StackAlignedAlloc(value_type, mc * kc, Alignment) }; // LhsBlock
		SALW blockB{ kc * nc, StackAlignedAlloc(value_type, kc * nc, Alignment) }; // RhsBlock
		
		const bool packRhsOnce = kc == lCols && nc == rCols;
		
		// Height (in rows) of lhs's block
		#pragma omp parallel for num_threads(3) 
		for (size_type m = 0; m < lRows; m += mc)
		{
			const size_type endM = std::min(m + mc, lRows) - m;

			// Width (in cols) of lhs's block
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

					if(!packRhsOnce || m == 0)
						packRhs(blockB.ptr, rhsW, endK, endN);

					IndexWrapperDest idxWrapper{ dest, m, n };
					gebp(idxWrapper, blockA.ptr, blockB.ptr, endM, endN, endK);
				}
			}
		}
	}

	static void run(Dest& dest, const LhsE& lhs, const RhsE& rhs)
	{
		if (DestTranspose)
			gemm(dest, rhs, lhs, dest.cols(), lhs.cols(), dest.rows());
		else
			gemm(dest, lhs, rhs, dest.rows(), lhs.cols(), dest.cols());
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

	// TODO: Look into packing non-identical types into identical types for blocks
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

	//constexpr bool productChain()
	//{
	//	return std::is_same_v<ThisType, LhsE> && std::is_same_v<ThisType, RhsE>;
	//}

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

	template<class Packet, bool isAligned>
	Packet packet(size_type row, size_type col) const
	{
		return matrixEval.template packet<Packet, isAligned>(row, col);
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