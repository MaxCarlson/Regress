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
//
//

// { 1,  2,  3,  4}	  { 1,  2,  3,  4}
// { 5,  6,  7,  8}   { 5,  6,  7,  8}
// { 9, 10, 11, 12} * { 9, 10, 11, 12}
// {13, 14, 15, 16}   {13, 14, 15, 16}

// Assuming floats with SSE
//
// 1 2 3 4		1 2 3 4		1111 2222 3333 4444 5555 ...
// 5 6 7 8		5 6 7 8		1234 1234 1234 1234 5678 ...
//
// 1 2 3 4		1 5  9 13	123 4 12 3 4
// 5 6 7 8		2 6 10 14   15913 261014
//
// 1 5  9 13	1 2 3 4		
// 2 6 10 14	5 6 7 8
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

	for (Index j = sCols; j < cols; ++j)
	{
		// Cannot Pack by packet as memory is not contiguous.
		// Look into packing into temporary and using _MM_transpose
		// Pack elements by packet

		// Pack elements one at a time
		for (Index i = sRows; i < rows; ++i)
		{
			Type v = lhs.evaluate(i, j);
			*blockA = v;
			++blockA;
		}
	}
}

template<class Type, class Rhs, class Index>
void packRhs(Type* blockB, const Rhs& rhs, Index sRows, Index rows, Index sCols, Index cols)
{
	using Traits = PacketTraits<Type>;
	using Packet = typename Traits::type;
	enum
	{
		Stride = Traits::Stride
	};

	const Index maxCols = cols - cols % Stride;

	for (Index i = sRows; i < rows; ++i)
	{
		// Pack elements by packet
		for (Index j = sCols; j < maxCols; j += Stride)
		{
			Packet p = rhs.template packet<Packet>(i, j);
			pstore(blockB, p);
			blockB += Stride;
		}

		// Pack elements one at a time
		for (Index j = maxCols; j < cols; ++j)
		{
			*blockB = rhs.evaluate(i, j);
			++blockB;
		}
	}

}

// GEneral Block Product
template<class Lhs, class Rhs, class Index>
void gebp()
{

}


} // End impl::