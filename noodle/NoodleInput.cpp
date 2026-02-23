#include "NoodlePch.h"
#include "NoodleInput.h"

namespace Input
{
	static InputState sInputState;

	InputState& GetMutableInput()
	{
		return sInputState;
	}

	const InputState& GetInput()
	{
		return sInputState;
	}

	void BeginFrame()
	{
		sInputState.pointerState.deltaX = 0.0f;
		sInputState.pointerState.deltaY = 0.0f;

		memcpy(sInputState.keyboardState.previousKeys, sInputState.keyboardState.currentKeys, sizeof(sInputState.keyboardState.currentKeys));
		memcpy(sInputState.pointerState.previousButtons, sInputState.pointerState.currentButtons, sizeof(sInputState.pointerState.currentButtons));
	}

	static void sSetButtonState(eButtonState& outState, bool current, bool previous)
	{
		if (current && !previous)
			outState = eButtonState::PRESSED;
		else if (current && previous)
			outState = eButtonState::HELD;
		else if (!current && previous)
			outState = eButtonState::RELEASED;
		else
			outState = eButtonState::UP;
	}

	void EndFrame()
	{
		for (uint32 i = 0; i < sMaxKeyInputs; ++i)
		{
			bool prev = sInputState.keyboardState.previousKeys[i];
			bool curr = sInputState.keyboardState.currentKeys[i];
			sSetButtonState(sInputState.keyboardState.keys[i], curr, prev);
		}

		for (uint32 i = 0; i < sMaxPointerInputs; ++i)
		{
			bool prev = sInputState.pointerState.previousButtons[i];
			bool curr = sInputState.pointerState.currentButtons[i];
			sSetButtonState(sInputState.pointerState.buttons[i], curr, prev);
		}
	}
}