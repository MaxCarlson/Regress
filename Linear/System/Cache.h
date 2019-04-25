#pragma once
#include "ForwardDeclarations.h"
#include <intrin.h>

namespace impl
{

#define CPUID(info, f, sf) __cpuidex(info, f, sf)

struct CacheInfo
{
	size_t l1, l2, l3;

	CacheInfo() :
		l1{ 32 * 1024 },
		l2{ 256 * 1024 },
		l3{ 4096 * 1024 } // Default sizes
	{
		// Registers EAX, EBX, ECX, EDX
		int abcd[4];

		// TODO: AMD
		int GenuineIntel[] = { 0x756E6547, 0x49656E69, 0x6C65746E };

		// Get processor vendor ID string
		CPUID(abcd, 0x00, 0);

		if (isIntel(abcd, GenuineIntel))
			calcIntel();
	}

private:
	bool isIntel(const int* abcd, const int* intel) const
	{
		return abcd[1] == intel[0] && abcd[2] == intel[2] && abcd[3] == intel[1];
	}

	void calcIntel()
	{
		int abcd[4];

		int cacheType = 0;
		for (int cid = 0; cid < 16; ++cid)
		{
			abcd[0] = abcd[1] = abcd[2] = abcd[3] = 0;

			CPUID(abcd, 0x04, cid);

			int cacheType = abcd[0] & 0x0F;
			if (cacheType == 1 || cacheType == 3) // Data Cache or Unified Cache
			{
				// From Intel Manual
				//Cache Size in Bytes	= (Ways + 1) ×(Partitions + 1) ×(Line Size + 1) ×(Sets + 1) 
				//						= (EBX[31:22] + 1) ×(EBX[21:12] + 1) ×(EBX[11:0] + 1 ×(ECX + 1)

				int cacheLevel		= (abcd[0] & 0xE0) >> 5;
				unsigned int ways	= abcd[1] >> 22;
				unsigned int parts	= (abcd[1] & 0x003FF000) >> 12;
				unsigned int lines	= (abcd[1] & 0x00000FFF);
				unsigned int sets	= abcd[2];

				auto cacheSize = (ways + 1) * (parts + 1) * (lines + 1) * (sets + 1);

				switch (cacheLevel)
				{
				case 1:
					l1 = cacheSize; break;
				case 2:
					l2 = cacheSize; break;
				case 3:
					l3 = cacheSize; break;
				}
			}
		}
	}
};


struct BlockSizesBase
{
	inline static const CacheInfo cacheInfo;
};

// Calculate the best size for mc/kc/nc
// Ideally we want BlockA's (mc * kc)  micro panel (mr * kc) to fit in l1 Cache
// and BlockB's (kc * nc) micro panel (kc * nr) to fit in l2 Cache
template<class Type, class Index>
struct BlockSizes : public BlockSizesBase
{
	enum
	{
		mr		= PackingInfo<Type>::mr,
		nr		= PackingInfo<Type>::nr,
		roundTo = PacketTraits<Type>::Stride,
	};

	inline static constexpr double l1Percentage = 0.4;

	inline static bool init = false;
	inline static Index mc, kc, nc;

	BlockSizes()
	{
		if (init)
			return;
		init = true;

		auto l1 = cacheInfo.l1;
		auto l2 = cacheInfo.l2;

		int l1Rel = l1 / sizeof(Type);
		int l2Rel = (l2 - l1) / sizeof(Type);

		auto round = [](int val)
		{
			return ((val + roundTo - 1) / roundTo) * roundTo;
		};

		// TODO: Look into mc/kc/nc values as it relates to cache sizes more
		int m = l1Rel * l1Percentage;
		m /= mr;

		mc = round(m / 4);
		kc = mc;
		nc = round(l2Rel / 88);
	}
};

static constexpr int STACK_ALLOCATION_MAX = 128 * 1024;

// Note, this should only be called to hand memory to StackAlignedWrapper!
// TODO: Look into using malloca
#define StackAlignedAlloc(Type, size, Alignment) \
(size * sizeof(Type) + Alignment < STACK_ALLOCATION_MAX) \
	? reinterpret_cast<Type*>((reinterpret_cast<size_t>(alloca(sizeof(Type) * (size + Alignment - 1))) + Alignment - 1) & ~(Alignment - 1)) \
	: AlignedAllocator<Type>::allocate<Alignment>(size) \

template<class T>
struct StackAlignedWrapper
{
	using Traits = PacketTraits<T>;
	enum { Alignment = Traits::Alignment };

	template<class Size>
	StackAlignedWrapper(Size size, T* ptr) :
		onHeap{ size * sizeof(T) + Alignment >= STACK_ALLOCATION_MAX },
		ptr{ ptr },
		size{ static_cast<size_t>(size) }
	{}

	~StackAlignedWrapper()
	{
		if (onHeap)
			AlignedAllocator<T>::deallocate(ptr, size);
	}

	bool onHeap;
	T* RGR_RESTRICT ptr;
	size_t size;
};

} // End impl::