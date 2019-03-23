#pragma once
#include "ForwardDeclarations.h"
#include "Instructions\PacketHelper.h"
#include <immintrin.h>

namespace impl
{
using Packet8f = __m256;
using Packet8i = __m256i;
using Packet4d = __m256d;

#ifdef REGRESS_AVX
template<>
struct PacketTraits<float> : public DefaultPacketTraits<float>
{
	using type = Packet8f;

	enum
	{
		Stride		= 8,
		Packetable	= true,
		HasDivide	= false
	};
};

template<>
struct PacketTraits<double> : public DefaultPacketTraits<double>
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
struct PacketTraits<int> : public DefaultPacketTraits<int>
{
	using type = Packet8i;

	enum
	{
		Stride		= 8,
		Packetable	= true,
	};
};
#endif

}