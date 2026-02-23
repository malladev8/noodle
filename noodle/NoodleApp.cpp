#include "NoodlePch.h"
#include "NoodleApp.h"

static const size_t APP_SIZE = 1_GB;

NoodleApp::NoodleApp()
	:ApplicationAllocator(APP_SIZE)
{
}

NoodleApp::~NoodleApp()
{
}

void NoodleApp::Run()
{
	
}
