#pragma once
#include "Core/NoodleCore.h"
#include "Math/NoodleMath.h"

struct Texture;

struct SpriteRenderCommand
{
	std::shared_ptr<Texture> texture;
	vec3 position;
	vec2 scale;
	float32 rotation;
	// TODO: Add tint i.e. Color tint;
};