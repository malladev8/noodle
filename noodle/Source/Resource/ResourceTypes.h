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

struct TextureLayout
{
	uint32 tileWidth = 0;
	uint32 tileHeight = 0;
	uint32 numRows = 0;
	uint32 numColumns = 0;
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
	TextureLayout diffuseTextureLayout;
};

enum class eProjectionType
{
	ORTHOGRAPHIC,
	PERSPECTIVE // Perspective is not currently supported.
};

struct CameraData
{
	mat4x4 view;
	mat4x4 projection;
	vec4 clearColor;
};

struct FrameConstants
{
	mat4x4 view;
	mat4x4 projection;
	mat4x4 viewProjection;
};

struct SpriteConstants
{
	mat4x4 world;
	vec4 color;
	vec2 uvMin;
	vec2 uvMax;
};

struct SpriteAnimationClip
{
	std::string name = "";
	uint32 fps = 0;
	uint32 frames = 0;
	bool loop = false;
};