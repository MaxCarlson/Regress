#pragma once

#ifdef __AVX2__
#define REGRESS_AVX2
#elif __AVX__
#define REGRESS_AVX
#else
#define REGRESS_SSE
#endif

namespace impl
{
// Get the packet traits of a type
template<class ValueType, class PacketType>
struct DefaultPacketTraits
{
	// The instruction packet type. (e.g. SSE double __m128d)
	using type = void;

	enum
	{
		Alignment	= alignof(PacketType),
		Size		= sizeof(ValueType),
		Stride		= 0,
		Packetable	= false,
		HasDivide	= true
	};
};

template<class Type>
struct PacketTraits : DefaultPacketTraits<Type>
{};
 
// Generic intrinsic templates
template<class Packet, class Type>
inline Packet pload(const Type* ptr) { static_assert(false); return {}; }

template<class Packet, class Type>
inline Packet pload1(const Type* ptr) { static_assert(false); return {}; }

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

} // End impl::