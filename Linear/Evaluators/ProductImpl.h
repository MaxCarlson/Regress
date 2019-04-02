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
//
//	1,  2,  3,  4,  5 		1,  2,  3,  4,  5 	
//	6,  7,  8,  9,  10		6,  7,  8,  9,  10
//	11, 12, 13, 14, 15	*	11, 12, 13, 14, 15
//	16, 17, 18, 19, 20		16, 17, 18, 19, 20
//	21, 22, 23, 24, 25		21, 22, 23, 24, 25

template<class Type>
struct PackingInfo
{
	using Traits = PacketTraits<Type>;
	using Packet = typename Traits::type;

	enum
	{
		nr = Traits::Stride,
		mr = nr //sizeof(void*) / 2,
	};

};

template<class Type, class From, class Index, int mr = PackingInfo<Type>::mr>
void packPanel(Type* block, const From& from, Index rows, Index cols)
{
	using Traits = PacketTraits<Type>;
	using Packet = typename Traits::type;
	enum
	{
		Stride = Traits::Stride
	};

	// DELETE THIS when done printing
	const Type* rr = block;

	const Index maxPackedCols = cols - cols % Stride;

	for (Index i = 0; i < rows; ++i)
	{
		for (Index j = 0; j < maxPackedCols; j += Stride)
		{
			Packet p = from.template packet<Packet>(i, j);
			pstore(block, p);
			block += Stride;
		}

		for (Index j = maxPackedCols; j < cols; ++j)
		{
			Type v = from.evaluate(i, j);
			*block = v;
			++block;
		}
	}

	for (int i = 0; i < rows * cols; ++i)
	{
		if (i != 0 && i % Stride == 0) std::cout << '\n';
		std::cout << rr[i] << ", ";
	}
	std::cout << "\n\n";
}


// TODO: Optimize both packing functions for Row/ColMajor Order
//
// Format in memory: 
// BlockSize nr = 4
// 1  2  3  4	17 18 19 20
// 5  6  7  8	21 22 23 24
// 9  10 11 12	25 26 27 28
// 13 14 15 16	29 30 31 32 
//
// Note: nr is number of columns from rhs per packed op
template<class Type, class From, class Index, int nr = PackingInfo<Type>::nr>
void packBlock(Type* block, const From& from, Index rows, Index cols)
{
	using Traits = PacketTraits<Type>;
	using Packet = typename Traits::type;
	enum
	{
		Stride = Traits::Stride
	};

	// DELETE THIS when done printing
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
	{
		if (i != 0 && i % nr == 0) std::cout << '\n';
		std::cout << rr[i] << ", ";
	}
	std::cout << "\n\n";
}

template<class Type, class Lhs, class Index>
void packLhs(Type* blockA, const Lhs& lhs, Index rows, Index cols)
{
	packPanel(blockA, lhs, rows, cols);
}

template<class Type, class Rhs, class Index>
void packRhs(Type* blockB, const Rhs& rhs,  Index rows, Index cols)
{
	packBlock(blockB, rhs, rows, cols);
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
		dest.template writePacket<Packet>(row, col, impl::padd(r, p));
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
	c	= impl::padd(c, tmp);
}


// GEneral Panel Block (Mirror of gebp and done in Row Major Order)
// https://www.cs.utexas.edu/users/pingali/CS378/2008sp/papers/gotoPaper.pdf
// m x n = m x k * k x n
//
// { 1,  2,  3,  4}	  { 1,  2,  3,  4} 
// { 5,  6,  7,  8}   { 5,  6,  7,  8}
// { 9, 10, 11, 12} * { 9, 10, 11, 12}
// {13, 14, 15, 16}   {13, 14, 15, 16}
//
template<class Dest, class Type, class Index, int nr = PackingInfo<Type>::nr, int mr = PackingInfo<Type>::mr>
void gepb(Dest& dest, const Type* blockA, const Type* blockB, Index mc, Index nc, Index kc)
{
	using Traits	= PacketTraits<Type>;
	using Packet	= typename Traits::type;
	using BlockPtr	= const Type* RGR_RESTRICT; // TODO: Benchmark
	enum { Stride = Traits::Stride };

	// TODO: Loop unrolling
	// TODO: Prefetching
	// TODO: NOTE: mr/nr/kr are block sizes to make block fit in registers?


	// blockA size is mc * kc 
	// Fits in l2 Cache
	// blockB size is kc * nc
	// Fits in l1 Cache

	const Index maxPackedN			= nc - nc % nr;				// number of packed rhs cols
	const Index columnsPerPacket	= maxPackedN / Stride;		// 

	for (Index m = 0; m < mc; ++m)
	{
		BlockPtr bPtr = &blockB[0];

		for (Index n = 0; n < maxPackedN; n += Stride)
		{
			Packet tmp;
			Packet C		= impl::pset1<Packet>(Type{ 0 });
			BlockPtr aPtr	= &blockA[m * kc];

			for (Index k = 0; k < kc; ++k)
			{
				//BlockPtr bPtr	= &blockB[k * nr + n * kc]; // TODO: Should nr be kr here?
				Packet A		= impl::pload1<Packet>(aPtr);
				Packet B		= pload<Packet>(bPtr);

				pmadd(A, B, C, tmp);
				++aPtr;
				bPtr += Stride;
			}
			dest.accumulate(m, n, C);
		}
		
		// Handle the column/s of RHS we were unable to pack
		for (Index n = maxPackedN; n < nc; ++n)
		{
			// Finish out micro row
			Type Cval		= 0;
			BlockPtr aPtr	= &blockA[m * kc]; 
			for (Index k = 0; k < kc; ++k)
			{
				//BlockPtr bPtr = &blockB[nr * kc * + k];
				Cval += *aPtr * *bPtr;
				++aPtr;
				++bPtr;
			}
			dest.evaluateRef(m, n) += Cval;
		}

	}
}


} // End impl::