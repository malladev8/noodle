#pragma once

#if defined(_DEBUG)
#define DEBUG 1
#endif

#ifdef DEBUG
#include <cassert>
#define NASSERT(expr, msg) assert((expr) && (msg))
#else
#define NASSERT(expr, msg) ((void)0)
#endif