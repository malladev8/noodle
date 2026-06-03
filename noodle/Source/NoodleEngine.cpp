#include "NoodlePch.h"
#include "NoodleEngine.h"
#include "NoodleApp.h"
#include "NoodleClock.h"
#include "Rendering/IRenderer.h"

Engine::Engine()
	: m_EventManager(EventManager("GlobalEventManager")),
	  m_Renderer(nullptr),
	  m_ResourceManager(ResourceManager()),
	  m_InputManager(InputManager()),
	  m_EngineContext(
		  {
			  m_EventManager, 
			  m_Renderer.get(),
			  m_ResourceManager,
			  m_InputManager,
		  }
	  )
{
}

Engine& Engine::Get()
{
	static Engine sEngineInstance;
	return sEngineInstance;
}

void Engine::BeginFrame()
{
	m_InputManager.BeginFrame();
}

void Engine::Update(float32 deltaSeconds)
{
	PlatformDispatchMessages();
	m_EventManager.Update(deltaSeconds);
	m_App->Update(deltaSeconds);
}

void Engine::Render()
{
	m_Renderer->BeginFrame();
	m_App->Render(*m_Renderer.get());
	m_Renderer->RenderFrame();
	m_Renderer->EndFrame();
	m_Renderer->Present();
}

void Engine::Run(std::unique_ptr<NoodleApp> app,
	             std::unique_ptr<IRenderer> renderer)
{
	// Initialization
	Clock clock;
	clock.Reset();

	PlatformInitLogger();

	NoodleWindowDesc windowDesc
	{
		1280,
		720,
		"Noodle"
	};

	void* hwnd = nullptr;
	PlatformCreateWindow(windowDesc, hwnd);
	
	m_Renderer = std::move(renderer);
	m_Renderer->Initialize(hwnd);

	m_App = std::move(app);
	m_App->Initialize();

	// Game Loop
	while (!PlatformShouldExit())
	{
		clock.Tick();
		float32 deltaSeconds = clock.GetDeltaSeconds();
		BeginFrame();
		Update(deltaSeconds);
		Render();
	}
	
	// Shutdown
	m_App->Shutdown();
	m_Renderer->Shutdown();
	PlatformShutdown();
}