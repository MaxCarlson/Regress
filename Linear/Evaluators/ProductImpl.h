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

// TODO: Optimize both packing functions for Row/ColMajor Order
//
// Format in memory: 
// BlockSize nr = 4
// 1  2  3  4	17 18 19 20
// 5  6  7  8	21 22 23 24
// 9  10 11 12	25 26 27 28
// 13 14 15 16	29 30 31 32 
//
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


	/*
	for (Index i = 0; i < rows; ++i)
		for (Index j = maxPackedCols; j < cols ; ++j)
		{
			Type v = from.evaluate(i, j);
			*block = v;
			++block;
		}
	*/
	for (int i = 0; i < rows * cols; ++i)
	{
		if (i != 0 && i % Stride == 0) std::cout << '\n';
		std::cout << rr[i] << ", ";
	}
	std::cout << "\n\n";
}

/*
// Format in memory: 
// mr = 4
// 1 5  9 13	17 21 25 29   
// 2 6 10 14	18 22 26 30
// 3 7 11 15	19 23 27 31
// 4 8 12 16	20 24 28 32
template<class Type, class From, class Index, int mr = PackingInfo<Type>::mr>
void packTranspose(Type* block, const From& from, Index rows, Index cols)
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
	{
		std::cout << rr[i] << ", ";
		if (rows % i == 0) std::cout << '\n';
	}
	std::cout << "\n\n";
}
*/

template<class Type, class Lhs, class Index>
void packLhs(Type* blockA, const Lhs& lhs, Index rows, Index cols)
{
	packPanel(blockA, lhs, rows, cols);
	//packInorder(blockA, lhs, rows, cols);
	//packTranspose(blockA, lhs, rows, cols);
}

template<class Type, class Rhs, class Index>
void packRhs(Type* blockB, const Rhs& rhs,  Index rows, Index cols)
{
	//packTranspose(blockB, rhs, rows, cols);
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


// GEneral Block Panel
// https://www.cs.utexas.edu/users/pingali/CS378/2008sp/papers/gotoPaper.pdf
// m x n = m x k * k x n
//
// { 1,  2,  3,  4}	  { 1,  2,  3,  4} 
// { 5,  6,  7,  8}   { 5,  6,  7,  8}
// { 9, 10, 11, 12} * { 9, 10, 11, 12}
// {13, 14, 15, 16}   {13, 14, 15, 16}
//
template<class Dest, class Type, class Index, int nr = PackingInfo<Type>::nr, int mr = PackingInfo<Type>::mr>
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

	Index maxK = kc - kc % Stride; // TODO: Should be kr?

	for (Index m = 0; m < mc; ++m)
	{
		Index n			= 0;

		if (maxK > 0)
		{
			BlockPtr aPtr = &blockA[m * mc];
			Packet C = impl::pset1<Packet>(0);
			Packet tmp;

			for (Index k2 = 0; k2 < kc; ++k2)
			{
				Packet A = impl::pload1<Packet>(aPtr);
				BlockPtr bPtr = &blockB[k2 * nr]; // TODO: Should nr be kr here?
				Packet B = pload<Packet>(bPtr);

				pmadd(A, B, C, tmp);
				++aPtr;
			}

			dest.accumulate(m, n, C);
			n += 4;
		}

		if (maxK < kc)
		{
			// Finish out micro row
			Type Cval = 0;
			aPtr = &blockA[m * mc]; // -= kc
			for (Index k2 = 0; k2 < kc; ++k2)
			{
				BlockPtr bPtr = &blockB[nr * kc + k2];
				Cval += *aPtr * *bPtr;
				++aPtr;
			}

			dest.evaluateRef(m, n) += Cval;
			n++;
		}

		
		// TODO:
		for (Index k1 = 0; k1 < kc; ++k1)
		{
		}
	}

	/*
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
	*/
}


} // End impl::