#pragma once
#include"ActorComponent.h"
#include <unordered_map>

struct SpriteAnimationClip;

class SpriteAnimator : public ActorComponent
{
public:
	const static eComponentId COMPONENT_ID;
	virtual eComponentId GetComponentId() const override { return COMPONENT_ID; }

	virtual void Update(float32 deltaSeconds) override;

	bool SetClipKey(const std::string& clipName);
	void PlayClip(const std::string& clipName);
	void Stop();

	bool IsFinished() const { return m_IsFinished; }
	const std::string& GetCurrentClipKey() const { return m_CurrentClipKey; }

protected:

private:
	std::unordered_map<std::string, SpriteAnimationClip> m_AnimationClipMap;
	std::string m_CurrentClipKey = "";
	float32 m_FrameTime = 0.0f;
	uint32 m_CurrentFrameIndex = 0; 
	bool m_IsFinished = false;

	virtual bool Init(std::ifstream& bin, EngineContext& engineContext) override;
	void AdvanceFrame(const SpriteAnimationClip& clip);
};
