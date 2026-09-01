#pragma once
#include "MovementInterface.h"
#include "Math/NoodleMath.h"

class Movement : public MovementInterface
{
public:
	virtual void Update(float32 deltaSeconds) override;
	void SetMoveInput(const vec3& moveInput) { m_MoveInput = moveInput; }

protected:

private:
	virtual bool Init(std::ifstream& bin, EngineContext& engineContext) override;

	float32 m_Speed = 1.0f;
	vec3 m_Velocity;
	vec3 m_MoveInput;
};