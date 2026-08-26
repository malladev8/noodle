#include "NoodlePch.h"
#include "Controller.h"

void Controller::Possess(Actor* actor)
{
	N_ASSERT(m_PossessedActor == nullptr, "Controller is already possessing an actor.");
	N_ASSERT(actor != nullptr, "Controller cannot possess null actor");
	m_PossessedActor = actor;
}

void Controller::UnPossess()
{
	m_PossessedActor = nullptr;
}
