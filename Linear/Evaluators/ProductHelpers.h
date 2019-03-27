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
// 1 2  1 2		11 22 33 44
// 3 4  4 5		12 45 12 45
//

template<class Type, class Lhs, class Index>
void packLhs(Type* blockA, const Lhs& lhs, Index sRows, Index rows, Index sCols, Index cols)
{
	using Traits	= PacketTraits<Type>;
	using Packet	= typename Traits::type;
	enum
	{
		Stride = Traits::Stride
	};

	// TODO: Specialize for MajorOrder
	// TODO: Loop unrolling

	Index idx = 0;
	const Index maxCols = cols - cols % Stride;

	for (Index i = sRows; i < rows; ++i)
	{
		// Pack elements by packet
		for (Index j = sCols; j < maxCols; j += Stride)
		{
			Packet p = lhs.template packet<Packet>(i, j);
			pstore(blockA, p);
			blockA += Stride;
		}

		// Pack elements one at a time
		for (Index j = maxCols; j < cols; ++j)
		{
			*blockA = lhs.evaluate(i, j);
			++blockA;
		}
	}
}

template<class Type, class Lhs, class Index>
void packRhs(Type* blockB, const Lhs& lhs, Index sCols, Index cols, Index sRows, Index rows)
{

}

// GEneral Block Product
template<class Lhs, class Rhs, class Index>
void gebp()
{

}


} // End impl::