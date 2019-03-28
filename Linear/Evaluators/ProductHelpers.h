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

// Format in memory: 
// BlockSize 4x4
// 1  2  3  4 
// 5  6  7  8
// 9  10 11 12
// 13 14 15 16
template<class Type, class From, class Index>
void packInorder(Type* block, const From& from, Index r, Index rows, Index c, Index cols)
{
	using Traits = PacketTraits<Type>;
	using Packet = typename Traits::type;
	enum
	{
		Stride = Traits::Stride
	};

	const Index maxCols = cols - cols % Stride;

	for (Index i = r; i < rows; ++i)
	{
		// Pack elements by packet
		for (Index j = c; j < maxCols; j += Stride)
		{
			Packet p = from.template packet<Packet>(i, j);
			pstore(blockB, p);
			block += Stride;
		}

		// Pack elements one at a time
		for (Index j = maxCols; j < cols; ++j)
		{
			*block = rhs.evaluate(i, j);
			++block;
		}
	}
}

// Format in memory: 
// BlockSize 4x4
// 1 5  9 13
// 2 6 10 14
// 3 7 11 15
// 4 8 12 16
template<class Type, class From, class Index>
void packTranspose(Type* block, const From& from, Index r, Index rows, Index c, Index cols)
{
	using Traits = PacketTraits<Type>;
	using Packet = typename Traits::type;
	enum
	{
		Stride = Traits::Stride
	};

	// TODO: Specialize for MajorOrder
	// TODO: Loop unrolling

	for (Index j = c; j < cols; ++j)
	{
		// Cannot Pack by packet as memory is not contiguous.
		// Look into packing into temporary and using _MM_transpose
		// Pack elements by packet

		// Pack elements one at a time
		for (Index i = r; i < rows; ++i)
		{
			Type v = from.evaluate(i, j);
			*block = v;
			++block;
		}
	}
}

template<class Type, class Lhs, class Index>
void packLhs(Type* blockA, const Lhs& lhs, Index sRows, Index rows, Index sCols, Index cols)
{
	packInorder(blockA, lhs, sRows, rows, sCols, cols);
}

template<class Type, class Rhs, class Index>
void packRhs(Type* blockB, const Rhs& rhs, Index sRows, Index rows, Index sCols, Index cols)
{
	packTranspose(blockB, rhs, sRows, rows, sCols, cols);
}


// Simple wrapper class to make indexing inside gebp easier
template<class Dest, class Index>
struct GebpIndexWrapper
{
	using Type = typename Dest::value_type;

	GebpIndexWrapper(Dest& dest, Index m, Index n, Index k) :
		dest{ dest },
		m{ m }, n{ n }, k{ k }
	{}

	inline Type& coeff(Index row, Index col)
	{
		return dest.evaluateRef(row + m, col + n);
	}

	template<class Packet>
	inline void writePacket(Index row, Index col, const Packet& p)
	{
		dest.template writePacket<Packet>(row + m, col + n, p);
	}

private:
	Dest& dest;
	Index m, n, k;
};


// GEneral Block Product
// https://www.cs.utexas.edu/users/pingali/CS378/2008sp/papers/gotoPaper.pdf
// m x n = m x k * k x n
//
// { 1,  2,  3,  4}	  { 1,  2,  3,  4}
// { 5,  6,  7,  8}   { 5,  6,  7,  8}
// { 9, 10, 11, 12} * { 9, 10, 11, 12}
// {13, 14, 15, 16}   {13, 14, 15, 16}
//
// Example input format of above matrix 
// Assuming floats with SSE
// BlockA		BlockB		ops
// 1 2 3 4		1 5  9 13
// 5 6 7 8		2 6 10 14
//
//
//
//
template<class Dest, class Type, class Index>
void gebp(Dest& dest, Type* blockA, Type* blockB, Index mc, Index nc, Index kc)
{
	// blockA size is mc * kc 
	// Fits in l2 Cache

	// blockB size is kc * nc
	// Fits in l1 Cache



}


} // End impl::