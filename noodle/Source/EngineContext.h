#pragma once
#include "Core/NoodleCore.h"

struct EngineContext
{
	class EventManager& eventManager;
	class ResourceManager& resourceManager;
	class InputManager& inputManager;

	EngineContext(EventManager& events, ResourceManager& resources, InputManager& input)
		: eventManager(events), 
		  resourceManager(resources),
		  inputManager(input)
	{ }
};