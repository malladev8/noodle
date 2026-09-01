#pragma once
#include "Core/NoodleCore.h"
#include "EngineContext.h"
#include "Event/EventManager.h"
#include "NoodleInput.h"
#include "NoodleWindow.h"
#include "Resource/ResourceManager.h"
#include "NoodleClock.h"

class NoodleApp;
class IRenderer;
class Scene;

// Platform functions should be implemented by platform entry point.
// See WinEntry.cpp for examples
bool PlatformCreateWindow(const Window& windowDesc, void*& outHwnd);
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
	void Initialize();
	void BeginFrame();
	void Update(float32 deltaSeconds);
	void Render();
	void EndFrame();
	void Shutdown();

	Scene* LoadScene(const char* scenePath);
	float32 TickClock();

	EngineContext& GetContext() { return m_EngineContext; }

protected:

private:
	EventManager m_EventManager;
	Window m_Window;
	std::unique_ptr<IRenderer> m_Renderer = nullptr;
	ResourceManager m_ResourceManager;
	InputManager m_InputManager;
	Clock m_Clock;

	EngineContext m_EngineContext;

	std::unique_ptr<Scene> m_ActiveScene;

	Engine();
};