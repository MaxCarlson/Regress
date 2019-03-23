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
template<class Type>
struct DefaultPacketTraits
{
	using type = void;

	enum
	{
		Size = sizeof(Type),
		Stride = 0,
		Packetable = false,
		HasDivide = true
	};
};

template<class Type>
struct PacketTraits : DefaultPacketTraits<Type>
{};

}