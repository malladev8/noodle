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

void Sprite::SubmitRenderCommands(IRenderer& renderer) const
{
	SpriteRenderCommand cmd;
	cmd.material = m_Material;
	
	N_ASSERT(m_Owner != nullptr, "Sprite component owning actor is null.");
	const Transform* transform = m_Owner->GetComponent<Transform>();
	N_ASSERT(transform != nullptr, "No transform component found for owning actor.");
	cmd.world = transform->GetWorldMatrix();

	renderer.SubmitSprite(cmd);
}

bool Sprite::Init(std::ifstream& bin, EngineContext& engineContext)
{
	std::string materialPath = path::ReadFilePath(bin);

	std::ifstream materialBin(materialPath, std::ios::binary);
	if (!materialBin.is_open())
	{
		N_LOG("Failed to open Material binary file %s", materialPath);
		return false;
	}

	m_Material = engineContext.resourceManager.LoadMaterial(materialBin);
	return true;
}
