#include "../NoodlePch.h"
#include "NoodleMath.h"

float32 Dot(const vec3& v0, const vec3& v1)
{
	return (v0.x * v1.x) + (v0.y * v1.y) + (v0.z * v1.z);
}

vec3 Cross(const vec3& v0, const vec3& v1)
{
	return 
	{
		(v0.y * v1.z) - (v0.z * v1.y),
		(v0.z * v1.x) - (v0.x * v1.z),
		(v0.x * v1.y) - (v0.y * v1.x)
	};
}

vec3 Normalize(const vec3& v)
{
	vec3 copy = v;
	float length = copy.Length();
	if (length > EPSILON)
	{
		return copy * (1.0f / length);
	}
	return copy;
}

void vec3::Normalize()
{
	float length = Length();
	if (length > EPSILON)
	{
		*this *= (1.0f / length);
	}
}

mat3x3::mat3x3()
{
	for (uint32 i = 0; i < 3; ++i)
	{
		for (uint32 j = 0; j < 3; ++j)
		{
			m[i][j] = i == j ? 1.0f : 0.0f;
		}
	}
}

mat3x3::mat3x3(float32 _00, float32 _01, float32 _02, 
	           float32 _10, float32 _11, float32 _12, 
	           float32 _20, float32 _21, float32 _22) :
			   m00(_00), m01(_01), m02(_02),
			   m10(_10), m11(_11), m12(_12),
			   m20(_20), m21(_21), m22(_22)
{
}

mat3x3 mat3x3::Identity()
{
	return 
	{
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};
}

mat3x3 mat3x3::Transpose() const
{
	return 
	{
		m00, m10, m20,
		m01, m11, m21,
		m02, m12, m22
	};
}

mat4x4::mat4x4() 
{
	for (uint32 i = 0; i < 4; ++i)
	{
		for (uint32 j = 0; j < 4; ++j)
		{
			m[i][j] = i == j ? 1.0f : 0.0f;
		}
	}
}

mat4x4::mat4x4(float32 _00, float32 _01, float32 _02, float32 _03, 
	           float32 _10, float32 _11, float32 _12, float32 _13, 
	           float32 _20, float32 _21, float32 _22, float32 _23, 
	           float32 _30, float32 _31, float32 _32, float32 _33) :
			   m00(_00), m01(_01), m02(_02), m03(_03),
			   m10(_10), m11(_11), m12(_12), m13(_13),
			   m20(_20), m21(_21), m22(_22), m23(_23),
			   m30(_20), m31(_21), m32(_22), m33(_33)
{
}

mat4x4 mat4x4::Identity()
{
	return 
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

mat4x4 mat4x4::Transpose() const
{
	return
	{
		m00, m10, m20, m30,
		m01, m11, m21, m31,
		m02, m12, m22, m32,
		m03, m13, m23, m33
	};
}

mat4x4 mat4x4::InverseTransform() const
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

mat4x4 operator*(const mat4x4& a, const mat4x4& b) 
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

mat4x4& mat4x4::operator*=(const mat4x4& other)
{
	*this = *this * other;
	return *this;
}
