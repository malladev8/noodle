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
	cmd.world = transform->GetWorldMatrix() * mat4x4::BuildScale(m_WorldSize.x, m_WorldSize.y, 1.0f);
	
	cmd.color = m_Color;

	cmd.uvMin.x = m_UvRect.x;
	cmd.uvMin.y = m_UvRect.y;
	cmd.uvMax.x = m_UvRect.z;
	cmd.uvMax.y = m_UvRect.w;

	renderer.SubmitSprite(cmd);
}

bool Sprite::Init(std::ifstream& bin, EngineContext& engineContext)
{
	// Load material
	std::string materialPath = path::ReadFilePath(bin);
	std::ifstream materialBin(materialPath, std::ios::binary);
	if (!materialBin.is_open())
	{
		N_LOG("Failed to open Material binary file %s", materialPath);
		return false;
	}
	m_Material = engineContext.resourceManager.LoadMaterial(materialBin);

	// Determine world size
	bin.read(reinterpret_cast<char*>(&m_PixelsPerUnit), sizeof(m_PixelsPerUnit));
	const Texture* texture = m_Material->diffuseTexture.get();
	m_WorldSize.x = static_cast<float32>(texture->width) / static_cast<float32>(m_PixelsPerUnit);
	m_WorldSize.y = static_cast<float32>(texture->height) / static_cast<float32>(m_PixelsPerUnit);

	// Color
	bin.read(reinterpret_cast<char*>(&m_Color), sizeof(m_Color));

	return true;
}
