#include "NoodlePch.h"
#include "NoodleEntry.h"
#include "NoodleApp.h"

int NoodleMain()
{
	NoodleWindowDesc windowDesc
	{
		1280,
		720,
		"Noodle"
	};

	PlatformCreateWindow(windowDesc);

	NoodleApp app;
	while (!PlatformShouldExit())
	{
		PlatformDispatchMessages();
		app.Run();
	}
	
	PlatformShutdown();
	return 0;
}