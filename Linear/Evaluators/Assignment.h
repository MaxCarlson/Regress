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
	// TODO: intrinsics
	for (int i = 0; i < dest.rows(); ++i)
		for (int j = 0; j < dest.cols(); ++j)
			dest.evaluateRef(i, j) = expr.evaluate(i, j);
}

template<class Dest, class Expr>
void assignmentLoopPacket(Dest& dest, const Expr& expr)
{

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


	ActualDest(Dest& dest) :
		dest{ dest }
	{}

	value_type& evaluateRef(size_type row, size_type col)
	{
		return dest.evaluateRef(row, col);
	}

	void set(Dest&& src)
	{
		dest.set(std::move(src));
	}

	size_type rows() const noexcept { return dest.rows(); }
	size_type cols() const noexcept { return dest.cols(); }

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
		
		dest.resize(expr.resultRows(), expr.resultCols());

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

		assignmentLoopCoeff(destE, exprE);
	}
};


} // End impl::
