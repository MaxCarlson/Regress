#pragma once
#include "ForwardDeclarations.h"
#include "Instructions\PacketHelper.h"

namespace impl
{
using Packet8f = __m256;
using Packet8i = __m256i;
using Packet4d = __m256d;

#ifdef REGRESS_AVX
template<>
struct PacketTraits<float> : public DefaultPacketTraits<float, Packet8f>
{
	using type = Packet8f;

	enum
	{
		Stride		= 8,
		Packetable	= true,
	};
};

template<>
struct PacketTraits<double> : public DefaultPacketTraits<double, Packet4d>
{
	using type = Packet4d;

	enum
	{
		Stride		= 4,
		Packetable	= true,
	};
};
#endif

#ifdef REGRESS_AVX2
template<>
struct PacketTraits<int> : public DefaultPacketTraits<int, Packet8i>
{
	using type = Packet8i;

	enum
	{
		Stride		= 8,
		Packetable	= true,
		HasDivide	= false
	};
};
#endif

// Specialized intrinsic templates
template<> inline Packet8f pload<Packet8f, float>(const float*		from) { return _mm256_load_ps(from); }
template<> inline Packet8i pload<Packet8i, int>(const int*			from) { return _mm256_load_si256(reinterpret_cast<const __m256i*>(from)); }
template<> inline Packet4d pload<Packet4d, double>(const double*	from) { return _mm256_load_pd(from); }

template<> inline void pstore<Packet8f, float>(float*	to, const Packet8f& from) { return _mm256_store_ps(to, from); }
template<> inline void pstore<Packet8i,	int>(int*		to, const Packet8i& from) { return _mm256_store_si256(reinterpret_cast<__m256i*>(to), from); }
template<> inline void pstore<Packet4d, double>(double*	to, const Packet4d& from) { return _mm256_store_pd(to, from); }

template<> inline Packet8f padd<Packet8f>(const Packet8f& p1, const Packet8f& p2) { return _mm256_add_ps(p1, p2); }
template<> inline Packet4d padd<Packet4d>(const Packet4d& p1, const Packet4d& p2) { return _mm256_add_pd(p1, p2); }
template<> inline Packet8i padd<Packet8i>(const Packet8i& p1, const Packet8i& p2) { return _mm256_add_epi32(p1, p2); }

template<> inline Packet8f psub<Packet8f>(const Packet8f& p1, const Packet8f& p2) { return _mm256_sub_ps(p1, p2); }
template<> inline Packet4d psub<Packet4d>(const Packet4d& p1, const Packet4d& p2) { return _mm256_sub_pd(p1, p2); }
template<> inline Packet8i psub<Packet8i>(const Packet8i& p1, const Packet8i& p2) { return _mm256_sub_epi32(p1, p2); }

template<> inline Packet8f pmul<Packet8f>(const Packet8f& p1, const Packet8f& p2) { return _mm256_mul_ps(p1, p2); }
template<> inline Packet4d pmul<Packet4d>(const Packet4d& p1, const Packet4d& p2) { return _mm256_mul_pd(p1, p2); }
template<> inline Packet8i pmul<Packet8i>(const Packet8i& p1, const Packet8i& p2) { return _mm256_mullo_epi32(p1, p2); }

template<> inline Packet8f pdiv<Packet8f>(const Packet8f& p1, const Packet8f& p2) { return _mm256_div_ps(p1, p2); }
template<> inline Packet4d pdiv<Packet4d>(const Packet4d& p1, const Packet4d& p2) { return _mm256_div_pd(p1, p2); }

}