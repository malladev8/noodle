#pragma once
#include "../Core/NoodleCore.h"
#include "Components/ActorComponent.h"
#include <unordered_map>
#include <vector>


typedef uint32 ActorId;

class Actor
{
	friend class ActorFactory;

public:
	explicit Actor(ActorId actorId);
	virtual ~Actor();

	bool Init(const char* binPath);
	void PostInit();
	void Destroy();
	void Update(int32 deltaMs);

	ActorId GetId() const { return m_ActorId; }
	
	template<class ComponentType>
	ComponentType* GetComponent()
	{
		auto it = m_ComponentLookup.find(ComponentType::GetComponentId());
		if (it != m_ComponentLookup.end())
		{
			return static_cast<ComponentType*>(it->second);
		}
		return nullptr;
	}

protected:

private:
	ActorId m_ActorId;
	std::vector<std::unique_ptr<ActorComponent>> m_Components;
	std::unordered_map<eComponentId, ActorComponent*> m_ComponentLookup;

	// This is called by the ActorFactory. No one else should be adding components
	void AddComponent(ActorComponent* component);
};