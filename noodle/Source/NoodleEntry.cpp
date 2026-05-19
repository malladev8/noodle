#include "NoodlePch.h"
#include "NoodleEntry.h"
#include "NoodleApp.h"
#include "NoodleInput.h"
#include "NoodleClock.h"

int NoodleMain()
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

		app->Run(deltaSeconds);
	}
	
	// Shutdown
	delete app;
	PlatformShutdown();
	return 0;
}