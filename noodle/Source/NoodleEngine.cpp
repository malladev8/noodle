#include "NoodlePch.h"
#include "NoodleEngine.h"
#include "NoodleApp.h"
#include "NoodleInput.h"
#include "NoodleClock.h"

Engine::Engine()
	: m_EventManager(EventManager("GlobalEventManager")),
	  m_ResourceManager(ResourceManager()),
	  m_EngineContext(
		  {
			  m_EventManager, 
			  m_ResourceManager
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

		Input::BeginFrame();
		PlatformDispatchMessages();
		Input::UpdateButtonStates();
		m_EventManager.Update(deltaSeconds);
		m_App->Run(deltaSeconds);
	}
	
	// Shutdown
	m_App->Shutdown();
	PlatformShutdown();
}