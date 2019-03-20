#pragma once
#include "ForwardDeclarations.h"

namespace impl
{
using Packet4f = __m128;
using Packet4i = __m128i;
using Packet2d = __m128d;

// Get the packet traits of a type
template<class Type>
struct DefaultPacketTraits
{
	using type = void;

	enum
	{
		Size		= sizeof(Type),
		Stride		= 0,
		Packetable	= false,
		HasDivide	= true
	};
};

template<class Type>
struct PacketTraits : DefaultPacketTraits<Type>
{};


template<>
struct PacketTraits<float> : public DefaultPacketTraits<float>
{
	using type = Packet4f;

	enum
	{
		Stride		= 4,
		Packetable	= true,
		HasDivide	= false
	};
};

template<>
struct PacketTraits<double> : public DefaultPacketTraits<double>
{
	using type = Packet2d;

	enum
	{
		Stride		= 2,
		Packetable	= true,
	};
};

template<> 
struct PacketTraits<int> : public DefaultPacketTraits<int>
{
	using type = Packet4i;

	enum
	{
		Stride		= 4,
		Packetable	= true,
	};
};

// Generic intrinsic templates
template<class Packet, class Type>
Packet pload(const Type* ptr) { static_assert(false); return {}; }

template<class Packet, class Type>
void pstore(Type* to, const Packet& ptr) { static_assert(false); }

template<class Packet>
Packet padd(const Packet& p1, const Packet& p2) { static_assert(false); return {}; }

template<class Packet>
Packet psub(const Packet& p1, const Packet& p2) { static_assert(false); return {}; }

template<class Packet>
Packet pmul(const Packet& p1, const Packet& p2) { static_assert(false); return {}; }

template<class Packet>
Packet pdiv(const Packet& p1, const Packet& p2) { static_assert(false); return {}; }

// Specialized intrinsic templates
template<> Packet4f pload<Packet4f, float	>(const float*		from) { return _mm_load_ps(from); }
template<> Packet4i pload<Packet4i, int		>(const int*		from) { return _mm_load_si128(reinterpret_cast<const __m128i*>(from)); }
template<> Packet2d pload<Packet2d, double	> (const double*	from) { return _mm_load_pd(from); }

template<> void pstore<Packet4f, float	>(float*	to, const Packet4f& from) { return _mm_store_ps(to, from); }
template<> void pstore<Packet4i, int	>(int*		to, const Packet4i& from) { return _mm_store_si128(reinterpret_cast<__m128i*>(to), from); }
template<> void pstore<Packet2d, double	>(double*	to, const Packet2d& from) { return _mm_store_pd(to, from); }

template<> Packet4f padd<Packet4f>(const Packet4f& p1, const Packet4f& p2) { return _mm_add_ps(p1, p2); }
template<> Packet2d padd<Packet2d>(const Packet2d& p1, const Packet2d& p2) { return _mm_add_pd(p1, p2); }
template<> Packet4i padd<Packet4i>(const Packet4i& p1, const Packet4i& p2) { return _mm_add_epi32(p1, p2); }

template<> Packet4f psub<Packet4f>(const Packet4f& p1, const Packet4f& p2) { return _mm_sub_ps(p1, p2); }
template<> Packet2d psub<Packet2d>(const Packet2d& p1, const Packet2d& p2) { return _mm_sub_pd(p1, p2); }
template<> Packet4i psub<Packet4i>(const Packet4i& p1, const Packet4i& p2) { return _mm_sub_epi32(p1, p2); }

template<> Packet4f pmul<Packet4f>(const Packet4f& p1, const Packet4f& p2) { return _mm_mul_ps(p1, p2); }
template<> Packet2d pmul<Packet2d>(const Packet2d& p1, const Packet2d& p2) { return _mm_mul_pd(p1, p2); }
template<> Packet4i pmul<Packet4i>(const Packet4i& p1, const Packet4i& p2) { return _mm_mul_epi32(p1, p2); }

// TODO: We're going to need to do some traits stuff so no integer div is called
template<> Packet4f pdiv<Packet4f>(const Packet4f& p1, const Packet4f& p2) { return _mm_div_ps(p1, p2); }
template<> Packet2d pdiv<Packet2d>(const Packet2d& p1, const Packet2d& p2) { return _mm_div_pd(p1, p2); }

} // End impl::

