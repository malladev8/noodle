#include "NoodlePch.h"
#include "Movement.h"
#include "Transform.h"
#include "Actor/Actor.h"
#include <fstream>

const eComponentId MovementInterface::COMPONENT_ID = eComponentId::COMPONENT_MOVEMENT;

void Movement::Update(float32 deltaSeconds)
{
	m_Velocity = m_MoveInput * m_Speed;

	// Eventually will want to resolve final velocity with physics / collision
	// Just mutate transform directly for now

	Transform* transform = m_Owner->GetComponent<Transform>();
	N_ASSERT(transform != nullptr, "Actor is missing Transform component.");
	transform->Translate(m_Velocity * deltaSeconds);
}

bool Movement::Init(std::ifstream& bin, EngineContext& engineContext)
{
	bin.read(reinterpret_cast<char*>(&m_Speed), sizeof(float32));
	return true;
}
