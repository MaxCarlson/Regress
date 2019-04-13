#pragma once
#include "ForwardDeclarations.h"
#include "System\Cache.h"

#ifndef NDEBUG
#define DEBUG_BLOCKS
#endif // DEBUG

namespace impl
{

template<class Packet>
inline void pmadd(const Packet& a, const Packet& b, Packet& c, Packet& tmp)
{
	// TODO: ifdef for FMA instructions
	tmp = b;
	tmp = impl::pmul(a, tmp);
	c = impl::padd(c, tmp);
}

// TODO: ploadN with variadic tuple?
template<class Packet, class Type>
inline decltype(auto) pload4(const Type* ptr)
{
	return std::tuple{ impl::pload1<Packet>(ptr + 0), impl::pload1<Packet>(ptr + 1),
		impl::pload1<Packet>(ptr + 2), impl::pload1<Packet>(ptr + 3) };
}

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
		nr = Traits::Stride,//sizeof(void*) / 2
		mr = 4,
	};

};

// Format in memory: 
// BlockSize mr = 4
// TODO:
template<class Type, class From, class Index, bool MajorOrder, int mr>
struct PackPanel
{
	static_assert(mr == 4, "mr must be 4 for LHS packing to succeed");

	using Traits = PacketTraits<Type>;
	using Packet = typename Traits::type;
	enum
	{
		Stride = Traits::Stride
	};

	static void printblock(const Type* block, Index rows, Index cols)
	{
		for (int i = 0; i < rows * cols; ++i)
		{
			if (i != 0 && i % mr == 0) std::cout << '\n';
			std::cout << block[i] << ", ";
		}
		std::cout << "\n\n";
	}

	template<bool MajorOrder>
	static void run(Type* block, const From& from, Index rows, Index cols) { static_assert(false); }

	template<> static void run<RowMajor>(Type* block, const From& from, Index rows, Index cols)
	{
		const Type* debug			= block;
		const Index maxPRows		= rows - rows % 4;
		const Index startPNTpRows	= Stride == 4 ? maxPRows : 0; // Start packed non-transposed rows
		const Index maxPCols		= cols - cols % 4;


		// TODO: Make this code Stride oblivious

		if constexpr (Stride == 4)
		{
			for (Index i = 0; i < maxPRows; i += mr)
			{
				for (Index j = 0; j < maxPCols; j += 4)
				{
					Packet P0 = from.loadUnaligned<Packet>(i + 0, j);
					Packet P1 = from.loadUnaligned<Packet>(i + 1, j);
					Packet P2 = from.loadUnaligned<Packet>(i + 2, j);
					Packet P3 = from.loadUnaligned<Packet>(i + 3, j);

					impl::transpose4x4(P0, P1, P2, P3);

					pstore(block, P0); block += 4;
					pstore(block, P1); block += 4;
					pstore(block, P2); block += 4;
					pstore(block, P3); block += 4;
				}

				// Finish out columns
				for (Index j = maxPCols; j < cols; ++j)
				{
					const Index maxI = std::min(maxPRows, i + mr);
					for (Index i2 = i; i2 < maxI; ++i2)
					{
						Type v = from.evaluate(i2, j);
						*block = v;
						++block;
					}
				}
			}
		}

		for (Index i2 = startPNTpRows; i2 < maxPRows; i2 += mr)
		{
			for (Index j = 0; j < cols; ++j)
			{
				const Index maxI = std::min(maxPRows, i2 + mr);
				for (Index i = i2; i < maxI; ++i)
				{
					Type v = from.evaluate(i, j);
					*block = v;
					++block;
				}
			}
		}

		// Pack last rows inorder
		for (Index i = maxPRows; i < rows; ++i)
		{
			for (Index j = 0; j < cols; ++j)
			{
				Type v = from.evaluate(i, j);
				*block = v;
				++block;
			}
		}

#ifdef DEBUG_BLOCKS
		printblock(debug, rows, cols);
#endif // DEBUG_BLOCKS
	}

	template<> static void run<ColMajor>(Type* block, const From& from, Index rows, Index cols)
	{
		const Type* debug = block;
		const Index maxPackedCols = cols - cols % Stride;

		//*
		for (Index i = 0; i < rows; ++i)
		{
			for (Index j = 0; j < cols; ++j)
			{
				Type v = from.evaluate(i, j);
				*block = v;
				++block;
			}
		}
		//*/

#ifdef DEBUG_BLOCKS
		printblock(debug, rows, cols);
#endif // DEBUG_BLOCKS
	}
};


