#include "../NoodlePch.h"
#include "ActorFactory.h"

ActorFactory::ActorFactory()
	: m_LastActorId(0)
{
}

StrongActorPtr ActorFactory::CreateActor(const char* actorResource)
{
	return StrongActorPtr();
}
