#pragma once
#include "ForwardDeclarations.h"
#include "System\Cache.h"

namespace impl
{

// lhs * rhs * lhs
// {1, 2},					{9,  12, 15}
// {3, 4}, * {1, 2, 3}, =	{19, 26, 33}
// {5, 6},   {4, 5, 6}		{29, 40, 51}
// 
// {9,  12, 15}		{1, 2},		{120, 156}
// {19, 26, 33} *	{3, 4}, =	{262, 340}
// {29, 40, 51}		{5, 6},		{404, 524}

// Assuming doubles with SSE
// 
// 1 3   1 2   11 33 22 44
// 2 4   4 5   12 12 45 45
//		

//
// 1 2  1 2		11 22 33 44
// 3 4  4 5		12 45 12 45
//

template<class Type, class Lhs, class Index>
void packLhs(Type* start, const Lhs& lhs, Index sCols, Index cols, Index sRows, Index rows)
{
	using Traits = PacketTraits<Type>;
	enum
	{
		Stride = Traits::Stride
	};
}

// GEneral Block Product
template<class Lhs, class Rhs, class Index>
void gebp()
{

}


} // End impl::