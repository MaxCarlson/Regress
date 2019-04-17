#pragma once
/*
#ifdef _MSC_VER
#include <intrin.h>
#elif
#include <x86intrin.h>
#endif 
*/
#include <immintrin.h>

#ifdef __AVX2__
#define REGRESS_AVX2
#elif __AVX__
#define REGRESS_AVX
#else
#define REGRESS_SSE
#endif

namespace impl
{
template<class T> struct findAlignment { enum { value = alignof(T) }; };
template<> struct findAlignment<void> { enum { value = 0 }; };

// Get the packet traits of a type
template<class ValueType, class PacketType>
struct DefaultPacketTraits
{
	// The instruction packet type. (e.g. SSE double __m128d)
	using type = PacketType;

	enum
	{
		Alignment	= findAlignment<PacketType>::value,
		Size		= sizeof(ValueType),
		Stride		= Alignment / Size,
		Packetable	= false,
		HasDivide	= true
	};
};

template<class Type>
struct PacketTraits : DefaultPacketTraits<Type, void>
{};
 
// Generic intrinsic templates
template<class Packet, class Type>
inline Packet pload(const Type* ptr) { static_assert(false); return {}; }

template<class Packet, class Type>
inline Packet ploadu(const Type* ptr) { static_assert(false); return {}; }

template<class Packet, class Type>
inline Packet pset1(const Type& ptr) { static_assert(false); return {}; }

template<class Packet, class Type>
inline Packet pload1(const Type* ptr) { return pset1<Packet, Type>(*ptr); }

template<class Packet, class Type>
inline void pstore(Type* to, const Packet& ptr) { static_assert(false); }

template<class Packet>
inline Packet padd(const Packet& p1, const Packet& p2) { static_assert(false); return {}; }

template<class Packet>
inline Packet psub(const Packet& p1, const Packet& p2) { static_assert(false); return {}; }

template<class Packet>
inline Packet pmul(const Packet& p1, const Packet& p2) { static_assert(false); return {}; }

template<class Packet>
inline Packet pdiv(const Packet& p1, const Packet& p2) { static_assert(false); return {}; }

template<class Packet, class Type>
inline Packet pbroadcast(const Type* ptr) { static_assert(false); return {}; }

template<class Packet>
inline void transpose4x4(Packet& r1, Packet& r2, Packet& r3, Packet& r4) { static_assert(false); }

template<class T>
inline void prefetch(const T* ptr) { _mm_prefetch(reinterpret_cast<const char*>(ptr), _MM_HINT_T0); }

} // End impl::