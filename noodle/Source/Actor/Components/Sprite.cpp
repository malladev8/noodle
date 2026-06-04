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
	cmd.world = transform->GetWorldMatrix();

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
