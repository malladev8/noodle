#pragma once
#include "../Core/NoodleCore.h"

constexpr float EPSILON = 1e-6f;

struct float2
{
	float32 x, y;

	float2() : x(0.0f), y(0.0f) {}
	float2(float32 x, float32 y) : x(x), y(y) {}
	float2(const float2& other) = default;
	float2& operator= (const float2& other) = default;

	float2 operator+(const float2& other) const { return float2(x + other.x, y + other.y); }
	void operator+=(const float2& other) { x += other.x; y += other.y; }

	float2 operator-(const float2& other) const { return float2(x - other.x, y - other.y); }
	void operator-=(const float2& other) { x -= other.x; y -= other.y; }

	float2 operator*(float s) const { return float2(x * s, y * s); }
	void operator*=(float s) { x *= s; y *= s; }

	float2 operator/(float s) const { return float2(x / s, y / s); }
	void operator/=(float s) { x /= s; y /= s; }

	float LengthSq() const { return std::powf(x, 2.0f) + std::powf(y, 2.0f); }
	float Length() const { return std::sqrtf(LengthSq()); }
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

	float3 operator*(float s) const { return float3(x * s, y * s, z * s); }
	void operator*=(float s) { x *= s; y *= s; z *= s; }

	float3 operator/(float s) const { return float3(x / s, y / s, z / s); }
	void operator/=(float s) { x /= s; y /= s; z /= s; }

	float LengthSq() const { return std::powf(x, 2.0f) + std::powf(y, 2.0f) + std::powf(z, 2.0f); }
	float Length() const { return std::sqrtf(LengthSq()); }
	void Normalize();
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

	float4 operator*(float s) const { return float4(x * s, y * s, z * s, w * s); }
	void operator*=(float s) { x *= s; y *= s; z *= s; }

	float4 operator/(float s) const { return float4(x / s, y / s, z / s, w / s); }
	void operator/=(float s) { x /= s; y /= s; z /= s; w /= s; }

	float LengthSq() const { return std::powf(x, 2.0f) + std::powf(y, 2.0f) + std::powf(z, 2.0f) + std::powf(w, 2.0f); }
	float Length() const { return std::sqrtf(LengthSq()); }
};

float Dot(const float3& f0, const float3& f1);
float3 Cross(const float3& f0, const float3& f1);
float3 Normalize(const float3& f0);

struct float3x3
{
	float3 r0, r1, r2;
};

struct float4x4
{
	float4 r0, r1, r2, r3;
};
