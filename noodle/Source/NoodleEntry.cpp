#include "NoodlePch.h"
#include "NoodleEntry.h"
#include "NoodleApp.h"
#include "NoodleInput.h"
#include "NoodleClock.h"
#include "Event/EventManager.h"

int NoodleMain()
{
	// Initialization
	Clock clock;
	clock.Reset();

	EventManager* globalEventManager = EventManager::GetGlobalEventManager();

	NoodleWindowDesc windowDesc
	{
		1280,
		720,
		"Noodle"
	};
	
	PlatformInitLogger();
	PlatformCreateWindow(windowDesc);
	NoodleApp* app = N_NEW(NoodleApp);
	app->Init();
	

	// Game Loop
	while (!PlatformShouldExit())
	{
		clock.Tick();
		float deltaSeconds = clock.GetDeltaSeconds();

		Input::BeginFrame();
		PlatformDispatchMessages();
		Input::UpdateButtonStates();
		globalEventManager->Update(deltaSeconds);
		app->Run(deltaSeconds);
	}
	
	// Shutdown
	delete app;
	PlatformShutdown();
	return 0;
}