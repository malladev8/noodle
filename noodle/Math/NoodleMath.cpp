#include "../NoodlePch.h"
#include "NoodleMath.h"

float32 Dot(const float3& f0, const float3& f1)
{
	return (f0.x * f1.x) + (f0.y * f1.y) + (f0.z * f1.z);
}

float3 Cross(const float3& f0, const float3& f1)
{
	return 
	{
		(f0.y * f1.z) - (f0.z * f1.y),
		(f0.z * f1.x) - (f0.x * f1.z),
		(f0.x * f1.y) - (f0.y * f1.x)
	};
}

float3 Normalize(const float3& f0)
{
	float3 f = f0;
	float length = f.Length();
	if (length > EPSILON)
	{
		return f / length;
	}
	return f;
}

void float3::Normalize()
{
	float length = Length();
	if (length > EPSILON)
	{
		*this /= length;
	}
}
