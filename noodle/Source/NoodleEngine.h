#pragma once
#include "Core/NoodleCore.h"
#include "Event/EventManager.h"
#include "Resource/ResourceManager.h"
#include "NoodleInput.h"
#include "EngineContext.h"

class NoodleApp;
class IRenderer;

// Platform functions should be implemented by platform entry point.
// See WinEntry.cpp for examples
bool PlatformCreateWindow(const NoodleWindowDesc& windowDesc, void*& outHwnd);
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
	void Run(std::unique_ptr<NoodleApp> app, std::unique_ptr<IRenderer> renderer);

	EngineContext& GetContext() { return m_EngineContext; }

protected:

private:
	EventManager m_EventManager;
	std::unique_ptr<IRenderer> m_Renderer = nullptr;
	ResourceManager m_ResourceManager;
	InputManager m_InputManager;

	EngineContext m_EngineContext;

	std::unique_ptr<NoodleApp> m_App = nullptr;

	Engine();
	void BeginFrame();
	void Update(float32 deltaSeconds);
	void Render();
};