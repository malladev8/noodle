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
		sInputState.deltaX = 0.0f;
		sInputState.deltaY = 0.0f;

		memcpy(sInputState.previousKeys, sInputState.currentKeys, sizeof(sInputState.currentKeys));
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
		for (uint32 i = 0; i < (uint64)eKey::NUM_KEYS; ++i)
		{
			bool prev = sInputState.previousKeys[i];
			bool curr = sInputState.currentKeys[i];
			sSetButtonState(sInputState.keys[i], curr, prev);
		}
	}
}