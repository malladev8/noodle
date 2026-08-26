#include "NoodlePch.h"
#include "ActorFactory.h"
#include "Components/Sprite.h"
#include "Components/Transform.h"
#include "Components/Camera.h"
#include "Math/NoodleMath.h"
#include "NoodleEngine.h"

#include <fstream>

static ActorComponent* sCreateTransformComponent()
{
	return N_NEW Transform;
}

static ActorComponent* sCreateSpriteComponent()
{
	return N_NEW Sprite;
}

static ActorComponent* sCreateCameraComponent()
{
	return N_NEW Camera;
}

ActorFactory::ActorFactory()
	: m_LastActorId(0)
{
	m_ActorComponentCreators[eComponentId::COMPONENT_TRANSFORM] = sCreateTransformComponent;
	m_ActorComponentCreators[eComponentId::COMPONENT_SPRITE] = sCreateSpriteComponent;
	m_ActorComponentCreators[eComponentId::COMPONENT_CAMERA] = sCreateCameraComponent;
}

Actor* ActorFactory::CreateActor(const char* binPath, std::ifstream* sceneStream)
{
	std::ifstream bin(binPath, std::ios::binary);
	if (!bin.is_open())
	{
		N_LOG("Failed to open Actor binary file %s", binPath);
		return N_NEW Actor(-1);
	}

	AssetId actorPrefabId = 0;
	bin.read(reinterpret_cast<char*>(&actorPrefabId), sizeof(AssetId));

	// Currently only maintaining GetNextActorID() to support runtime actor creation
	// Otherwise ActorId comes from scene data
	ActorId newActorId = GetNextActorID();
	if (sceneStream != nullptr)
	{
		sceneStream->read(reinterpret_cast<char*>(&newActorId), sizeof(newActorId));
	}

	Actor* actor = N_NEW Actor(newActorId);

	if (!actor->Init(binPath))
	{
		N_LOG("Failed to initialize actor");
		bin.close();
		return actor;
	}

	// Check for parent Actor and cache parent ID
	bool hasParent = false;
	if (sceneStream != nullptr)
	{
		sceneStream->read(reinterpret_cast<char*>(&hasParent), sizeof(hasParent));
	}
	if (hasParent)
	{
		ActorId parentId = UINT_MAX;
		sceneStream->read(reinterpret_cast<char*>(&parentId), sizeof(parentId));
		actor->m_ParentId = parentId;
	}

	EngineContext& context = Engine::Get().GetContext();

	// If initialized from a scene, create transform component from scene transform data
	if (sceneStream != nullptr)
	{
		ActorComponent* component = CreateComponent(*sceneStream, context);
		actor->AddComponent(component);
		component->SetOwner(actor);
	}

	// Create components
	uint8 numComponents = 0;
	bin.read(reinterpret_cast<char*>(&numComponents), sizeof(uint8));

	for (uint8 i = 0; i < numComponents; ++i)
	{
		ActorComponent* component = CreateComponent(bin, context);
		actor->AddComponent(component);
		component->SetOwner(actor);
	}

	actor->PostInit();
	bin.close();
	return actor;
}

ActorComponent* ActorFactory::CreateComponent(std::ifstream& bin, EngineContext& engineContext)
{
	eComponentId componentId;
	bin.read(reinterpret_cast<char*>(&componentId), sizeof(eComponentId));
	ActorComponent* component = m_ActorComponentCreators[componentId]();
	component->Init(bin, engineContext);
	return component;
}