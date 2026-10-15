#pragma once
#include "ResourceTypes.h"

struct ComponentSerializationData
{
	eComponentId componentId = eComponentId::COUNT;
};

struct TransformSerializationData : public ComponentSerializationData
{	
	quaternion rotation;
	vec3 position;
	vec3 scale;
};

struct SpriteSerializationData : public ComponentSerializationData
{
	std::string material;
	std::string atlas;
	std::string region;
	vec4 color;
	uint32 pixlesPerUnit = 0;
	eSpriteLayer layer = eSpriteLayer::BACKGROUND;
	uint32 order = 0;
	bool usingAtlas = false;
};

struct SpriteAnimatorSerializationData : ComponentSerializationData
{
	std::vector<SpriteAnimationClip> animationClips;
	uint32 numClips = 0;
};

struct MovementSerializationData : public ComponentSerializationData
{
	float32 speed = 0.0f;
};

struct CameraSerializationData : public ComponentSerializationData
{
	vec4 clearColor;
	float32 nearPlane = 0.0f;
	float32 farPlane = 0.0f;
	float32 orthoHeight = 0.0f;
	eProjectionType projectionType = eProjectionType::ORTHOGRAPHIC;
};