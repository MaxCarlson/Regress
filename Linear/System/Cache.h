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

static const CacheInfo cacheInfo;

// TODO: Use allocStackAligned once working
template<class T>
struct StackAlignedWrapper
{
	using Traits = PacketTraits<T>;
	static constexpr int STACK_ALLOCATION_MAX = 128 * 1024;
	enum { Alignment = Traits::Alignment };

	template<class Size>
	StackAlignedWrapper(Size size) :
		onHeap{ size * sizeof(T) + Alignment >= STACK_ALLOCATION_MAX },
		ptr{ allocStackAligned(size) },
		size{ static_cast<size_t>(size) }
	{}

	RGR_FORCE_INLINE T* allocStackAligned(size_t size)
	{
		// BUG: Why is alloca allocating the same space twice in a row?
		// Is it because this is inside a function? That would make sense
		// Note: Not Working!
		//if(false)
		if (!onHeap)
		{
			size_t space = sizeof(T) * size + Alignment;
			void* ptr = alloca(space);
			auto* rr = reinterpret_cast<T*>(std::align(Alignment, sizeof(T) * size, ptr, space));
			return rr;
		}

		return AlignedAllocator<T>::allocate<Alignment>(size);
	}

	~StackAlignedWrapper()
	{
		if (onHeap)
			AlignedAllocator<T>::deallocate(ptr, size);
		//else
		//	_freea(ptr);
	}

	bool onHeap;
	T* RGR_RESTRICT ptr;
	size_t size;
};

} // End impl::