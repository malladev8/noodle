#pragma once
#include "Core/NoodleCore.h"
#include "ComponentIds.h"

class Actor;
struct EngineContext;

class ActorComponent
{
	friend class ActorFactory;

public:
	virtual ~ActorComponent() {}

	virtual void PostInit() {}
	virtual void Update(float deltaSeconds) {}
	virtual eComponentId GetComponentId() const = 0;

protected:
	Actor* m_Owner = nullptr;

private:
	virtual bool Init(std::ifstream& bin, EngineContext& engineContext) = 0;
	void SetOwner(Actor* owner) { m_Owner = owner; }
};
