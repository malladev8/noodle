#include "NoodlePch.h"
#include "NoodleMath.h"

float32 vec3::Dot(const vec3& v0, const vec3& v1)
{
	return (v0.x * v1.x) + (v0.y * v1.y) + (v0.z * v1.z);
}

vec3 vec3::Cross(const vec3& v0, const vec3& v1)
{
	return
	{
		(v0.y * v1.z) - (v0.z * v1.y),
		(v0.z * v1.x) - (v0.x * v1.z),
		(v0.x * v1.y) - (v0.y * v1.x)
	};
}

vec3 vec3::Normalize(const vec3& v)
{
	float length = v.Length();
	if (length > EPSILON)
	{
		return v * (1.0f / length);
	}
	return v;
}

mat4x4 mat4x4::BuildTranslation(float32 x, float32 y, float32 z)
{
	mat4x4 m;
	m.m[0][3] = x;
	m.m[1][3] = y;
	m.m[2][3] = z;
	return m;
}

mat4x4 mat4x4::BuildTranslation(const vec3& v)
{
	return BuildTranslation(v.x, v.y, v.z);
}

mat4x4 mat4x4::BuildScale(float32 x, float32 y, float32 z)
{
	mat4x4 m;
	m.m[0][0] = x;
	m.m[1][1] = y;
	m.m[2][2] = z;
	m.m[3][3] = 1.0f;
	return m;
}

mat4x4 mat4x4::BuildScale(const vec3& v)
{
	return BuildScale(v.x, v.y, v.z);
}

mat4x4 mat4x4::BuildScale(float32 scale)
{
	return BuildScale(scale, scale, scale);
}

mat4x4 mat4x4::BuildRotationX(float32 radians)
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

mat4x4 mat4x4::BuildRotationY(float32 radians)
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

mat4x4 mat4x4::BuildRotationZ(float32 radians)
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

mat4x4 mat4x4::BuildRotation(float32 rollRadians, float32 pitchRadians, float32 yawRadians)
{
	mat4x4 rz = BuildRotationZ(yawRadians);
	mat4x4 ry = BuildRotationY(pitchRadians);
	mat4x4 rx = BuildRotationX(rollRadians);
	return rz * ry * rx;
}

mat4x4 mat4x4::BuildRotation(const quaternion& q)
{
	quaternion n = quaternion::Normalize(q);

	float xx = n.x * n.x;
	float yy = n.y * n.y;
	float zz = n.z * n.z;

	float xy = n.x * n.y;
	float xz = n.x * n.z;
	float yz = n.y * n.z;

	float wx = n.w * n.x;
	float wy = n.w * n.y;
	float wz = n.w * n.z;

	mat4x4 result;

	result.m[0][0] = 1.0f - 2.0f * (yy + zz);
	result.m[0][1] = 2.0f * (xy - wz);
	result.m[0][2] = 2.0f * (xz + wy);
	result.m[0][3] = 0.0f;

	result.m[1][0] = 2.0f * (xy + wz);
	result.m[1][1] = 1.0f - 2.0f * (xx + zz);
	result.m[1][2] = 2.0f * (yz - wx);
	result.m[1][3] = 0.0f;

	result.m[2][0] = 2.0f * (xz - wy);
	result.m[2][1] = 2.0f * (yz + wx);
	result.m[2][2] = 1.0f - 2.0f * (xx + yy);
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;
}

mat4x4 mat4x4::BuildLookAt(const vec3& eye, const vec3& target, const vec3& up)
{
	vec3 forward = vec3::Normalize(target - eye);
	vec3 right = vec3::Normalize(vec3::Cross(up, forward)); // TODO: verify this is the correct cross order
	vec3 newUp = vec3::Cross(forward, right); // TODO: verify this is the correct cross order

	mat4x4 m;

	// Forward -> column 0
	m.m[0][0] = forward.x;
	m.m[1][0] = forward.y;
	m.m[2][0] = forward.z;

	// Right -> column 1
	m.m[0][1] = right.x;
	m.m[1][1] = right.y;
	m.m[2][1] = right.z;

	// Up -> column 2
	m.m[0][2] = newUp.x;
	m.m[1][2] = newUp.y;
	m.m[2][2] = newUp.z;

	// Translation column
	m.m[0][3] = -vec3::Dot(forward, eye);
	m.m[1][3] = -vec3::Dot(right, eye);
	m.m[2][3] = -vec3::Dot(newUp, eye);

	m.m[3][3] = 1.0f;

	return m;
}

mat4x4 mat4x4::BuildPerspective(float32 fov, float32 aspect, float32 nearZ, float32 farZ)
{
	float f = 1.0f / tanf(fov * 0.5f);

	mat4x4 m = {};
	m.m[0][0] = f / aspect;
	m.m[1][1] = f;

	m.m[2][2] = farZ / (farZ - nearZ);
	m.m[2][3] = (-nearZ * farZ) / (farZ - nearZ);

	m.m[3][2] = 1.0f;
	m.m[3][3] = 0.0f;

	return m;
}