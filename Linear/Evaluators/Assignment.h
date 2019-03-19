#pragma once
#include "ForwardDeclarations.h"

namespace impl
{

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
		TransposeEvaluator<Dest>, Dest&>;
	using size_type = typename Dest::size_type;

	ActualDest(Dest& dest) :
		dest{ dest }
	{}

	void evaluate(size_type row, size_type col)
	{
		dest.evaluate(row, col);
	}

	void set(Dest&& src)
	{
		dest = std::move(src);
	}

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

		ExprEval					exprE{ expr };
		ActualDest<Dest, ExprEval>	actDest{ dest };
		
		actDest.set(exprE.moveMatrix());
	}
};

template<class Dest, class Op, class Lhs, class Rhs, class Type> // TODO: Specilaize for +=, etc
struct Assignment<Dest, CwiseBinaryOp<Op, Lhs, Rhs>, Type>
{
	using ExprType = ProductOp<Lhs, Rhs>;

	inline static void run(Dest& dest, const ExprType& expr)
	{
		Evaluator<ExprType> exprE{ expr };
		Evaluator<Dest>		destE{ dest };
	}
};


} // End impl::
