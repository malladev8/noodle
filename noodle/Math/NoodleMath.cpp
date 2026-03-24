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
		return copy / length;
	}
	return copy;
}

void vec3::Normalize()
{
	float length = Length();
	if (length > EPSILON)
	{
		*this /= length;
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
