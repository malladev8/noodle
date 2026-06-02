#include "NoodlePch.h"
#include "NoodleEngine.h"
#include "NoodleApp.h"
#include "NoodleClock.h"

Engine::Engine()
	: m_EventManager(EventManager("GlobalEventManager")),
	  m_ResourceManager(ResourceManager()),
	  m_InputManager(InputManager()),
	  m_EngineContext(
		  {
			  m_EventManager, 
			  m_ResourceManager,
			  m_InputManager
		  }
	  )
{
}

Engine& Engine::Get()
{
	static Engine sEngineInstance;
	return sEngineInstance;
}

void Engine::Run(std::unique_ptr<NoodleApp> app)
{
	// Initialization
	Clock clock;
	clock.Reset();

	NoodleWindowDesc windowDesc
	{
		1280,
		720,
		"Noodle"
	};
	
	PlatformInitLogger();
	PlatformCreateWindow(windowDesc);
	
	m_App = std::move(app);
	m_App->Init();

	// Game Loop
	while (!PlatformShouldExit())
	{
		clock.Tick();
		float deltaSeconds = clock.GetDeltaSeconds();

		m_InputManager.BeginFrame();
		PlatformDispatchMessages();
		m_EventManager.Update(deltaSeconds);
		m_App->Run(deltaSeconds);
		// Render Here
	}
	
	// Shutdown
	m_App->Shutdown();
	PlatformShutdown();
}