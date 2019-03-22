#pragma once
#include "ForwardDeclarations.h"

namespace impl
{

template<class Dest, class Expr>
void assignmentLoopCoeff(Dest& dest, const Expr& expr)
{
	// TODO: Index based assignement
	// TODO: Correct major order assignement
	// TODO: OpenMp
	for (int i = 0; i < dest.rows(); ++i)
		for (int j = 0; j < dest.cols(); ++j)
			dest.evaluateRef(i, j) = expr.evaluate(i, j);
}

template<class Dest, class Expr>
void assignmentLoopCoeffPacket(Dest& dest, const Expr& expr)
{
	using size_type		= typename Dest::size_type;
	using Traits		= PacketTraits<typename Dest::value_type>;
	using PacketType	= typename Traits::type;

	enum
	{
		Stride = Traits::Stride,
	};

	// TODO: Make just as optimal for ColumnOrder matrix's
	// through use of outer / inner (need to be implemented in all evaluators)

	size_type rows = dest.rows();
	size_type cols = dest.cols();

	size_type maxRow = rows - rows % Stride;
	size_type maxCol = cols - cols % Stride;

	// TODO: Detect non-aligned memory / do non-aligned memory up to aligned block

	// Do aligned ops
	for (size_type i = 0; i < maxRow; ++i)
		for (size_type j = 0; j < maxCol; j += Stride)
			dest.writePacket<PacketType>(i, j, expr.packet<PacketType>(i, j));
	
	// Do leftovers
	for(size_type i = 0; i < rows; ++i)
		for (size_type j = maxCol; j < cols; ++j)
			dest.evaluateRef(i, j) = expr.evaluate(i, j);
}

template<class... Args>
struct Assignment {};

template<class... Args>
struct AssignmentKernel {};

template<class DestImpl, class ExprImpl>
struct AssignmentKernel<DestImpl, ExprImpl>
{
	AssignmentKernel(DestImpl& destImpl, ExprImpl& exprImpl) :
		destImpl{ destImpl },
		exprImpl{ exprImpl }
	{}

	inline void run()
	{
		
	}

private:
	DestImpl& destImpl;
	ExprImpl& exprImpl;
};

template<class Dest, class ExprEval>
struct ActualDest
{
	using DestType = std::conditional_t<ExprEval::MajorOrder != Dest::MajorOrder,
		TransposeEvaluator<Dest>, Evaluator<Dest>>;
	using size_type		= typename Dest::size_type;
	using value_type	= typename Dest::value_type;

	enum
	{
		MajorOrder = Dest::MajorOrder
	};


	ActualDest(Dest& dest) :
		dest{ dest }
	{}

	value_type& evaluateRef(size_type row, size_type col)
	{
		return dest.evaluateRef(row, col);
	}

	template<class Packet>
	void writePacket(size_type row, size_type col, const Packet& p) 
	{
		dest.template writePacket<Packet>(row, col, p);
	}

	void set(Dest&& src)
	{
		dest.set(std::move(src));
	}

	size_type rows() const noexcept { return dest.rows(); }
	size_type cols() const noexcept { return dest.cols(); }
	//size_type outer() const noexcept { return MajorOrder ? cols() : rows(); }
	//size_type inner() const noexcept { return MajorOrder ? rows() : cols(); }

private:
	DestType dest;
};

template<class Dest, class Lhs, class Rhs, class Type> // TODO: Specilaize for +=, etc
struct Assignment<Dest, ProductOp<Lhs, Rhs>, Type>
{
	using ExprType = ProductOp<Lhs, Rhs>;

	inline static void run(Dest& dest, const ExprType& expr)
	{
		using ExprEval = Evaluator<ExprType>;
		
		// Note:  No need to resize when we're just moving the temporary in below
		// BUG: Why does resizing this then deleting it result in an error with AlignedAlloc?
		//dest.resize(expr.resultRows(), expr.resultCols());

		ExprEval					exprE{ expr };
		ActualDest<Dest, ExprEval>	actDest{ dest };
		
		actDest.set(exprE.moveMatrix());
	}
};

template<class Dest, class Op, class Lhs, class Rhs, class Type> // TODO: Specilaize for +=, etc
struct Assignment<Dest, CwiseBinaryOp<Op, Lhs, Rhs>, Type>
{
	using ExprType = CwiseBinaryOp<Op, Lhs, Rhs>;

	inline static void run(Dest& dest, const ExprType& expr)
	{
		using ExprEval = Evaluator<ExprType>;

		dest.resize(expr.resultRows(), expr.resultCols());

		ExprEval					exprE{ expr };
		ActualDest<Dest, ExprEval>	destE{ dest };

		// TODO: Specialize AssignmentKernel for this stuff
		if constexpr(ExprEval::Packetable)
			assignmentLoopCoeffPacket(destE, exprE);
		else
			assignmentLoopCoeff(destE, exprE);
	}
};

template<class Dest, class Expr, class Type> // TODO: Specilaize for +=, etc
struct Assignment<Dest, TransposeOp<Expr>, Type>
{
	using ExprType = TransposeOp<Expr>;

	inline static void run(Dest& dest, const ExprType& expr)
	{
		using ExprEval = Evaluator<ExprType>;

		dest.resize(expr.resultRows(), expr.resultCols());

		ExprEval					exprE{ expr };
		ActualDest<Dest, ExprEval>	destE{ dest };

		// TODO: Specialize AssignmentKernel for this stuff
		if constexpr (ExprEval::Packetable)
			assignmentLoopCoeffPacket(destE, exprE);
		else
			assignmentLoopCoeff(destE, exprE);
	}
};


} // End impl::
