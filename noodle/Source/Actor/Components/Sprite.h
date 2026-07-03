#pragma once
#include "Core/NoodleCore.h"
#include "SpriteInterface.h"
#include "Resource/ResourceTypes.h"

class Sprite : public SpriteInterface
{
	friend class ActorFactory;

public:
	virtual void SubmitRenderCommands(class IRenderer& renderer) const override;
protected:
private:
	std::shared_ptr<Material> m_Material = nullptr;
	uint32 m_PixelsPerUnit = 100;
	vec2 m_WorldSize { 1.0f, 1.0f };

	virtual bool Init(std::ifstream& bin, EngineContext& engineContext) override;
};