// TODO:
// Format in memory: 
// BlockSize nr = 4
//
// Note: nr is number of columns from rhs per packed op
template<class Type, class From, class Index, bool MajorOrder, int nr>
struct PackBlock
{
	using Traits = PacketTraits<Type>;
	using Packet = typename Traits::type;
	enum
	{
		Stride = Traits::Stride
	};

	static void printblock(const Type* block, Index rows, Index cols)
	{
		for (int i = 0; i < rows * cols; ++i)
		{
			if (i != 0 && i % nr == 0) std::cout << '\n';
			std::cout << block[i] << ", ";
		}
		std::cout << "\n\n"; 
	}

	template<int MajorOrder>
	static void run(Type* block, const From& from, Index rows, Index cols) { static_assert(false); }

	template<> static void run<RowMajor>(Type* block, const From& from, Index rows, Index cols)
	{
		const Type* debug = block;
		const Index maxPCol = cols - cols % nr;

		for (Index j = 0; j < maxPCol; j += Stride)
		{
			for (Index i = 0; i < rows; ++i)
			{
				Packet p = from.template loadUnaligned<Packet>(i, j);
				impl::pstore(block, p);
				block += Stride;
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
#ifdef DEBUG_BLOCKS
		printblock(debug, rows, cols);
#endif // DEBUG_BLOCKS
	}

	template<> static void run<ColMajor>(Type* block, const From& from, Index rows, Index cols)
	{
		const Type* debug = block;
		const Index maxPCol = cols - cols % nr;

		for (Index j = 0; j < maxPCol; j += nr)
		{
			for (Index i = 0; i < rows; ++i)
			{
				for (Index h = 0; h < nr; ++h)
				{
					Type v = from.evaluate(i, j + h);
					*block = v;
					++block;
				}
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
#ifdef DEBUG_BLOCKS
		printblock(debug, rows, cols);
#endif // DEBUG_BLOCKS
	}
};

template<class Type, class Lhs, class Index>
void packLhs(Type* blockA, const Lhs& lhs, Index rows, Index cols)
{
	//std::cout << "Packing Lhs \n";
	using packer = PackPanel<Type, Lhs, Index, Lhs::MajorOrder, PackingInfo<Type>::mr>;
	packer::run<Lhs::MajorOrder>(blockA, lhs, rows, cols);
}

template<class Type, class Rhs, class Index>
void packRhs(Type* blockB, const Rhs& rhs,  Index rows, Index cols)
{
	//std::cout << "Packing Rhs \n";
	using packer = PackBlock<Type, Rhs, Index, Rhs::MajorOrder, PackingInfo<Type>::nr>;
	packer::run<Rhs::MajorOrder>(blockB, rhs, rows, cols);
}

// Simple wrapper class to make indexing inside gebp easier
template<class Dest, class Index, bool Transpose>
struct IndexWrapper
{
	using Type = typename Dest::value_type;
	enum
	{
		Transposed = Transpose,
		MajorOrder = Transposed ? !Dest::MajorOrder : Dest::MajorOrder,
	};

	IndexWrapper(Dest& dest, Index r, Index c) :
		dest{ dest },
		r{ r }, c{ c }
	{}

	inline Index getRow(Index row, Index col) const { return Transpose ? col + c : row + r; }
	inline Index getCol(Index row, Index col) const { return Transpose ? row + r : col + c; }

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
	inline void accumulate(Index row, Index col, const Packet& p) 
	{
		Packet r = loadUnaligned<Packet>(row, col);
		writePacket<Packet>(row, col, impl::padd(r, p));
	}

private:
	Dest& RGR_RESTRICT dest;
	Index r, c;
};


// GEneral Block Panel 
// https://www.cs.utexas.edu/users/pingali/CS378/2008sp/papers/gotoPaper.pdf
// m x n = m x k * k x n
//
// { 1,  2,  3,  4}	  { 1,  2,  3,  4} 
// { 5,  6,  7,  8}   { 5,  6,  7,  8}
// { 9, 10, 11, 12} * { 9, 10, 11, 12}
// {13, 14, 15, 16}   {13, 14, 15, 16}
//
template<class Dest, class Type, class Index, int mr = PackingInfo<Type>::mr, int nr = PackingInfo<Type>::nr>
void gebp(Dest& dest, const Type* blockA, const Type* blockB, const Index mc, const Index nc, const Index kc)
{
	using Traits	= PacketTraits<Type>;
	using Packet	= typename Traits::type;
	using BlockPtr	= const Type*;
	enum { Stride = Traits::Stride };

	// TODO: Loop unrolling
	// TODO: Prefetching
	// TODO: NOTE: mr/nr/kr are block sizes to make block fit in registers?

	// blockA size is mc * kc 
	// Fits in l2 Cache
	// blockB size is kc * nc
	// Fits in l1 Cache

	const Index packedM = mc - mc % mr;
	const Index packedN = nc - nc % nr;

	//const Index

	for (Index m = 0; m < packedM; m += mr)
	{
		for (Index n = 0; n < packedN; n += nr)
		{
			BlockPtr aPtr = &blockA[m * kc]; // TODO: BUG: Current bug appears to come from indexing past lhs block here
			BlockPtr bPtr = &blockB[n * kc];

			Packet tmp{ Type{ 0 } };
			Packet C0 { Type{ 0 } }; Packet C1{ Type{ 0 } };
			Packet C2 { Type{ 0 } }; Packet C3{ Type{ 0 } };

			for (Index k = 0; k < kc; ++k)
			{
				auto[A0, A1, A2, A3] = pload4<Packet, Type>(aPtr);

				Packet B0 = impl::pload<Packet>(bPtr);

				pmadd(A0, B0, C0, tmp);
				pmadd(A1, B0, C1, tmp);
				pmadd(A2, B0, C2, tmp);
				pmadd(A3, B0, C3, tmp);

				aPtr += mr;
				bPtr += nr;
			}
			dest.accumulate(m + 0, n, C0);
			dest.accumulate(m + 1, n, C1);
			dest.accumulate(m + 2, n, C2);
			dest.accumulate(m + 3, n, C3);
		}

		// TODO: This is just simple element by element but can be made
		// faster (do once main packed algorithm is optimized)
		for (Index n = packedN; n < nc; ++n)
		{
			Type C0{ 0 }; Type C1{ 0 };
			Type C2{ 0 }; Type C3{ 0 };
			BlockPtr aPtr = &blockA[m * kc];
			BlockPtr bPtr = &blockB[n * kc];

			for (Index k = 0; k < kc; ++k)
			{
				C0 += *(aPtr + 0) * *bPtr;
				C1 += *(aPtr + 1) * *bPtr;
				C2 += *(aPtr + 2) * *bPtr;
				C3 += *(aPtr + 3) * *bPtr;
				aPtr += mr;
				++bPtr;
			}
			dest.evaluateRef(m + 0, n) += C0;
			dest.evaluateRef(m + 1, n) += C1;
			dest.evaluateRef(m + 2, n) += C2;
			dest.evaluateRef(m + 3, n) += C3;
		}
	}

	// TODO: This can be optimized
	// Finish out the last row/s of lhs that couldn't be packed (in optimal scheme)
	for (Index m = packedM; m < mc; ++m)
	{
		// Finish muling the leftover rows of lhs and 
		// all columns of rhs that are packed normally
		for (Index n = 0; n < packedN; n += nr)
		{
			Index offsetJ = 0;
			for (Index j = n; j < n + nr; j += 2, offsetJ += 2) // TODO: Make sure nr % 2 == 0
			{
				BlockPtr aPtr = &blockA[m * kc];
				BlockPtr bPtr = &blockB[n * kc + offsetJ];

				Type C0{ 0 }; Type C1{ 0 };

				for (Index k = 0; k < kc; ++k)
				{
					C0 += *aPtr * *(bPtr + 0);
					C1 += *aPtr * *(bPtr + 1);
					++aPtr;
					bPtr += nr;
				}

				dest.evaluateRef(m, n + offsetJ + 0) += C0;
				dest.evaluateRef(m, n + offsetJ + 1) += C1;
			}
		}

		// Finish the last rows of lhs * the non-normally packed columns of rhs
		for (Index n = packedN; n < nc; ++n)
		{
			BlockPtr aPtr = &blockA[m * kc];
			BlockPtr bPtr = &blockB[n * kc];
			Type C0{ 0 };

			for (Index k = 0; k < kc; ++k)
			{
				C0 += *aPtr * *bPtr;
				++aPtr;
				++bPtr;
			}
			dest.evaluateRef(m, n) += C0;
		}
	}
}


} // End impl::