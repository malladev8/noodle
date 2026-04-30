#include "NoodlePch.h"
#include "NoodleEntry.h"
#include "NoodleApp.h"
#include "NoodleInput.h"

int NoodleMain()
{
	// Initialization
	NoodleWindowDesc windowDesc
	{
		1280,
		720,
		"Noodle"
	};
	
	PlatformInitLogger();
	PlatformCreateWindow(windowDesc);
	NoodleApp app;
	
	// Game Loop
	while (!PlatformShouldExit())
	{
		Input::BeginFrame();
		PlatformDispatchMessages();
		Input::UpdateButtonStates();

		app.Run();
	}
	
	// Shutdown
	PlatformShutdown();
	return 0;
}