#pragma once
#include "ForwardDeclarations.h"
#include <xmmintrin.h>
#include <emmintrin.h>

using Packet4f = __m128;
using Packet4i = __m128i;
using Packet2d = __m128d;

template<class Packet, class Type>
Packet pload(const Type* ptr) {}

template<class Packet, class Type>
void pstore(Type* to, const Packet& ptr) {}

template<> Packet4f pload<Packet4f, float>(const float* from) { return _mm_load_ps(from); }

template<> void pstore<Packet4f, float>(float* to, const Packet4f& from) { return _mm_store_ps(to, from); }