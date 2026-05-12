#include "../NoodlePch.h"
#include "ActorFactory.h"
#include "../Math/NoodleMath.h"

#include "Components/Transform.h"

#include <fstream>

static ActorComponent* sCreateTransformComponent()
{
	return N_NEW Transform;
}

ActorFactory::ActorFactory()
	: m_LastActorId(0)
{
	m_ActorComponentCreators[eComponentId::COMPONENT_TRANSFORM] = sCreateTransformComponent;
}

Actor* ActorFactory::CreateActor(const char* binPath)
{
	std::ifstream bin(binPath, std::ios::binary);
	if (!bin.is_open())
	{
		N_LOG("Failed to open Actor binary file %s", binPath);
		return N_NEW Actor(-1);
	}

	Actor* actor = N_NEW Actor(GetNextActorID());

	if (!actor->Init(binPath))
	{
		N_LOG("Failed to initialize actor");
		bin.close();
		return actor;
	}

	// Create components
	uint8 numComponents = 0;
	bin.read(reinterpret_cast<char*>(&numComponents), sizeof(uint8));

	for (uint8 i = 0; i < numComponents; ++i)
	{
		ActorComponent* component = CreateComponent(bin);
		actor->AddComponent(component);
		component->SetOwner(actor);
	}

	actor->PostInit();
	bin.close();
	return actor;
}

ActorComponent* ActorFactory::CreateComponent(std::ifstream& bin)
{
	eComponentId componentId;
	bin.read(reinterpret_cast<char*>(&componentId), sizeof(eComponentId));
	ActorComponent* component = m_ActorComponentCreators[componentId]();
	component->Init(bin);
	return component;
}