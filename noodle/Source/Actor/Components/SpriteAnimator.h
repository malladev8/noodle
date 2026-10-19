#pragma once
#include"ActorComponent.h"
#include <unordered_map>

class SpriteAnimator : public ActorComponent
{
public:
	virtual void Update(float32 deltaSeconds) override;
	virtual eComponentId GetComponentId() const override { return COMPONENT_ID; }

	const static eComponentId COMPONENT_ID;

protected:

private:
	std::unordered_map<std::string, struct SpriteAnimationClip> m_AnimationClipMap;
	std::string m_CurrentClipKey = "";
	float32 m_FrameTime = 0.0f;
	uint32 m_CurrentFrameIndex = 0; 

	virtual bool Init(std::ifstream& bin, EngineContext& engineContext) override;
};
