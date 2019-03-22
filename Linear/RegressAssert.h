#pragma once

#ifdef _DEBUG
#else
#define NDEBUG
#endif

#include <assert.h>
#define regress_assert(cond) assert(cond)

