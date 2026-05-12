#pragma once

#if defined(_DEBUG)
#define DEBUG 1
#endif

#ifdef DEBUG
#include <cassert>
#define N_ASSERT(expr, msg) assert((expr) && (msg))
#else
#define N_ASSERT(expr, msg) ((void)0)
#endif

#ifdef DEBUG
#include <crtdbg.h>
#define N_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__) // Helps with memory leak detection
#else
#define N_NEW new
#endif