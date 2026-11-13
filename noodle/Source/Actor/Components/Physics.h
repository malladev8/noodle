#pragma once
#include "ActorComponent.h"
#include "Math/NoodleMath.h"

class Physics : public ActorComponent
{
public:
	const static eComponentId COMPONENT_ID;
	virtual eComponentId GetComponentId() const override { return COMPONENT_ID; }

	const vec3& GetVelocity() const { return m_Velocity; }
	void SetVelocity(const vec3& velocity) { m_Velocity = velocity; }

	float32 GetMass() const { return m_Mass; }
	void SetMass(float32 mass) { m_Mass = mass; }

	bool IsAffectedByGravity() const { return m_UsesGravity; }
	float32 GetGravityScale() const { return m_GravityScale; }

	bool IsGrounded() const { return m_IsGrounded; }

protected:

private:
	friend class PhysicsSystem2D;
	friend class PhysicsSystem3D;

	vec3 m_Velocity;
	float32 m_Mass = 1.0f;
	float32 m_GravityScale = 1.0f;
	bool m_UsesGravity = true;
	bool m_IsGrounded = false;

	virtual bool Init(std::ifstream& bin, EngineContext& engineContext) override;
};
