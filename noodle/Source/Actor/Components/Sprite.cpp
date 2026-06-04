#include "NoodlePch.h"
#include "Sprite.h"

#include "Actor/Actor.h"
#include "EngineContext.h"
#include "Resource/ResourceManager.h"
#include "Rendering/RenderCommands.h"
#include "Rendering/IRenderer.h"
#include "Transform.h"
#include <fstream>

const eComponentId SpriteInterface::COMPONENT_ID = eComponentId::COMPONENT_SPRITE;

void Sprite::Render(IRenderer& renderer)
{
	SpriteRenderCommand cmd;
	cmd.texture = m_Texture;
	
	N_ASSERT(m_Owner != nullptr, "Sprite component owning actor is null.");
	const Transform* transform = m_Owner->GetComponent<Transform>();
	N_ASSERT(transform != nullptr, "No transform component found for owning actor.");
	
	cmd.position = transform->GetPosition();

	// TODO: Scale can be a little confusing - consider clarifying / reorganizing for 2D
	// Right now the only scale being tracked is in 3D space
	// Sprites can only scale in 2D, and forward (x-axis) scale is meaningless
	// For 2D scaling x becoms right, and y becomes up
	cmd.scale.x = transform->GetScale().y;
	cmd.scale.y = transform->GetScale().z;

	cmd.rotation = m_RotationRadians;

	renderer.SubmitSprite(cmd);
}

bool Sprite::Init(std::ifstream& bin, EngineContext& engineContext)
{
	std::string texturePath = path::ReadFilePath(bin);

	std::ifstream textureBin(texturePath, std::ios::binary);
	if (!bin.is_open())
	{
		N_LOG("Failed to open Texture binary file %s", texturePath);
		return false;
	}

	m_Texture = engineContext.resourceManager.LoadTexture(textureBin);
	return true;
}
