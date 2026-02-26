#include "NoodlePch.h"
#include "NoodleEntry.h"
#include "NoodleApp.h"
#include "NoodleInput.h"

int NoodleMain()
{
	NoodleWindowDesc windowDesc
	{
		1280,
		720,
		"Noodle"
	};

	PlatformCreateWindow(windowDesc);
	PlatformInitLogger();

	NoodleApp app;
	while (!PlatformShouldExit())
	{
		Input::BeginFrame();
		PlatformDispatchMessages();
		Input::UpdateButtonStates();

		app.Run();
	}
	
	PlatformShutdown();
	return 0;
}