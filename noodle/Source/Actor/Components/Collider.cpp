#include "NoodlePch.h"
#include "Collider.h"

#include "Actor/Actor.h"
#include "Physics/PhysicsSystem.h"
#include "Resource/SerializationTypes.h"
#include "Transform.h"

#include <fstream>

const eComponentId Collider::COMPONENT_ID = eComponentId::COMPONENT_COLLIDER;

void Collider::Destroy()
{
	if (m_PhysicsSystem != nullptr)
	{
		UnregisterFromPhysics(*m_PhysicsSystem);
	}
}

void Collider::Update(float32 deltaSeconds)
{
}

void Collider::RegisterWithPhysics(PhysicsSystem& physics)
{
	N_ASSERT(m_PhysicsSystem == nullptr,
		"Collider is already registered with a PhysicsSystem.");

	Actor* owner = GetOwner();
	N_ASSERT(owner != nullptr, "Collider owning Actor is null.");

	m_Handle = physics.RegisterCollider(owner, m_Offset, m_HalfExtents, m_Radius, m_ColliderType);
	m_PhysicsSystem = &physics;
}

void Collider::UnregisterFromPhysics(PhysicsSystem& physics)
{
	if (m_PhysicsSystem != &physics)
	{
		return;
	}

	physics.UnregisterCollider(m_Handle);

	m_Handle = {};
	m_PhysicsSystem = nullptr;
}

AABB Collider::GetWorldAABB() const
{
	N_ASSERT(m_PhysicsSystem != nullptr, "Collider is not registered with physics.");
	const ColliderData* data = m_PhysicsSystem->GetCollider(m_Handle);

	N_ASSERT(data != nullptr,"Collider handle is invalid.");
	return data->worldAABB;
}

bool Collider::Init(std::ifstream& bin, EngineContext& engineContext)
{
	ColliderSerializationData serializationData;
	N_ASSERT(bin.read(reinterpret_cast<char*>(&serializationData), sizeof(serializationData)), "Failed to read collider serialization data.");

	m_ColliderType = serializationData.colliderType;
	m_Offset = serializationData.offset;
	m_HalfExtents = serializationData.halfExtents;
	m_Radius = serializationData.radius;

	return true;
}
