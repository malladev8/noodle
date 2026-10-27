#include "NoodlePch.h"
#include "Actor.h"
#include "Components/Transform.h"
#include <algorithm>

Actor::Actor(ActorId actorId)
	: m_ActorId (actorId), m_ParentId(std::nullopt)
{
}

Actor::~Actor()
{
	Destroy();
}

void Actor::PostInit()
{
	for (auto it = m_Components.begin(); it != m_Components.end(); ++it)
	{
		(*it)->PostInit();
	}
}

void Actor::Destroy()
{
	if (m_Destroyed)
	{
		return;
	}

	// Teardown components in reverse construction order.
	for (auto it = m_Components.rbegin(); it != m_Components.rend(); ++it)
	{
		(*it)->Destroy();
	}

	m_Destroyed = true;
}

void Actor::Update(float deltaSeconds)
{
	size_t numComponents = m_Components.size();
	for (size_t i = 0; i < numComponents; ++i)
	{
		m_Components[i]->Update(deltaSeconds);
	}
}

void Actor::SubmitRenderCommands(IRenderer& IRenderer) const
{
	size_t numComponents = m_Components.size();
	for (size_t i = 0; i < numComponents; ++i)
	{
		m_Components[i]->SubmitRenderCommands(IRenderer);
	}
}

void Actor::SetParent(Actor* parent)
{
	N_ASSERT(parent != nullptr, "Parent Actor cannot be null.");
	m_Parent = parent;
	m_ParentId = m_Parent->GetId();
	parent->m_Children.push_back(this);
}

void Actor::UnParent()
{
	if (m_Parent)
	{
		std::erase(m_Parent->m_Children, this);
	}
	m_Parent = nullptr;
	m_ParentId = std::nullopt;
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

	// Transform component is accessed frequently, cache it
	if (componentId == eComponentId::COMPONENT_TRANSFORM)
	{
		m_CachedTransform = static_cast<Transform*>(component);
	}
}
