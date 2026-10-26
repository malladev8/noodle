#pragma once
#include "ActorComponent.h"
#include "Resource/ResourceTypes.h"

class Collider : public ActorComponent
{
public:
	const static eComponentId COMPONENT_ID;

	virtual void Update(float32 deltaSeconds) override;
	virtual eComponentId GetComponentId() const override { return COMPONENT_ID; }

	eColliderType GetShape() const { return m_ColliderType; }

	AABB GetWorldAABB() const;

protected:
private:

	virtual bool Init(std::ifstream& bin, EngineContext& engineContext) override;

	vec3 m_Offset = {};
	vec3 m_HalfExtents = { 0.5f, 0.5f, 0.0f };
	float32 m_Radius = 0.5f;
	eColliderType m_ColliderType = eColliderType::AABB;
};