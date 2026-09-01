#pragma once
#include "MovementInterface.h"
#include "Math/NoodleMath.h"

class Movement : public MovementInterface
{
public:
	virtual void Update(float32 deltaSeconds) override;

protected:

private:
	virtual bool Init(std::ifstream& bin, EngineContext& engineContext) override;

	float32 m_Speed = 1.0f;
	vec3 m_Velocity;
};