#include "NoodlePch.h"
#include "Collider.h"

#include "Actor/Actor.h"
#include "Resource/SerializationTypes.h"
#include "Transform.h"

#include <fstream>

const eComponentId Collider::COMPONENT_ID = eComponentId::COMPONENT_COLLIDER;

void Collider::Update(float32 deltaSeconds)
{
}

AABB Collider::GetWorldAABB() const
{
	Actor* owner = GetOwner();
	N_ASSERT(owner != nullptr, "Collider owning Actor is null.");
	Transform* transform = owner->GetComponent<Transform>();
	N_ASSERT(transform != nullptr, "Collider owning Actor is missing Transform component.");

	const vec3& pos = transform->GetPosition();
	const float32 centerX = pos.x + m_Offset.x;
	const float32 centerY = pos.y + m_Offset.y;

	const vec3& scale = transform->GetScale();
	const float32 halfWidth = m_HalfExtents.x * scale.x;
	const float32 halfHeight = m_HalfExtents.y * scale.y;

	AABB aabb;
	aabb.min.x = centerX - halfWidth;
	aabb.min.y = centerY - halfHeight;
	aabb.max.x = centerX + halfWidth;
	aabb.max.y = centerY + halfHeight;

	return aabb;
}

bool Collider::Init(std::ifstream& bin, EngineContext& engineContext)
{
	ColliderSerializationData serializationData;
	bin.read(reinterpret_cast<char*>(&serializationData), sizeof(serializationData));

	m_ColliderShape = serializationData.colliderShape;
	m_Offset = serializationData.offset;
	m_HalfExtents = serializationData.halfExtents;
	m_Radius = serializationData.radius;

	return true;
}
