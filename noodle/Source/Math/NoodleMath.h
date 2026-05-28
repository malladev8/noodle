#pragma once
#include "Core/NoodleCore.h"
#include <cmath>

// Noodle uses a left handed coordinate system where: X is forward, Y is right, Z is up

constexpr float32 EPSILON = 1e-6f;
constexpr float32 EPSILON_SQ = EPSILON * EPSILON;

constexpr float32 PI = 3.14159265358979323846f;
constexpr float32 TWO_PI = 2.0f * PI;
constexpr float32 HALF_PI = 0.5f * PI;

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

	float LengthSq() const { return powf(x, 2.0f) + powf(y, 2.0f); }
	float Length() const { return sqrtf(LengthSq()); }
};

inline vec2 operator+(const vec2& a, const vec2& b) { return {a.x + b.x, a.y + b.y}; }
inline vec2 operator-(const vec2& a, const vec2& b) { return {a.x - b.x, a.y - b.y}; }
inline vec2 operator*(const vec2& v, float s) { return { v.x * s, v.y * s }; }
inline vec2 operator*(float s, const vec2& v) { return v * s; }

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

	float LengthSq() const { return powf(x, 2.0f) + powf(y, 2.0f) + powf(z, 2.0f); }
	float Length() const { return sqrtf(LengthSq()); }
	void Normalize()
	{
		float length = Length();
		if (length > EPSILON)
		{
			*this *= (1.0f / length);
		}
	}
};

inline vec3 operator+(const vec3& a, const vec3& b) { return { a.x + b.x, a.y + b.y, a.z + b.z }; }
inline vec3 operator-(const vec3& a, const vec3& b) { return { a.x - b.x, a.y - b.y, a.z - b.z }; }
inline vec3 operator*(const vec3& v, float s) { return { v.x * s, v.y * s, v.z * s }; }
inline vec3 operator*(float s, const vec3& v) { return v * s; }

inline float32 Dot(const vec3& v0, const vec3& v1) { return (v0.x * v1.x) + (v0.y * v1.y) + (v0.z * v1.z); }

inline vec3 Cross(const vec3& v0, const vec3& v1)
{
	return
	{
		(v0.y * v1.z) - (v0.z * v1.y),
		(v0.z * v1.x) - (v0.x * v1.z),
		(v0.x * v1.y) - (v0.y * v1.x)
	};
}

inline vec3 Normalize(const vec3& v)
{
	float length = v.Length();
	if (length > EPSILON)
	{
		return v * (1.0f / length);
	}
	return v;
}

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

	float LengthSq() const { return powf(x, 2.0f) + powf(y, 2.0f) + powf(z, 2.0f) + powf(w, 2.0f); }
	float Length() const { return sqrtf(LengthSq()); }
};

inline vec4 operator+(const vec4& a, const vec4& b) { return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; }
inline vec4 operator-(const vec4& a, const vec4& b) { return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w }; }
inline vec4 operator*(const vec4& v, float s) { return { v.x * s, v.y * s, v.z * s, v.w * s }; }
inline vec4 operator*(float s, const vec4& v) { return v * s; }

