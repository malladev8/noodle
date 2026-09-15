#pragma once
#include"SpriteAnimatorInterface.h"
#include <unordered_map>

class SpriteAnimator : public SpriteAnimatorInterface
{
public:
	virtual void Update(float32 deltaSeconds) override;

protected:

private:
	std::unordered_map<std::string, struct SpriteAnimationClip> m_AnimationClipMap;
	std::string m_CurrentClipKey = "";
	float32 m_FrameTime = 0.0f;
	uint32 m_CurrentFrameIndex = 0; 

	virtual bool Init(std::ifstream& bin, EngineContext& engineContext) override;
};
