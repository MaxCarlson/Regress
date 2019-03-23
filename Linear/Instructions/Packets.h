#pragma once


namespace impl
{
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

} // End impl::

#ifdef __AVX__
#include "AVX\PacketInstructions.h"
#else
#include "SSE\PacketInstructions.h"
#endif
