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

// TODO: Optimize both packing functions for Row/ColMajor Order
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

	const Index maxCols			= cols;
	const Index maxPCols		= cols - cols % Stride;

	for (Index i = 0; i < rows; ++i)
	{
		// Pack elements by packet
		Index j = 0;
		for (; j < maxPCols; j += Stride)
		{
			Packet p = from.template packet<Packet>(i, j);
			pstore(block, p);
			block += Stride;
		}

		// Pack elements one at a time
		for (; j < maxCols; ++j)
		{
			*block = from.evaluate(i, j);
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

	// TODO: Loop unrolling

	for (Index j = 0; j < cols; ++j)
	{
		// Cannot Pack by packet as memory is not contiguous.
		// Look into packing into temporary and using _MM_transpose
		// Pack elements by packet

		// Pack elements one at a time
		for (Index i = 0; i < rows; ++i)
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
	packTranspose(blockA, lhs, sRows, rows, sCols, cols);
}

template<class Type, class Rhs, class Index>
void packRhs(Type* blockB, const Rhs& rhs, Index sRows, Index rows, Index sCols, Index cols)
{
	packInorder(blockB, rhs, sRows, rows, sCols, cols);
}

// Simple wrapper class to make indexing inside gebp easier
template<class Dest, class Index, bool Order>
struct IndexWrapper
{
	using Type = typename Dest::value_type;
	enum { MajorOrder = Dest::MajorOrder };

	IndexWrapper(Dest& dest, Index r, Index c) :
		dest{ dest },
		r{ r }, c{ c }
	{}

	inline Type evaluate(Index row, Index col) const
	{
		if(Order)
			return dest.evaluate(row + r, col + c);
		else
			return dest.evaluate(col + c, row + r);
	}

	inline Type& evaluateRef(Index row, Index col)
	{
		if(Order)
			return dest.evaluateRef(row + r, col + c);
		else
			return dest.evaluateRef(col + c, row + r);
	}

	template<class Packet>
	inline Packet packet(Index row, Index col) const 
	{
		if(Order)
			return dest.template packet<Packet>(row + r, col + c);
		else
			return dest.template packet<Packet>(col + c, row + r);
	}

	template<class Packet>
	inline void writePacket(Index row, Index col, const Packet& p)
	{
		if (Order)
			dest.template writePacket<Packet>(row + r, col + c, p);
		else
			dest.template writePacket<Packet>(col + c, row + r, p);
	}

private:
	Dest& dest;
	Index r, c;
};

template<class Packet>
RGR_FORCE_INLINE void pmadd(const Packet& a, const Packet& b, Packet& c, Packet& tmp)
{
	// TODO: ifdef for FMA instructions
	//tmp = b;
	tmp = impl::pmul(a, b);
	c = impl::padd(c, tmp);
}


// GEneral Block Panel
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
// lhs'			rhs (RowMajor)
// BlockA		BlockB		
// 1 5  9 13	1 2 3 4		1111 5555
// 2 6 10 14	5 6 7 8		1234 1234
//						
// rhs			lhs' (ColumnMajor)
// 1 5  9 13	1 2 3 4		
// 2 6 10 14	5 6 7 8	
template<class Dest, class Type, class Index>
void gebp(Dest& dest, const Type* blockA, const Type* blockB, Index mc, Index nc, Index kc)
{
	using Traits	= PacketTraits<Type>;
	using Packet	= typename Traits::type;
	using BlockPtr	= const Type*;
	enum { Stride = Traits::Stride };

	// TODO: Loop unrolling
	// TODO: NOTE: mr/nr/kr are register block sizes

	// blockA size is mc * kc 
	// Fits in l2 Cache
	// blockB size is kc * nc
	// Fits in l1 Cache

	const Index maxN = nc - nc % Stride;

	// Loop through blockA (rows of lhs)
	for (Index k = 0; k < kc; ++k)
	{
		// Loop through blockA (cols of lhs)
		for (Index m = 0; m < mc; ++m)
		{
			BlockPtr aPtr = &blockA[k * mc + m];
			BlockPtr bPtr = &blockB[k * nc];

			Packet tmp;
			Packet A	= impl::pbroadcast<Packet>(aPtr);
			Type Aval	= *aPtr;

			// Evaluate packet by packet (SIMD)
			for (Index n = 0; n < maxN; n += Stride)
			{
				Packet C = dest.template packet<Packet>(m, n);
				Packet B = pload<Packet>(bPtr);

				pmadd(A, B, C, tmp);
				dest.template writePacket<Packet>(m, n, C);
				bPtr += Stride;
			}

			// Evaluate element by element
			for (Index n = maxN; n < nc; ++n)
			{
				// TODO/BUG: This could easily be wrong here
				Type C = dest.evaluate(m, n);
				Type B = *bPtr;
				dest.evaluateRef(m, n) = C + Aval * B;

				++bPtr;
			}
		}
	}
}


} // End impl::