#pragma once
#include <memory>

namespace impl
{

template<class Type>
struct AlignedAllocator
{
	using value_type		= Type;
	using pointer			= value_type * ;
	using const_pointer		= const value_type*;
	using reference			= value_type & ;
	using const_reference	= const value_type&;
	using size_type			= size_t;
	using difference_type	= std::ptrdiff_t;

	enum
	{
		Alignment = 16 < alignof(void*) ? alignof(void*) : 16
	};

	template<class T = Type>
	static inline T* allocate(size_type size, size_type alignment = Alignment)
	{
		if (alignment < alignof(void*))
			alignment = alignof(void*);

		size_type space	= size + alignment - 1;
		void* mem		= operator new(space + sizeof(void*));
		void* aligMem	= reinterpret_cast<void*>(reinterpret_cast<char*>(mem) + sizeof(void*));

		std::align(alignment, size, aligMem, space);

		*(reinterpret_cast<void**>(aligMem) - 1) = mem;

		return reinterpret_cast<T*>(aligMem);
	}

	template<class T>
	static inline void deallocate(T* ptr, size_type) {
		operator delete(*(reinterpret_cast<void**>(ptr) - 1));
	}

	bool operator==(const AlignedAllocator& other) const noexcept { return true; }
	bool operator!=(const AlignedAllocator& other) const noexcept { return false; }

};


} // End impl::
