#pragma once
#include <cstdint>

typedef std::uint8_t uint8;
typedef std::uint32_t uint32;
typedef std::uint64_t uint64;

typedef std::int8_t int8;
typedef std::int32_t int32;
typedef std::int64_t int64;

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