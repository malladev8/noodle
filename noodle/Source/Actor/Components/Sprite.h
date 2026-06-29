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
	virtual bool Init(std::ifstream& bin, EngineContext& engineContext) override;
};
