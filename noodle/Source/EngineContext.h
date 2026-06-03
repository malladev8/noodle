#pragma once
#include "Core/NoodleCore.h"

struct EngineContext
{
	class EventManager& eventManager;
	class IRenderer* renderer;
	class ResourceManager& resourceManager;
	class InputManager& inputManager;

	EngineContext(EventManager& events, IRenderer* inRenderer, ResourceManager& resources, InputManager& input)
		: eventManager(events), 
		  renderer(inRenderer),
		  resourceManager(resources),
		  inputManager(input)
	{ }
};