#pragma once
#include "Core/NoodleCore.h"

namespace
{
	static const uint32 sMaxKeyInputs = 256;
	static const uint32 sMaxPointerInputs = 3;
}

namespace Input
{
	enum class eButtonState
	{
		UP = 0,
		PRESSED,
		HELD,
		RELEASED
	};

	enum class eMouseButtons
	{
		LEFT = 0,
		RIGHT,
		MIDDLE
	};

	struct KeyboardState
	{
		bool currentKeys[sMaxKeyInputs]{};
		bool previousKeys[sMaxKeyInputs]{};
		eButtonState keys[sMaxKeyInputs]{};
	};

	struct PointerState
	{
		int32 x;
		int32 y;
		float32 deltaX;
		float32 deltaY;
		bool currentButtons[sMaxPointerInputs]{};
		bool previousButtons[sMaxPointerInputs]{};
		eButtonState buttons[sMaxPointerInputs]{};
	};

	struct InputState
	{
		KeyboardState keyboardState;
		PointerState pointerState;
	};

	InputState& GetMutableInput();
	const InputState& GetInput();
	void BeginFrame();
	void EndFrame();
}