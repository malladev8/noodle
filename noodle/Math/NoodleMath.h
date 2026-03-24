#pragma once
#include "../Core/NoodleCore.h"

constexpr float EPSILON = 1e-6f;

struct vec2
{
	union
	{
		struct{ float32 x, y; };
		struct{ float32 r, g; };
		float32 data[2];
	};
	

	vec2() : x(0.0f), y(0.0f) {}
	vec2(float32 x, float32 y) : x(x), y(y) {}
	vec2(const vec2& other) = default;
	vec2& operator= (const vec2& other) = default;

	vec2 operator+(const vec2& other) const { return vec2(x + other.x, y + other.y); }
	void operator+=(const vec2& other) { x += other.x; y += other.y; }

	vec2 operator-(const vec2& other) const { return vec2(x - other.x, y - other.y); }
	void operator-=(const vec2& other) { x -= other.x; y -= other.y; }

	vec2 operator*(float s) const { return vec2(x * s, y * s); }
	void operator*=(float s) { x *= s; y *= s; }

	vec2 operator/(float s) const { return vec2(x / s, y / s); }
	void operator/=(float s) { x /= s; y /= s; }

	float LengthSq() const { return std::powf(x, 2.0f) + std::powf(y, 2.0f); }
	float Length() const { return std::sqrtf(LengthSq()); }
};

struct vec3
{
	union
	{
		struct{ float32 x, y, z; };
		struct{ float32 r, g, b; };
		float32 data[3];
	};
	

	vec3() : x(0.0f), y(0.0f), z(0.0f) {}
	vec3(float32 x, float32 y, float32 z) : x(x), y(y), z(z) {}
	vec3(const vec3& other) = default;
	vec3& operator= (const vec3& other) = default;

	vec3 operator+ (const vec3& other) const { return vec3(x + other.x, y + other.y, z + other.z); }
	void operator+= (const vec3& other) { x += other.x; y += other.y; z += other.z; }

	vec3 operator- (const vec3& other) const { return vec3(x - other.x, y - other.y, z - other.z); }
	void operator-= (const vec3& other) { x -= other.x; y -= other.y; z -= other.z; }

	vec3 operator*(float s) const { return vec3(x * s, y * s, z * s); }
	void operator*=(float s) { x *= s; y *= s; z *= s; }

	vec3 operator/(float s) const { return vec3(x / s, y / s, z / s); }
	void operator/=(float s) { x /= s; y /= s; z /= s; }

	float LengthSq() const { return std::powf(x, 2.0f) + std::powf(y, 2.0f) + std::powf(z, 2.0f); }
	float Length() const { return std::sqrtf(LengthSq()); }
	void Normalize();
};

struct vec4
{
	union
	{
		struct { float32 x, y, z, w; };
		struct { float32 r, g, b, a; };
		float32 data[4];
	};
	

	vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
	vec4(float32 x, float32 y, float32 z, float32 w) : x(x), y(y), z(z), w(w) {}
	vec4(const vec4& other) = default;
	vec4& operator= (const vec4& other) = default;

	vec4 operator+ (const vec4& other) const { return vec4(x + other.x, y + other.y, z + other.z, w + other.w); }
	void operator+= (const vec4& other) { x += other.x; y += other.y; z += other.z; w += other.w; }

	vec4 operator- (const vec4& other) const { return vec4(x - other.x, y - other.y, z - other.z, w - other.w); }
	void operator-= (const vec4& other) { x -= other.x; y -= other.y; z -= other.z; w -= other.w; }

	vec4 operator*(float s) const { return vec4(x * s, y * s, z * s, w * s); }
	void operator*=(float s) { x *= s; y *= s; z *= s; }

	vec4 operator/(float s) const { return vec4(x / s, y / s, z / s, w / s); }
	void operator/=(float s) { x /= s; y /= s; z /= s; w /= s; }

	float LengthSq() const { return std::powf(x, 2.0f) + std::powf(y, 2.0f) + std::powf(z, 2.0f) + std::powf(w, 2.0f); }
	float Length() const { return std::sqrtf(LengthSq()); }
};

float32 Dot(const vec3& v0, const vec3& v1);
vec3 Cross(const vec3& v0, const vec3& v1);
vec3 Normalize(const vec3& v);

struct mat3x3
{
	union
	{
		float32 m[3][3]; // [row][column]

		struct
		{
			float32 m00, m01, m02;
			float32 m10, m11, m12;
			float32 m20, m21, m22;
		};

		vec3 rows[3];
	};
};

struct mat4x4
{
	union
	{
		float32 m[4][4]; // [row][column]

		struct
		{
			float32 m00, m01, m02, m03;
			float32 m10, m11, m12, m13;
			float32 m20, m21, m22, m23;
			float32 m30, m31, m32, m33;
		};

		vec4 rows[4];
	};
};
