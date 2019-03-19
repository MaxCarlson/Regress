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

template<class Dest, class Lhs, class Rhs, class Type> // TODO: Specilaize for +=, etc
struct Assignment<Dest, ProductOp<Lhs, Rhs>, Type>
{
	using ExprType = ProductOp<Lhs, Rhs>;

	inline static void run(Dest& dest, const ExprType& expr)
	{
		Evaluator<ExprType> exprE{ expr };
		//Evaluator<Dest>		destE{ dest };
		
		
		if (Evaluator<ExprType>::MajorOrder != Dest::MajorOrder)
			; // TODO: Add code to automatically transpose exprE if it's MajorOrder does not match!
		else
			dest = exprE.moveMatrix();
	}
};



} // End impl::
