#include "NoodlePch.h"
#include "Movement.h"
#include "Actor/Actor.h"
#include "Actor/WorldContext.h"
#include "Collider.h"
#include "Physics.h"
#include "Physics/PhysicsSystem.h"
#include "Transform.h"
#include "Resource/SerializationTypes.h"

#include <fstream>

const eComponentId Movement::COMPONENT_ID = eComponentId::COMPONENT_MOVEMENT;

void Movement::PostInit(const WorldContext& worldContext)
{
	m_PhysicsSystem = worldContext.physicsSystem;
}

void Movement::Update(float32 deltaSeconds)
{
	vec3 velocity = m_MoveInput * m_WalkSpeed;

	Physics* physicsComponent = m_Owner->GetComponent<Physics>();
	if (physicsComponent)
	{
		if (physicsComponent->IsGrounded())
		{
			vec3 physVelocity = physicsComponent->GetVelocity();
			physVelocity.x = velocity.x;
			physicsComponent->SetVelocity(physVelocity);

			if (m_Jump)
			{
				N_ASSERT(m_PhysicsSystem != nullptr, "PhysicsSystem is required for jumping.");
				const vec3 jumpVelocity{ 0.0f, m_JumpSpeed, 0.0f };
				Collider* collider = m_Owner->GetComponent<Collider>();
				N_ASSERT(collider != nullptr, "Collider component is required for jumping.");
				m_PhysicsSystem->RequestImpulse(collider->GetColliderHandle(), jumpVelocity);
			}
		}
	}
	else
	{
		vec3 displacement = velocity * deltaSeconds;
		Transform* transform = m_Owner->GetComponent<Transform>();
		N_ASSERT(transform != nullptr, "Actor is missing Transform component.");
		transform->Translate(displacement);
	}
}

bool Movement::Init(std::ifstream& bin, EngineContext& engineContext)
{
	MovementSerializationData movementData;
	bin.read(reinterpret_cast<char*>(&movementData), sizeof(MovementSerializationData));
	m_WalkSpeed = movementData.walkSpeed;
	m_JumpSpeed = movementData.jumpSpeed;
	return true;
}
