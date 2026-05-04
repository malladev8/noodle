#pragma once
#include "../Core/NoodleCore.h"
#include <map>

class Actor;
class ActorComponent;

typedef ActorComponent* (*ActorComponentCreator)(void);
typedef std::map<std::string, ActorComponentCreator> ActorComponentCreatorMap;

typedef unsigned long ActorId;
typedef std::shared_ptr<Actor> StrongActorPtr;
typedef std::shared_ptr<ActorComponent> StrongActorComponentPtr;

class ActorFactory
{
public:
	ActorFactory();
	StrongActorPtr CreateActor(const char* actorResource);

protected:
	ActorComponentCreatorMap m_ActorComponentCreators;
	//virtual StrongActorComponentPtr CreateComponent(TiXmlElement* data);

private:
	ActorId m_LastActorId;
	ActorId GetNextActorID() { return ++m_LastActorId; }
};