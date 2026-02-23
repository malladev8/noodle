#pragma once
// Not actually a precompiled header yet.
#include "Core/NoodleCore.h"

#include <iostream>
#include <cstdlib>

#if defined(WINDOWS)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#endif