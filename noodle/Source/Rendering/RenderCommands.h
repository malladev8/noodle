#pragma once
#include "Core/NoodleCore.h"
#include "Math/NoodleMath.h"

struct Texture;

struct SpriteRenderCommand
{
	mat4x4 world = mat4x4::Identity();
	std::shared_ptr<Texture> texture = nullptr;
	vec4 color { 1.0f ,1.0f ,1.0f, 1.0f};
};