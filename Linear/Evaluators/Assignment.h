#pragma once
#include "ForwardDeclarations.h"

namespace impl
{

template<class... Args>
struct Assignment {};

template<class Dest, class Lhs, class Rhs, class Type> // TODO: Specilaize for +=, etc
struct Assignment<Dest, ProductOp<Lhs, Rhs>, Type>
{
	using ExprType = ProductOp<Lhs, Rhs>;

	inline static void run(Dest& dest, const ExprType& expr)
	{
		Evaluator<ExprType> exprE{ expr };
		Evaluator<Dest>		destE{ dest };
	}
};



} // End impl::
