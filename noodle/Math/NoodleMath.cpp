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
