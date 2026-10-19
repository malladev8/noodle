#pragma once
#include "ActorComponent.h"
#include "Math/NoodleMath.h"

class Movement : public ActorComponent
{
public:
	virtual void Update(float32 deltaSeconds) override;
	void SetMoveInput(const vec3& moveInput) { m_MoveInput = moveInput; }
	virtual eComponentId GetComponentId() const override { return COMPONENT_ID; }

	const static eComponentId COMPONENT_ID;

protected:

private:
	float32 m_Speed = 1.0f;
	vec3 m_Velocity;
	vec3 m_MoveInput;

	virtual bool Init(std::ifstream& bin, EngineContext& engineContext) override;
};