#include "NoodlePch.h"
#include "ActorFactory.h"
#include "Components/Sprite.h"
#include "Components/SpriteAnimator.h"
#include "Components/Transform.h"
#include "Components/Camera.h"
#include "Components/Movement.h"
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

static ActorComponent* sCreateSpriteAnimatorComponent()
{
	return N_NEW SpriteAnimator;
}

static ActorComponent* sCreateCameraComponent()
{
	return N_NEW Camera;
}

static ActorComponent* sCreateMovementComponent()
{
	return N_NEW Movement;
}

ActorFactory::ActorFactory()
	: m_LastActorId(0)
{
	m_ActorComponentCreators[eComponentId::COMPONENT_TRANSFORM] = sCreateTransformComponent;
	m_ActorComponentCreators[eComponentId::COMPONENT_SPRITE] = sCreateSpriteComponent;
	m_ActorComponentCreators[eComponentId::COMPONENT_SPRITE_ANIMATOR] = sCreateSpriteAnimatorComponent;
	m_ActorComponentCreators[eComponentId::COMPONENT_CAMERA] = sCreateCameraComponent;
	m_ActorComponentCreators[eComponentId::COMPONENT_MOVEMENT] = sCreateMovementComponent;
}

Actor* ActorFactory::CreateActor(std::ifstream& sceneStream)
{
	// Currently only maintaining GetNextActorID() to support runtime actor creation
	// Otherwise ActorId comes from scene data
	ActorId newActorId = GetNextActorID();
	sceneStream.read(reinterpret_cast<char*>(&newActorId), sizeof(newActorId));

	Actor* actor = N_NEW Actor(newActorId);
	actor->Init();

	// Check for parent Actor and cache parent ID
	bool hasParent = false;
	sceneStream.read(reinterpret_cast<char*>(&hasParent), sizeof(hasParent));
	if (hasParent)
	{
		ActorId parentId = UINT_MAX;
		sceneStream.read(reinterpret_cast<char*>(&parentId), sizeof(parentId));
		actor->m_ParentId = parentId;
	}

	// Check if actor will need a Controller
	bool hasController = false;
	sceneStream.read(reinterpret_cast<char*>(&hasController), sizeof(hasController));
	if (hasController)
	{
		uint8 controllerTypeLength = 0;
		sceneStream.read(reinterpret_cast<char*>(&controllerTypeLength), sizeof(controllerTypeLength));
		actor->m_ControllerType.resize(controllerTypeLength);
		sceneStream.read(reinterpret_cast<char*>(actor->m_ControllerType.data()), controllerTypeLength);
	}

	EngineContext& context = Engine::Get().GetContext();

	// Create components
	uint8 numComponents = 0;
	sceneStream.read(reinterpret_cast<char*>(&numComponents), sizeof(numComponents));

	for (uint8 i = 0; i < numComponents; ++i)
	{
		ActorComponent* component = CreateComponent(sceneStream, context);
		actor->AddComponent(component);
		component->SetOwner(actor);
	}

	actor->PostInit();
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