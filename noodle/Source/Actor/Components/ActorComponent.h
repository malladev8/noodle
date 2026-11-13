#pragma once
#include "Core/NoodleCore.h"
#include "ComponentIds.h"

class Actor;
class IRenderer;
struct EngineContext;
struct WorldContext;

class ActorComponent
{
	friend class ActorFactory;

public:
	virtual ~ActorComponent() {}

	virtual eComponentId GetComponentId() const = 0;
	virtual void PostInit(const WorldContext&) {}
	virtual void Destroy() {}
	virtual void Update(float deltaSeconds) {}
	virtual void SubmitRenderCommands(IRenderer& IRenderer) const {}
	Actor* const GetOwner() const { return m_Owner; }

protected:
	Actor* m_Owner = nullptr;

private:
	virtual bool Init(std::ifstream& bin, EngineContext& engineContext) = 0;
	void SetOwner(Actor* owner) { m_Owner = owner; }
};
