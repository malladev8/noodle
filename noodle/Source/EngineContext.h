#pragma once
#include "Core/NoodleCore.h"

struct EngineContext
{
	class EventManager& eventManager;
	class IRenderer* renderer;
	class ResourceManager& resourceManager;
	class InputManager& inputManager;
	class Window& window;
	class Clock& clock;

	EngineContext(EventManager& events, IRenderer* inRenderer, ResourceManager& resources, InputManager& input, Window& inWindow, Clock& inClock)
		: eventManager(events), 
		  renderer(inRenderer),
		  resourceManager(resources),
		  inputManager(input),
		  window(inWindow),
		  clock(inClock)
	{ }
};