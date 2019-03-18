#pragma once
#include "ForwardDeclarations.h"

namespace impl
{

template<class... Args>
struct Assignment {};

template<class Dest, class Lhs, class Rhs, class Type> // TODO: Specilaize for +=, etc
struct Assignment<Dest, ProductOp<Lhs, Rhs>, Type>
{
	static void run(Dest& dest, const Lhs& lhs, const Rhs& rhs)
	{

	}
};



} // End impl::
