#pragma once
#include "Core/NoodleCore.h"
#include "SpriteInterface.h"
#include "Resource/ResourceTypes.h"

class Sprite : public SpriteInterface
{
	friend class ActorFactory;

public:
	virtual void SubmitRenderCommands(class IRenderer& renderer) const override;
	void SetUvRect(const vec4& uvRect) { m_UvRect = uvRect; }
	std::weak_ptr<Material> GetMaterial() const { return m_Material; }

protected:
private:
	std::shared_ptr<Material> m_Material = nullptr;
	std::shared_ptr<TextureAtlas> m_Atlas = nullptr;
	std::string m_RegionName = "";
	vec4 m_Color{ 1.0f, 1.0f, 1.0f, 1.0f };
	vec4 m_UvRect{ 0.0f, 0.0f, 1.0f, 1.0f };
	vec2 m_WorldSize{ 1.0f, 1.0f };
	uint32 m_PixelsPerUnit = 100;
	eSpriteLayer m_Layer = eSpriteLayer::BACKGROUND;
	int32 m_Order = 0;

	virtual bool Init(std::ifstream& bin, EngineContext& engineContext) override;
};
