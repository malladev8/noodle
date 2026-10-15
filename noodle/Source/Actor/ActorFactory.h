#pragma once
#include "Core/NoodleCore.h"
#include "Actor.h"
#include <unordered_map>

typedef ActorComponent* (*ActorComponentCreator)(void);
typedef std::unordered_map<eComponentId, ActorComponentCreator> ActorComponentCreatorMap;

struct EngineContext;

class ActorFactory
{
public:
	ActorFactory();
	Actor* CreateActor(std::ifstream& sceneStream);

protected:
	ActorComponentCreatorMap m_ActorComponentCreators;
	virtual ActorComponent* CreateComponent(std::ifstream& bin, EngineContext& engineContext);

private:
	ActorId m_LastActorId;
	ActorId GetNextActorID() { return ++m_LastActorId; }

};