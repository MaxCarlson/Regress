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
// 1  2  3  4	17 18 19 20
// 5  6  7  8	21 22 23 24
// 9  10 11 12	25 26 27 28
// 13 14 15 16	29 30 31 32 
//
template<class Type, class From, class Index, int nr = PacketTraits<Type>::Stride>
void packInorder(Type* block, const From& from, Index r, Index rows, Index c, Index cols)
{
	using Traits = PacketTraits<Type>;
	using Packet = typename Traits::type;
	enum
	{
		Stride = Traits::Stride
	};

	// DELETE THIS
	const Type* rr = block;

	// Columns after this col will be stored in transposed order
	const Index maxPCol = cols - cols % nr;

	for (Index j = 0; j < maxPCol; j += nr)
	{
		for (Index i = 0; i < rows; ++i)
		{
			Packet p = from.template packet<Packet>(i, j);
			pstore(block, p);
			block += nr;
		}
	}

	// Pack last cols transposed
		for (Index j = maxPCol; j < cols; ++j)
			for (Index i = 0; i < rows; ++i)
			{
				Type v = from.evaluate(i, j);
				*block = v;
				++block;
			}

	for (int i = 0; i < rows * cols; ++i)
		std::cout << rr[i] << ", ";
	std::cout << "\n\n";
}

// Format in memory: 
// BlockSize 4x4
// 1 5  9 13
// 2 6 10 14
// 3 7 11 15
// 4 8 12 16
template<class Type, class From, class Index, int mr = 4>
void packTranspose(Type* block, const From& from, Index r, Index rows, Index c, Index cols)
{
	using Traits = PacketTraits<Type>;
	using Packet = typename Traits::type;
	enum
	{
		Stride = Traits::Stride
	};

	// TODO: Loop unrolling
	
	// DELETE THIS
	const Type* rr = block;

	const Index packedRows		= rows / mr + 1;
	const Index lastPackedRow	= rows - rows % mr; // Rows after this row will be stored inorder

	// Pack elements in transposed blocks
	for (Index ri = 1; ri < packedRows; ++ri)
	{
		for (Index j = 0; j < cols; ++j)
		{
			// Cannot Pack by packet as memory is not contiguous.
			// Look into packing into temporary and using _MM_transpose
			for (Index i = 0; i < mr; ++i)
			{
				Type v = from.evaluate(i * ri, j);
				*block = v;
				++block;
			}
		}
	}

	// Finish out packing elements in order
	for (Index i = lastPackedRow; i < rows; ++i)
		for (Index j = 0; j < cols; ++j)
		{
			*block = from.evaluate(i, j);
			++block;
		}

	for (int i = 0; i < rows * cols; ++i)
		std::cout << rr[i] << ", ";
	std::cout << "\n\n";
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
template<class Dest, class Index, bool Transpose>
struct IndexWrapper
{
	using Type = typename Dest::value_type;
	enum { MajorOrder = Dest::MajorOrder };

	IndexWrapper(Dest& dest, Index r, Index c) :
		dest{ dest },
		r{ r }, c{ c }
	{}

	RGR_FORCE_INLINE Index getRow(Index row, Index col) const { return Transpose ? col + c : row + r; }
	RGR_FORCE_INLINE Index getCol(Index row, Index col) const { return Transpose ? row + r : col + c; }

	inline Type evaluate(Index row, Index col) const
	{
		return dest.evaluate(getRow(row, col), getCol(row, col));
	}

	inline Type& evaluateRef(Index row, Index col)
	{
		return dest.evaluateRef(getRow(row, col), getCol(row, col));
	}

	template<class Packet>
	inline Packet packet(Index row, Index col) const 
	{
		return dest.template packet<Packet>(getRow(row, col), getCol(row, col));
	}

	template<class Packet>
	inline Packet loadUnaligned(Index row, Index col) const
	{
		return dest.template packet<Packet, false>(getRow(row, col), getCol(row, col));
	}

	template<class Packet>
	inline void writePacket(Index row, Index col, const Packet& p)
	{
		dest.template writePacket<Packet>(getRow(row, col), getCol(row, col), p);
	}

	template<class Packet>
	inline void accumulate(Index row, Index col, const Packet& p) const
	{
		Packet r = loadUnaligned<Packet>(row, col);
		writePacket<Packet>(row, col, impl::padd(r, p));
	}

private:
	Dest& RGR_RESTRICT dest;
	Index r, c;
};

template<class Packet>
inline void pmadd(const Packet& a, const Packet& b, Packet& c, Packet& tmp)
{
	// TODO: ifdef for FMA instructions
	tmp = b;
	tmp = impl::pmul(a, tmp);
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
// 1 2 3 4		1 5  9 13
// 5 6 7 8		2 6 10 14
template<class Dest, class Type, class Index>
void gebp(Dest& dest, const Type* blockA, const Type* blockB, Index mc, Index nc, Index kc)
{
	using Traits	= PacketTraits<Type>;
	using Packet	= typename Traits::type;
	using BlockPtr	= const Type* RGR_RESTRICT; // TODO: Benchmark
	enum { Stride = Traits::Stride };

	// TODO: Loop unrolling
	// TODO: NOTE: mr/nr/kr are block sizes to make block fit in registers?
	// TODO: Is this gepb instead?
	// TODO: Look into using sentinals?

	// blockA size is mc * kc 
	// Fits in l2 Cache
	// blockB size is kc * nc
	// Fits in l1 Cache

	const Index maxPackedN	= nc - nc % Stride;
	const Index unroll1 = 0;// maxPackedN - maxPackedN % (4 * Stride);

	// Loop through blockA (rows of lhs)
	for (Index k = 0; k < kc; ++k)
	{
		// Loop through blockA (cols of lhs)
		for (Index m = 0; m < mc; ++m)
		{
			BlockPtr aPtr = &blockA[k * mc + m];
			BlockPtr bPtr = &blockB[k * nc];

			Packet tmp;
			Packet A	= impl::pload1<Packet>(aPtr); //impl::pbroadcast<Packet>(aPtr); 
			Type Aval	= *aPtr;

			// Evaluate packet by packet (SIMD)
			for (Index n = unroll1; n < maxPackedN; n += Stride)
			{
				Packet B = impl::ploadu<Packet>(bPtr);
				Packet C = dest.template loadUnaligned<Packet>(m, n);

				pmadd(A, B, C, tmp);
				dest.template writePacket<Packet>(m, n, C);
				bPtr += Stride;
			}

			// Evaluate element by element
			for (Index n = maxPackedN; n < nc; ++n)
			{
				Type B = *bPtr;
				dest.evaluateRef(m, n) += Aval * B;
				++bPtr;
			}
		}
	}
}


} // End impl::