#pragma once
#include "../../Core/NoodleCore.h"
#include "SpriteInterface.h"
#include "../../Resource/ResourceTypes.h"

class Sprite : public SpriteInterface
{
	friend class ActorFactory;

public:
	virtual void Render(class IRenderer& renderer) override;
protected:
private:
	std::shared_ptr<Texture> m_Texture = nullptr;
	float32 m_RotationRadians = 0.0f;
	virtual bool Init(std::ifstream& bin, EngineContext& engineContext) override;
};
