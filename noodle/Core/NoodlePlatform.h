#pragma once

#if defined(_WIN32) || (_WIN64)
#define WINDOWS 1
#else
#define WINDOWS 0
#endif

#if defined(_MSC_VER)
#define MSVC 1
#else
#define MSVC 0
#endif