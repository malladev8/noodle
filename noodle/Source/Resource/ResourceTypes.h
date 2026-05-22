#pragma once
#include "../Core/NoodleCore.h"
#include "../Math/NoodleMath.h"
#include <vector>

struct Vertex
{
	vec3 position;
	vec3 normal;
	vec2 uv;
};

struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<uint32> indices;
};

enum class eTextureFormat  :uint32
{
	R8_UNORM = 0,
	RG8_UNORM,
	RGBA8_UNORM,
	RGBA8_UNORM_SRGB,
	BGRA8_UNORM,
	D24_UNORM_S8_UINT,
	D32_FLOAT,
	UNKNOWN
};

struct Texture
{
	void* resource = nullptr;
	uint32 width = 0;
	uint32 height = 0;
	eTextureFormat format = eTextureFormat::UNKNOWN;
};