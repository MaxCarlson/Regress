#pragma once

#ifdef __AVX__
#include "AVX\PacketInstructions.h"
#else
#include "SSE\PacketInstructions.h"
#endif