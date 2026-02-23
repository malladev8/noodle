#pragma once
#include <cstdint>

using uint8 = std::uint8_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

using int8 = std::int8_t;
using int32 = std::int32_t;
using int64 = std::int64_t;

using float32 = float;
using float64 = double;

constexpr size_t operator"" _KB(unsigned long long x)
{
    return x * 1024ull;
}

constexpr size_t operator"" _MB(unsigned long long x)
{
    return x * 1024ull * 1024ull;
}

constexpr size_t operator"" _GB(unsigned long long x)
{
    return x * 1024ull * 1024ull * 1024ull;
}

struct NoodleWindowDesc
{
    uint32 width;
    uint32 height;
    const char* title;
};