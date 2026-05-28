#pragma once
#include "Core/NoodleCore.h"
#include "Event/EventManager.h"
#include "Resource/ResourceManager.h"

class NoodleApp;

// Platform functions should be implemented by platform entry point.
// See WinEntry.cpp for examples
bool PlatformCreateWindow(const NoodleWindowDesc& windowDesc);
void PlatformInitLogger();
void PlatformDispatchMessages();
bool PlatformShouldExit();
void PlatformShutdown();

class Engine
{
public:
	Engine(const Engine& other) = delete;
	Engine& operator=(const Engine&) = delete;

	static Engine& Get();
	void Run(std::unique_ptr<NoodleApp> app);

	EventManager& GetEventManager() { return m_EventManager; }
	ResourceManager& GetResourceManager() { return m_ResourceManager; }

protected:

private:
	EventManager m_EventManager;
	// Renderer will live here eventually
	ResourceManager m_ResourceManager;

	std::unique_ptr<NoodleApp> m_App = nullptr;

	Engine();
};