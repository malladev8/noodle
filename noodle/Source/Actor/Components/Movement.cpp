#include "NoodlePch.h"
#include "Movement.h"
#include "Actor/Actor.h"
#include "Actor/WorldContext.h"
#include "Collider.h"
#include "Physics/PhysicsSystem.h"
#include "Transform.h"
#include "Resource/SerializationTypes.h"

#include <fstream>

const eComponentId Movement::COMPONENT_ID = eComponentId::COMPONENT_MOVEMENT;

void Movement::PostInit(const WorldContext& worldContext)
{
	m_Physics = worldContext.physics;
}

void Movement::Update(float32 deltaSeconds)
{
	m_Velocity = m_MoveInput * m_Speed;
	vec3 displacement = m_Velocity * deltaSeconds;
	const Collider* collider = m_Owner->GetComponent<Collider>();

	if (m_Physics && collider)
	{
		m_Physics->RequestMovement(collider->GetColliderHandle(), displacement);
	}
	else
	{
		Transform* transform = m_Owner->GetComponent<Transform>();
		N_ASSERT(transform != nullptr, "Actor is missing Transform component.");
		transform->Translate(displacement);
	}
}

bool Movement::Init(std::ifstream& bin, EngineContext& engineContext)
{
	MovementSerializationData movementData;
	bin.read(reinterpret_cast<char*>(&movementData), sizeof(MovementSerializationData));
	m_Speed = movementData.speed;
	return true;
}
