#include "NoodlePch.h"
#include "NoodleInput.h"

static const uint32 sMaxInputs = 256;
static bool sCurrentInput[sMaxInputs];
static bool sPreviousInput[sMaxInputs];

#if defined(WINDOWS)
LRESULT CALLBACK WndProc(
	HWND hwnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
	{
		// wParam = virtual key code
		if (wParam == 'W')
		{
			// W key pressed
		}
		break;
	}
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
#endif

void NoodleInput::Update()
{
	memcpy(sPreviousInput, sCurrentInput, sizeof(sCurrentInput));
}

bool NoodleInput::InputPressed(uint32 key)
{
	NASSERT(key < sMaxInputs, "Invalid Input");
	return sCurrentInput[key] && !sPreviousInput[key];
}

bool NoodleInput::InputReleased(uint32 key)
{
	NASSERT(key < sMaxInputs, "Invalid Input");
	return sPreviousInput[key] && !sCurrentInput[key];
}
