#pragma once
#include "../Core/NoodleCore.h"

struct float2
{
	float32 x, y;

	float2() : x(0.0f), y(0.0f) {}
	float2(float32 x, float32 y) : x(x), y(y) {}
	float2(const float2& other) = default;
	float2& operator= (const float2& other) = default;

	float2 operator+ (const float2& other) const { return float2(x + other.x, y + other.y); }
	void operator+= (const float2& other) { x += other.x; y += other.y; }

	float2 operator- (const float2& other) const { return float2(x - other.x, y - other.y); }
	void operator-= (const float2& other) { x -= other.x; y -= other.y; }
};

struct float3
{
	float32 x, y, z;

	float3() : x(0.0f), y(0.0f), z(0.0f) {}
	float3(float32 x, float32 y, float32 z) : x(x), y(y), z(z) {}
	float3(const float3& other) = default;
	float3& operator= (const float3& other) = default;

	float3 operator+ (const float3& other) const { return float3(x + other.x, y + other.y, z + other.z); }
	void operator+= (const float3& other) { x += other.x; y += other.y; z += other.z; }

	float3 operator- (const float3& other) const { return float3(x - other.x, y - other.y, z - other.z); }
	void operator-= (const float3& other) { x -= other.x; y -= other.y; z -= other.z; }
};

struct float4
{
	float32 x, y, z, w;

	float4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
	float4(float32 x, float32 y, float32 z, float32 w) : x(x), y(y), z(z), w(w) {}
	float4(const float4& other) = default;
	float4& operator= (const float4& other) = default;

	float4 operator+ (const float4& other) const { return float4(x + other.x, y + other.y, z + other.z, w + other.w); }
	void operator+= (const float4& other) { x += other.x; y += other.y; z += other.z; w += other.w; }

	float4 operator- (const float4& other) const { return float4(x - other.x, y - other.y, z - other.z, w - other.w); }
	void operator-= (const float4& other) { x -= other.x; y -= other.y; z -= other.z; w -= other.w; }
};

float Dot(const float3& f0, const float3& f1);
float3 Cross(const float3& f0, const float3& f1);

struct float3x3
{
	float3 r0, r1, r2;
};

struct float4x4
{
	float4 r0, r1, r2, r3;
};
