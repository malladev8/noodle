#pragma once
#include "ActorComponent.h"
#include "Math/NoodleMath.h"

class Movement : public ActorComponent
{
public:
	virtual void PostInit(const WorldContext& worldContext) override;
	virtual void Update(float32 deltaSeconds) override;
	void SetMoveInput(const vec3& moveInput) { m_MoveInput = moveInput; }
	void SetJump(bool jump) { m_Jump = jump; }
	virtual eComponentId GetComponentId() const override { return COMPONENT_ID; }

	const static eComponentId COMPONENT_ID;

protected:

private:
	virtual bool Init(std::ifstream& bin, EngineContext& engineContext) override;

	class PhysicsSystem* m_PhysicsSystem = nullptr;
	float32 m_WalkSpeed = 1.0f;
	float32 m_JumpSpeed = 1.0f;
	vec3 m_MoveInput;
	bool m_Jump = false;
};