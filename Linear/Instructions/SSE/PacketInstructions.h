#pragma once
#include "ForwardDeclarations.h"
#include "Instructions\PacketHelper.h"

namespace impl
{
using Packet4f = __m128;
using Packet4i = __m128i;
using Packet2d = __m128d;

#ifdef REGRESS_SSE
template<>
struct PacketTraits<float> : public DefaultPacketTraits<float, Packet4f>
{
	using type = Packet4f;

	enum
	{
		Stride		= 4,
		Packetable	= true,
	};
};

template<>
struct PacketTraits<double> : public DefaultPacketTraits<double, Packet2d>
{
	using type = Packet2d;

	enum
	{
		Stride		= 2,
		Packetable	= true,
	};
};
#endif

#ifndef REGRESS_AVX2
template<> 
struct PacketTraits<int> : public DefaultPacketTraits<int, Packet4i>
{
	using type = Packet4i;

	enum
	{
		Stride		= 4,
		Packetable	= true,
		HasDivide	= false
	};
};
#endif

// Specialized intrinsic templates
template<> inline Packet4f pload<Packet4f, float>(const float*		from) { return _mm_load_ps(from); }
template<> inline Packet4i pload<Packet4i, int>(const int*			from) { return _mm_load_si128(reinterpret_cast<const __m128i*>(from)); }
template<> inline Packet2d pload<Packet2d, double> (const double*	from) { return _mm_load_pd(from); }

template<> inline Packet4f ploadu<Packet4f, float>(const float*		from) { return _mm_loadu_ps(from); }
template<> inline Packet4i ploadu<Packet4i, int>(const int*			from) { return _mm_loadu_si128(reinterpret_cast<const __m128i*>(from)); }
template<> inline Packet2d ploadu<Packet2d, double>(const double*	from) { return _mm_loadu_pd(from); }

template<> inline Packet4f pload1<Packet4f, float	>(const float*		from) { return _mm_load1_ps(from); }
// Int defaults to pset1
template<> inline Packet2d pload1<Packet2d, double	>(const double*	from) { return _mm_load1_pd(from); }

template<> inline Packet4f pset1<Packet4f, float	>(const float&		from) { return _mm_set1_ps(from); }
template<> inline Packet4i pset1<Packet4i, int		>(const int&		from) { return _mm_set1_epi32(from); }
template<> inline Packet2d pset1<Packet2d, double	>(const double&		from) { return _mm_set1_pd(from); }

template<> inline void pstore<Packet4f, float	>(float*	to, const Packet4f& from) { return _mm_store_ps(to, from); }
template<> inline void pstore<Packet4i, int		>(int*		to, const Packet4i& from) { return _mm_store_si128(reinterpret_cast<__m128i*>(to), from); }
template<> inline void pstore<Packet2d, double	>(double*	to, const Packet2d& from) { return _mm_store_pd(to, from); }

template<> inline Packet4f padd<Packet4f>(const Packet4f& p1, const Packet4f& p2) { return _mm_add_ps(p1, p2); }
template<> inline Packet2d padd<Packet2d>(const Packet2d& p1, const Packet2d& p2) { return _mm_add_pd(p1, p2); }
template<> inline Packet4i padd<Packet4i>(const Packet4i& p1, const Packet4i& p2) { return _mm_add_epi32(p1, p2); }

template<> inline Packet4f psub<Packet4f>(const Packet4f& p1, const Packet4f& p2) { return _mm_sub_ps(p1, p2); }
template<> inline Packet2d psub<Packet2d>(const Packet2d& p1, const Packet2d& p2) { return _mm_sub_pd(p1, p2); }
template<> inline Packet4i psub<Packet4i>(const Packet4i& p1, const Packet4i& p2) { return _mm_sub_epi32(p1, p2); }

template<> inline Packet4f pmul<Packet4f>(const Packet4f& p1, const Packet4f& p2) { return _mm_mul_ps(p1, p2); }
template<> inline Packet2d pmul<Packet2d>(const Packet2d& p1, const Packet2d& p2) { return _mm_mul_pd(p1, p2); }
template<> inline Packet4i pmul<Packet4i>(const Packet4i& p1, const Packet4i& p2) { return _mm_mullo_epi32(p1, p2); }

template<> inline Packet4f pdiv<Packet4f>(const Packet4f& p1, const Packet4f& p2) { return _mm_div_ps(p1, p2); }
template<> inline Packet2d pdiv<Packet2d>(const Packet2d& p1, const Packet2d& p2) { return _mm_div_pd(p1, p2); }

template<> inline Packet4f pbroadcast(const float* ptr) 
{ 
	return _mm_broadcast_ss(ptr);
}

template<> inline Packet4i pbroadcast(const int* ptr)
{
	// TODO: Clean-up
	// This is super ugly and probably takes a performance hit. 
	return *reinterpret_cast<Packet4i*>(&_mm_broadcast_ss(reinterpret_cast<const float*>(ptr)));
}

template<> inline Packet2d pbroadcast(const double* ptr)
{
	return _mm_broadcastsd_pd(*reinterpret_cast<const Packet2d*>(ptr));
}

template<> inline void transpose4x4<Packet4f>(Packet4f& r1, Packet4f& r2, Packet4f& r3, Packet4f& r4)
{
	_MM_TRANSPOSE4_PS(r1, r2, r3, r4);
}

/* https://www.randombit.net/bitbashing/2009/10/08/integer_matrix_transpose_in_sse2.html */
template<> inline void transpose4x4<Packet4i>(Packet4i& r1, Packet4i& r2, Packet4i& r3, Packet4i& r4)
{
	Packet4i T0 = _mm_unpacklo_epi32(r1, r2);
	Packet4i T1 = _mm_unpacklo_epi32(r3, r4);
	Packet4i T2 = _mm_unpackhi_epi32(r1, r2);
	Packet4i T3 = _mm_unpackhi_epi32(r3, r4);

	/* Assigning transposed values back into I[0-3] */
	r1 = _mm_unpacklo_epi64(T0, T1);
	r2 = _mm_unpackhi_epi64(T0, T1);
	r3 = _mm_unpacklo_epi64(T2, T3);
	r4 = _mm_unpackhi_epi64(T2, T3);
}

} // End impl::

