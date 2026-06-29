#pragma once
#include "Core/NoodleCore.h"
#include "Math/NoodleMath.h"
#include <vector>
#include <memory>

using AssetId = uint64;

struct Vertex
{
	vec3 position;
	vec3 normal;
	vec2 uv;
};

struct SpriteVertex
{
	vec3 position;
	vec2 uv;
};

struct ColorVertex
{
	vec3 position;
	vec3 normal;
	vec2 uv;
	vec4 color;
};

struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<uint32> indices;
};

enum class eTextureFormat : uint32
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

struct ITextureResource
{
	virtual ~ITextureResource() = default;
};

struct Texture
{
	std::unique_ptr<ITextureResource> resource = nullptr;
	uint32 width = 0;
	uint32 height = 0;
	eTextureFormat format = eTextureFormat::UNKNOWN;
};

struct Shader
{
	virtual ~Shader() = default;
};

struct CookedShaderData
{
	std::vector<uint8> vsBytes;
	std::vector<uint8> psBytes;
};

struct Material
{
	std::shared_ptr<Shader> shader = nullptr;
	std::shared_ptr<Texture> diffuseTexture = nullptr;
};

enum class eProjectionType
{
	ORTHOGRAPHIC,
	PERSPECTIVE // Perspective is not currently supported.
};