// Matrices use column vectors with row-major storage
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

	mat3x3()
	{
		{
			for (uint32 i = 0; i < 3; ++i)
			{
				for (uint32 j = 0; j < 3; ++j)
				{
					m[i][j] = i == j ? 1.0f : 0.0f;
				}
			}
		}
	}

	mat3x3(float32 _00, float32 _01, float32 _02,
		   float32 _10, float32 _11, float32 _12,
		   float32 _20, float32 _21, float32 _22) :
		   m00(_00), m01(_01), m02(_02),
		   m10(_10), m11(_11), m12(_12),
		   m20(_20), m21(_21), m22(_22) {}

	static mat3x3 Identity()
	{
		return
		{
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f
		};
	}

	mat3x3 Transpose() const
	{
		return
		{
			m00, m10, m20,
			m01, m11, m21,
			m02, m12, m22
		};
	}
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

	mat4x4()
	{
		for (uint32 i = 0; i < 4; ++i)
		{
			for (uint32 j = 0; j < 4; ++j)
			{
				m[i][j] = i == j ? 1.0f : 0.0f;
			}
		}
	}

	mat4x4(float32 _00, float32 _01, float32 _02, float32 _03,
		   float32 _10, float32 _11, float32 _12, float32 _13,
		   float32 _20, float32 _21, float32 _22, float32 _23,
		   float32 _30, float32 _31, float32 _32, float32 _33) :
		   m00(_00), m01(_01), m02(_02), m03(_03),
		   m10(_10), m11(_11), m12(_12), m13(_13),
		   m20(_20), m21(_21), m22(_22), m23(_23),
		   m30(_20), m31(_21), m32(_22), m33(_33) {}

	static mat4x4 Identity()
	{
		return
		{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	mat4x4 Transpose() const
	{
		return
		{
			m00, m10, m20, m30,
			m01, m11, m21, m31,
			m02, m12, m22, m32,
			m03, m13, m23, m33
		};
	}

	mat4x4 InverseTransform() const
	{
		mat4x4 r;

		// Transpose rotation
		for (uint32 i = 0; i < 3; ++i)
		{
			for (uint32 j = 0; j < 3; ++j)
			{
				r.m[i][j] = m[j][i];
			}
		}

		// Invert translation
		r.m[0][3] = -1.0f * (r.m[0][0] * m[0][3] + r.m[0][1] * m[1][3] + r.m[0][2] * m[2][3]);
		r.m[1][3] = -1.0f * (r.m[1][0] * m[0][3] + r.m[1][1] * m[1][3] + r.m[1][2] * m[2][3]);
		r.m[2][3] = -1.0f * (r.m[2][0] * m[0][3] + r.m[2][1] * m[1][3] + r.m[2][2] * m[2][3]);

		// Last row
		r.m[3][0] = r.m[3][1] = r.m[3][2] = 0.0f;
		r.m[3][3] = 1.0f;

		return r;
	}

	mat4x4& operator*=(const mat4x4& other)
	{
		mat4x4 r;
		for (uint32 i = 0; i < 4; ++i)
		{
			for (uint32 j = 0; j < 4; ++j)
			{
				r.m[i][j] = (m[i][0] * other.m[0][j] +
					         m[i][1] * other.m[1][j] +
					         m[i][2] * other.m[2][j] +
					         m[i][3] * other.m[3][j]);
			}
		}

		*this = r;
		return *this;
	}
};

inline mat4x4 operator*(const mat4x4& a, const mat4x4& b)
{
	mat4x4 r;
	for (uint32 i = 0; i < 4; ++i)
	{
		for (uint32 j = 0; j < 4; ++j)
		{
			r.m[i][j] = (a.m[i][0] * b.m[0][j] +
				         a.m[i][1] * b.m[1][j] +
				         a.m[i][2] * b.m[2][j] +
				         a.m[i][3] * b.m[3][j]);
		}
	}
	return r;
}

// Transform Builders
inline mat4x4 BuildTranslation(float32 x, float32 y, float32 z)
{
	mat4x4 m;
	m.m[0][3] = x;
	m.m[1][3] = y;
	m.m[2][3] = z;
	return m;
}

inline mat4x4 BuildScale(float32 x, float32 y, float32 z)
{
	mat4x4 m;
	m.m[0][0] = x;
	m.m[1][1] = y;
	m.m[2][2] = z;
	m.m[3][3] = 1.0f;
	return m;
}

inline mat4x4 BuildScale(float32 scale)
{
	return BuildScale(scale, scale, scale);
}

inline mat4x4 BuildRotationX(float32 radians)
{
	float c = cosf(radians);
	float s = sinf(radians);

	mat4x4 m;

	m.m[1][1] = c;
	m.m[1][2] = -1.0f * s;
	m.m[2][1] = s;
	m.m[2][2] = c;

	return m;
}

inline mat4x4 BuildRotationY(float32 radians)
{
	float c = cosf(radians);
	float s = sinf(radians);

	mat4x4 m;

	m.m[0][0] = c;
	m.m[0][2] = s;
	m.m[2][0] = -1.0f * s;
	m.m[2][2] = c;

	return m;
}

inline mat4x4 BuildRotationZ(float32 radians)
{
	float c = cosf(radians);
	float s = sinf(radians);

	mat4x4 m;

	m.m[0][0] = c;
	m.m[0][1] = -1.0f * s;
	m.m[1][0] = s;
	m.m[1][1] = c;

	return m;
}

inline mat4x4 BuildRotation(float32 rollRadians, float32 pitchRadians, float32 yawRadians)
{
	mat4x4 rz = BuildRotationZ(yawRadians);
	mat4x4 ry = BuildRotationY(pitchRadians);
	mat4x4 rx = BuildRotationX(rollRadians);
	return rz * ry * rx;
}

inline mat4x4 BuildLookAt(const vec3& eye, const vec3& target, const vec3& up)
{
	vec3 forward = Normalize(target - eye);
	vec3 right = Normalize(Cross(up, forward));
	vec3 newUp = Cross(forward, right);

	// X axis (forward)
	mat4x4 m;
	m.m[0][0] = forward.x;
	m.m[0][1] = forward.y;
	m.m[0][2] = forward.z;
	m.m[0][3] = -1.0f * Dot(forward, eye);

	// Y axis (right)
	m.m[1][0] = right.x;
	m.m[1][1] = right.y;
	m.m[1][2] = right.z;
	m.m[1][3] = -1.0f * Dot(right, eye);

	// Z axis (up)
	m.m[2][0] = newUp.x;
	m.m[2][1] = newUp.y;
	m.m[2][2] = newUp.z;
	m.m[2][3] = -1.0f * Dot(newUp, eye);

	m.m[3][3] = 1.0f;

	return m;
}

inline mat4x4 BuildPerspective(float32 fov, float32 aspect, float32 nearZ, float32 farZ)
{
	float f = 1.0f / tanf(fov * 0.5f);

	mat4x4 m;

	m.m[0][0] = f / aspect;
	m.m[1][1] = f;
	m.m[2][2] = farZ / (farZ - nearZ);
	m.m[2][3] = (-1.0f * nearZ * farZ) / (farZ - nearZ);
	m.m[3][2] = 1.0f;

	return m;
}

// Matrix / Vector Transforms
inline vec4 operator*(const mat4x4& m, const vec4& v)
{
	vec4 r;

	r.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
	r.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
	r.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
	r.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;

	return r;
}

inline vec3 TransformPoint(const mat4x4& m, const vec3& v)
{
	vec4 r = m * vec4{ v.x, v.y, v.z, 1.0f };
	return { r.x / r.w, r.y / r.w, r.z / r.w };
}

inline vec3 TransformVector(const mat4x4& m, const vec3& v)
{
	vec4 r = m * vec4{ v.x, v.y, v.z, 0.0f };
	return { r.x, r.y, r.z };
}

// Directions
inline vec3 GetForward(const mat4x4& m)
{
	return Normalize({ m.m[0][0], m.m[1][0], m.m[2][0] });
}

inline vec3 GetRight(const mat4x4& m)
{
	return Normalize({ m.m[0][1], m.m[1][1], m.m[2][1] });
}

inline vec3 GetUp(const mat4x4& m)
{
	return Normalize({ m.m[0][2], m.m[1][2], m.m[2][2] });
}