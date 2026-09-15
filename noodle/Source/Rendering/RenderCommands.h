#pragma once
#include "Core/NoodleCore.h"
#include "Math/NoodleMath.h"

struct SpriteRenderCommand
{
	mat4x4 world = mat4x4::Identity();
	std::shared_ptr<struct Material> material = nullptr;
	vec4 color { 1.0f ,1.0f ,1.0f, 1.0f};
	vec2 uvMin { 0.0f , 0.0f };
	vec2 uvMax { 1.0f , 1.0f };
};