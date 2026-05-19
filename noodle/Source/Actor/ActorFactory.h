#pragma once
#include "../Core/NoodleCore.h"
#include "Actor.h"
#include <unordered_map>

typedef ActorComponent* (*ActorComponentCreator)(void);
typedef std::unordered_map<eComponentId, ActorComponentCreator> ActorComponentCreatorMap;

class ActorFactory
{
public:
	ActorFactory();
	Actor* CreateActor(const char* binPath, std::ifstream* sceneStream = nullptr);

protected:
	ActorComponentCreatorMap m_ActorComponentCreators;
	virtual ActorComponent* CreateComponent(std::ifstream& bin);

private:
	ActorId m_LastActorId;
	ActorId GetNextActorID() { return ++m_LastActorId; }

};