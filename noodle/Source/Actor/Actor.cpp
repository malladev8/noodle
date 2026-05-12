#include "../NoodlePch.h"
#include "Actor.h"

Actor::Actor(ActorId actorId)
	: m_ActorId (actorId)
{
}

Actor::~Actor()
{
}

bool Actor::Init(const char* binPath)
{
	return true;
}

void Actor::PostInit()
{
}

void Actor::Destroy()
{
}

void Actor::Update(int32 deltaMs)
{
	size_t numComponents = m_Components.size();
	for (size_t i = 0; i < numComponents; ++i)
	{
		m_Components[i]->Update(deltaMs);
	}
}

void Actor::AddComponent(ActorComponent* component)
{
	N_ASSERT(component != nullptr, "Cannot add null component");

	eComponentId componentId = component->GetComponentId();
	auto it = m_ComponentLookup.find(componentId);
	N_ASSERT(it == m_ComponentLookup.end(), "Actors can only have one of each component type.");

	m_Components.push_back(std::unique_ptr<ActorComponent>(component));
	size_t numComponents = m_Components.size();
	m_ComponentLookup[componentId] = m_Components[numComponents - 1].get();
}
