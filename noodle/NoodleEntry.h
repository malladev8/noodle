#pragma once
#include "Core/NoodleCore.h"

// Platform functions should be implemented by platform entry point.
// See WinEntry.cpp for examples
bool PlatformCreateWindow(const NoodleWindowDesc& windowDesc);
void PlatformInitLogger();
void PlatformDispatchMessages();
bool PlatformShouldExit();
void PlatformShutdown();

int NoodleMain();