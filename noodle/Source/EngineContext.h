#pragma once
#include "Core/NoodleCore.h"

struct EngineContext
{
	class EventManager& eventManager;
	class ResourceManager& resourceManager;

	EngineContext(EventManager& events, ResourceManager& resources)
		: eventManager(events), resourceManager(resources)
	{ }
};