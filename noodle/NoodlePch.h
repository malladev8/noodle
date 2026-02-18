#pragma once
// Not actually a precompiled header yet.


#ifdef _DEBUG
#include <cassert>
#define NASSERT(expr, msg) assert((expr) && (msg))
#else
#define NASSERT(expr, msg) ((void)0)
#endif
