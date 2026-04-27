#pragma once
#include "../Core/NoodleCore.h"

// Noodle uses a left handed coordinate system where: X - forward, Y - right, Z - up

constexpr float EPSILON = 1e-6f;
constexpr float EPSILON_SQ = EPSILON * EPSILON;

constexpr float PI = 3.14159265358979323846f;
constexpr float TWO_PI = 2.0f * PI;
constexpr float HALF_PI = 0.5f * PI;

constexpr float32 RadToDeg(float32 rad)
{
	return rad * (180.0f / PI);
}

constexpr float32 DegToRad(float32 deg)
{
	return deg * (PI / 180.0f);
}

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

	vec2& operator+=(const vec2& other) { x += other.x; y += other.y; return *this; }
	vec2& operator-=(const vec2& other) { x -= other.x; y -= other.y; return *this; }
	vec2& operator*=(float s) { x *= s; y *= s; return *this; }

	float LengthSq() const { return std::powf(x, 2.0f) + std::powf(y, 2.0f); }
	float Length() const { return std::sqrtf(LengthSq()); }
};

vec2 operator+(const vec2& a, const vec2& b) { return {a.x + b.x, a.y + b.y}; }
vec2 operator-(const vec2& a, const vec2& b) { return {a.x - b.x, a.y - b.y}; }
vec2 operator*(const vec2& v, float s) { return { v.x * s, v.y * s }; }
vec2 operator*(float s, const vec2& v) { return v * s; }

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

	vec3& operator+=(const vec3& other) { x += other.x; y += other.y; z += other.z; return *this; }
	vec3& operator-=(const vec3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
	vec3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }

	float LengthSq() const { return std::powf(x, 2.0f) + std::powf(y, 2.0f) + std::powf(z, 2.0f); }
	float Length() const { return std::sqrtf(LengthSq()); }
	void Normalize();
};

vec3 operator+(const vec3& a, const vec3& b) { return { a.x + b.x, a.y + b.y, a.z + b.z }; }
vec3 operator-(const vec3& a, const vec3& b) { return { a.x - b.x, a.y - b.y, a.z - b.z }; }
vec3 operator*(const vec3& v, float s) { return { v.x * s, v.y * s, v.z * s }; }
vec3 operator*(float s, const vec3& v) { return v * s; }

float32 Dot(const vec3& v0, const vec3& v1);
vec3 Cross(const vec3& v0, const vec3& v1);
vec3 Normalize(const vec3& v);

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

	vec4& operator+=(const vec4& other) { x += other.x; y += other.y; z += other.z; w += other.w; return *this; }
	vec4& operator-=(const vec4& other) { x -= other.x; y -= other.y; z -= other.z; w -= other.w; return *this; }
	vec4& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }

	float LengthSq() const { return std::powf(x, 2.0f) + std::powf(y, 2.0f) + std::powf(z, 2.0f) + std::powf(w, 2.0f); }
	float Length() const { return std::sqrtf(LengthSq()); }
};

vec4 operator+(const vec4& a, const vec4& b) { return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; }
vec4 operator-(const vec4& a, const vec4& b) { return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w }; }
vec4 operator*(const vec4& v, float s) { return { v.x * s, v.y * s, v.z * s, v.w * s }; }
vec4 operator*(float s, const vec4& v) { return v * s; }

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

	mat3x3();
	mat3x3(float32 _00, float32 _01, float32 _02,
		   float32 _10, float32 _11, float32 _12,
		   float32 _20, float32 _21, float32 _22);

	static mat3x3 Identity();
	mat3x3 Transpose() const;
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

	mat4x4();
	mat4x4(float32 _00, float32 _01, float32 _02, float32 _03,
		   float32 _10, float32 _11, float32 _12, float32 _13,
		   float32 _20, float32 _21, float32 _22, float32 _23,
		   float32 _30, float32 _31, float32 _32, float32 _33);

	static mat4x4 Identity();
	mat4x4 Transpose() const;
	mat4x4 InverseTransform() const;

	mat4x4& operator*=(const mat4x4& other);
};

mat4x4 operator*(const mat4x4& a, const mat4x4& b);

// Transform Builders
mat4x4 Translation(float32 x, float32 y, float32 z);
mat4x4 Scale(float32 x, float32 y, float32 z);
mat4x4 Scale(float32 scale);
mat4x4 RotationX(float32 angle);
mat4x4 RotationY(float32 angle);
mat4x4 RotationZ(float32 angle);
mat4x4 Rotation(float32 roll, float32 pitch, float32 yaw);
mat4x4 LookAt(const vec3& eye, const vec3& target, const vec3& up);
mat4x4 Perspective(float32 fov, float32 aspect, float32 nearZ, float32 farZ);