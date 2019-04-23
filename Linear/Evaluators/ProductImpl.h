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
	c	= impl::padd(c, tmp);
}

// TODO: ploadN with variadic tuple?
template<class Packet, class Type>
inline decltype(auto) pload4(const Type* ptr)
{
	return std::tuple{ impl::pload1<Packet>(ptr + 0), impl::pload1<Packet>(ptr + 1),
		impl::pload1<Packet>(ptr + 2), impl::pload1<Packet>(ptr + 3) };
}

template<class Packet, class Type>
inline void pload4r(const Type* ptr, Packet& p0, Packet& p1, Packet& p2, Packet& p3)
{
	p0 = impl::pload1<Packet>(ptr + 0);
	p1 = impl::pload1<Packet>(ptr + 1);
	p2 = impl::pload1<Packet>(ptr + 2);
	p3 = impl::pload1<Packet>(ptr + 3);
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

template<class Type, class Index>
struct BlockSizer
{
	inline static bool init = false;
	inline static Index mc, kc, nc;

	BlockSizer()
	{
		if (init)
			return;
		init = true;

		auto l1 = cacheInfo.l1;
		auto l2 = cacheInfo.l2;
		calculateBlockA(l2);
		calculateBlockB(l1);

		int l1Rel = l1 * 0.7 / sizeof(Type);
		int l2Rel = (l2 * 0.5 - l1) / sizeof(Type);

		int m = 8;
		int k = 8;
		int n = 8;

		int stepSize = 8;

		while (true)
		{
			int mk = m * k;
			int nk = n * k;
			if (mk < l1Rel * 0.9)
			{
				//if()
			}
		}
	}

	inline void calculateBlockA(int l2)
	{

	}

	inline void calculateBlockB(int l1)
	{

	}
};

// Packing order: 
// BlockSize mr == 4
// 
// 1  5   9 13  17 21 25 29
// 2  6  10 14  18 22 26 30
// 3  7  11 15  19 23 27 31
// 4  8  12 16  20 24 28 32
// ...
// (rows - rows % mr are packed in order)
// ...
// 33 34 35 36  37 38 39 40 
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
		const Index startPNTpRows	= Stride == 4 && !std::is_same_v<Type, double> ? maxPRows : 0; // Start packed non-transposed rows
		const Index maxPCols		= cols - cols % 4;


		// TODO: Make this code Stride oblivious
		// TODO: AVX 8x8 packing ?

		if constexpr (Stride == 4 && !std::is_same_v<Type, double>) // TODO: Remove this double restriction for avx
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

		// Finish packing the rest of the rows/cols that can be packed optimally
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
		const Index maxPRows = rows - rows % 4;

		for (Index i = 0; i < maxPRows; i += mr)
		{
			for (Index j = 0; j < cols; ++j)
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
		const Type* debug		= block;
		const Index maxPCol		= cols - cols % nr;
		const bool unrollPack	= (nr == Stride && nr >= 4);
		const Index maxUrlRow	=  unrollPack ? rows - rows % nr : 0;

		for (Index j = 0; j < maxPCol; j += Stride)
		{
			if (unrollPack)
			{
				for (Index i = 0; i < maxUrlRow; i += 4)
				{
					Packet P0 = from.template loadUnaligned<Packet>(i + 0, j);
					Packet P1 = from.template loadUnaligned<Packet>(i + 1, j);
					Packet P2 = from.template loadUnaligned<Packet>(i + 2, j);
					Packet P3 = from.template loadUnaligned<Packet>(i + 3, j);

					impl::pstore(block, P0); block += Stride;
					impl::pstore(block, P1); block += Stride;
					impl::pstore(block, P2); block += Stride;
					impl::pstore(block, P3); block += Stride;
				}
			}

			for (Index i = maxUrlRow; i < rows; ++i)
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

// An even simpler wrapper class just for result values, 
// so we can prefetch now and accumulate later while only indexing in once
template<class Type, class Index>
struct IndexedValue
{
	IndexedValue(Type* ptr) noexcept :
		ptr{ ptr }
	{}

	inline void prefetch(Index idx) const noexcept { impl::prefetch(ptr + idx); }

	template<class Packet>
	inline void accumulate(const Packet& p, Index idx = 0)
	{
		impl::pstore(ptr + idx, impl::padd(impl::ploadu<Packet>(ptr + idx), p));
	}

	Type* RGR_RESTRICT ptr;
};

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

	inline IndexedValue<Type, Index> get(Index row, Index col) noexcept 
	{ 
		return IndexedValue<Type, Index>{ &evaluateRef(row, col) };
	}

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

	inline void prefetch(Index row, Index col)
	{
		impl::prefetch(&dest.evaluateRef(getRow(row, col), getCol(row, col)));
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
	using BlockPtr	= const Type* RGR_RESTRICT;
	using Indexer	= IndexedValue<Type, Index>;
	enum { Stride = Traits::Stride };

	// TODO: NOTE: mr/nr/kr are block sizes to make block fit in registers?

	// blockA size is mc * kc 
	// TODO: Fits in l2 Cache
	// blockB size is kc * nc
	// TODO: Fits in l1 Cache
	
	// Unroll step size for k
	static constexpr Index kStep = 8;

	const Index kUnroll		= kc - kc % kStep;
	const Index packedM		= mc - mc % mr;
	const Index packedN3	= 0;// nc - nc % (nr * 3);
	const Index packedN2	= nc - (nc - packedN3) % (nr * 2);
	const Index packedN		= nc - (nc - packedN2) % nr;

	// TODO: Test this with other higher level pragmas
	//#pragma omp parallel for 
	for (Index m = 0; m < packedM; m += mr)
	{
		for (Index n = packedN3; n < packedN2; n += nr * 2) 
		{
			BlockPtr aPtr	= &blockA[m * kc];
			BlockPtr bPtr	= &blockB[n * kc];
			BlockPtr bPtr2	= &blockB[n * kc + kc * nr];
			impl::prefetch(aPtr);
			impl::prefetch(bPtr);
			impl::prefetch(bPtr2);

			Packet B0, B1, tmp;
			Packet C0{ Type{ 0 } }; Packet C1{ Type{ 0 } };
			Packet C2{ Type{ 0 } }; Packet C3{ Type{ 0 } };
			Packet C4{ Type{ 0 } }; Packet C5{ Type{ 0 } };
			Packet C6{ Type{ 0 } }; Packet C7{ Type{ 0 } };

			Indexer R0 = dest.get(m + 0, n);
			Indexer R1 = dest.get(m + 1, n);
			Indexer R2 = dest.get(m + 2, n);
			Indexer R3 = dest.get(m + 3, n);
			R0.prefetch(0);
			R1.prefetch(0);
			R2.prefetch(0);
			R3.prefetch(0);

			for (Index k = 0; k < kUnroll; k += kStep)
			{
				Packet A0, A1, A2, A3; 
				// TODO: Try removing B1
#define PROCESS_STEP(K) \
				{ \
					pload4r(aPtr + K * mr, A0, A1, A2, A3);		\
					B0 = impl::pload<Packet>(bPtr  + K * nr);	\
					B1 = impl::pload<Packet>(bPtr2 + K * nr);	\
					pmadd(A0, B0, C0, tmp); \
					pmadd(A1, B0, C1, tmp); \
					pmadd(A2, B0, C2, tmp); \
					pmadd(A3, B0, C3,  B0); \
					pmadd(A0, B1, C4,  B0); \
					pmadd(A1, B1, C5,  B0); \
					pmadd(A2, B1, C6,  B0); \
					pmadd(A3, B1, C7,  B1); \
				} \

				// TODO: Run a test with differing values of prefetch distance
				// TODO: Integrate that testing with similar tests in bench
				impl::prefetch(aPtr + (32 + 0));
				PROCESS_STEP(0); 
				PROCESS_STEP(1);
				PROCESS_STEP(2); // Exactly one cache line traversed of blockA
				PROCESS_STEP(3); 
				impl::prefetch(aPtr + (32 + 16));
				PROCESS_STEP(4);
				PROCESS_STEP(5);
				PROCESS_STEP(6);
				PROCESS_STEP(7);
				static_assert(kStep == 8);

				aPtr	+= mr * kStep;
				bPtr	+= nr * kStep;
				bPtr2	+= nr * kStep;
			}

			for (Index k = kUnroll; k < kc; ++k)
			{
				Packet A0, A1, A2, A3; 
				PROCESS_STEP(0);
#undef PROCESS_STEP
				aPtr	+= mr;
				bPtr	+= nr;
				bPtr2	+= nr;
			}
			R0.accumulate(C0);
			R0.accumulate(C4, Stride);
			R1.accumulate(C1);
			R1.accumulate(C5, Stride);
			R2.accumulate(C2);
			R2.accumulate(C6, Stride);
			R3.accumulate(C3);
			R3.accumulate(C7, Stride);
		}

		for (Index n = packedN2; n < packedN; n += nr)
		{
			BlockPtr aPtr = &blockA[m * kc]; 
			BlockPtr bPtr = &blockB[n * kc];
			impl::prefetch(aPtr);
			impl::prefetch(bPtr);

			Packet B0;
			Packet tmp{ Type{ 0 } };
			Packet C0 { Type{ 0 } }; Packet C1{ Type{ 0 } };
			Packet C2 { Type{ 0 } }; Packet C3{ Type{ 0 } };

			Indexer R0 = dest.get(m + 0, n);
			Indexer R1 = dest.get(m + 1, n);
			Indexer R2 = dest.get(m + 2, n);
			Indexer R3 = dest.get(m + 3, n);
			R0.prefetch(0);
			R1.prefetch(0);
			R2.prefetch(0);
			R3.prefetch(0);

			for (Index k = 0; k < kc; ++k)
			{
				Packet A0, A1, A2, A3;
				pload4r<Packet, Type>(aPtr, A0, A1, A2, A3);
				B0 = impl::pload<Packet>(bPtr);

				pmadd(A0, B0, C0, tmp);
				pmadd(A1, B0, C1, tmp);
				pmadd(A2, B0, C2, tmp);
				pmadd(A3, B0, C3, tmp);

				aPtr += mr;
				bPtr += nr;
			}
			R0.accumulate(C0);
			R1.accumulate(C1);
			R2.accumulate(C2);
			R3.accumulate(C3);
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
				C0 += *aPtr++ * *bPtr;
				C1 += *aPtr++ * *bPtr;
				C2 += *aPtr++ * *bPtr;
				C3 += *aPtr++ * *bPtr;
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