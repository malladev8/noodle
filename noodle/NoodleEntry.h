#pragma once
#include "Core/NoodleCore.h"

bool PlatformCreateWindow(const NoodleWindowDesc& windowDesc);
void PlatformDispatchMessages();
bool PlatformShouldExit();
void PlatformShutdown();

int NoodleMain();