#pragma once
#include "ActorComponent.h"
#include "Physics/ColliderData.h"

class PhysicsSystem;

class Collider : public ActorComponent
{
public:
	const static eComponentId COMPONENT_ID;

	virtual void Destroy() override;
	virtual void Update(float32 deltaSeconds) override;
	virtual eComponentId GetComponentId() const override { return COMPONENT_ID; }

	eColliderType GetShape() const { return m_ColliderType; }

	AABB GetWorldAABB() const;

	void RegisterWithPhysics(PhysicsSystem& physics);
	void UnregisterFromPhysics(PhysicsSystem& physics);

protected:
private:

	virtual bool Init(std::ifstream& bin, EngineContext& engineContext) override;

	vec3 m_Offset = {};
	vec3 m_HalfExtents = { 0.5f, 0.5f, 0.0f };
	float32 m_Radius = 0.5f;
	eColliderType m_ColliderType = eColliderType::AABB;

	ColliderHandle m_Handle;
	PhysicsSystem* m_PhysicsSystem = nullptr;
};