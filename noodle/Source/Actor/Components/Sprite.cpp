#include "NoodlePch.h"
#include "Sprite.h"

#include "Actor/Actor.h"
#include "EngineContext.h"
#include "Resource/ResourceManager.h"
#include "Rendering/RenderCommands.h"
#include "Rendering/IRenderer.h"
#include "Transform.h"
#include <fstream>

const eComponentId Sprite::COMPONENT_ID = eComponentId::COMPONENT_SPRITE;

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
	
	cmd.layer = (uint32)m_Layer;
	cmd.order = m_Order;

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

	// Load atlas
	bool usesAtlas = false;
	bin.read(reinterpret_cast<char*>(&usesAtlas), sizeof(usesAtlas));
	if (usesAtlas)
	{
		std::string atlasPath = path::ReadFilePath(bin);
		std::ifstream atlasBin(atlasPath, std::ios::binary);
		if (!atlasBin.is_open())
		{
			N_LOG("Failed to open Atlas binary file %s", atlasPath);
			return false;
		}
		m_Atlas = engineContext.resourceManager.LoadAtlas(atlasBin);
		m_RegionName = path::ReadFilePath(bin);

		// Calculate UV
		const SpriteRegion& region = m_Atlas->regions[m_RegionName];
		m_UvRect.x = (float32)region.x / (float32)m_Material->diffuseTexture->width;
		m_UvRect.y = (float32)region.y / (float32)m_Material->diffuseTexture->height;
		m_UvRect.z = ((float32)region.x + (float32)region.width) / (float32)m_Material->diffuseTexture->width;
		m_UvRect.w = ((float32)region.y + (float32)region.height) / (float32)m_Material->diffuseTexture->height;
	}

	// Determine world size
	bin.read(reinterpret_cast<char*>(&m_PixelsPerUnit), sizeof(m_PixelsPerUnit));
	const Texture* texture = m_Material->diffuseTexture.get();
	float32 ppuX = static_cast<float32>(texture->width) / static_cast<float32>(m_PixelsPerUnit);
	float32 ppuY = static_cast<float32>(texture->height) / static_cast<float32>(m_PixelsPerUnit);

	if (usesAtlas)
	{
		m_WorldSize.x = (float32)m_Atlas->regions[m_RegionName].width / (float32)m_Material->diffuseTexture->width * ppuX;
		m_WorldSize.y = (float32)m_Atlas->regions[m_RegionName].height / (float32)m_Material->diffuseTexture->height * ppuY;
	}
	else
	{
		m_WorldSize.x = (float32)m_Material->diffuseTextureLayout.tileWidth / (float32)m_Material->diffuseTexture->width * ppuX;
		m_WorldSize.y = (float32)m_Material->diffuseTextureLayout.tileHeight / (float32)m_Material->diffuseTexture->height * ppuY;
	}
	
	// Color
	bin.read(reinterpret_cast<char*>(&m_Color), sizeof(m_Color));

	// Layering
	bin.read(reinterpret_cast<char*>(&m_Layer), sizeof(m_Layer));
	bin.read(reinterpret_cast<char*>(&m_Order), sizeof(m_Order));

	return true;
}
