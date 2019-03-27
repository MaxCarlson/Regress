#pragma once
#include <memory>

#ifdef _MSC_VER
#else
#include <cstdlib>
#endif

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
		Alignment = 32
	};

	AlignedAllocator() = default;

	template<class Other>
	AlignedAllocator(const AlignedAllocator<Other>& o) {}

	template<int AlignmentV = Alignment>
	static inline Type* allocate(size_type size)
	{
		#ifdef _MSC_VER
			return reinterpret_cast<Type*>(_aligned_malloc(size * sizeof(Type), AlignmentV));
		#else
			return std::aligned_alloc(size * sizeof(Type), Alignment);
		#endif
	}

	static inline void deallocate(Type* ptr, size_type) 
	{
		#ifdef _MSC_VER		
			_aligned_free(ptr);
		#else
			std::free(ptr);
		#endif
	}

	bool operator==(const AlignedAllocator& other) const noexcept { return true; }
	bool operator!=(const AlignedAllocator& other) const noexcept { return false; }

};


} // End impl::
