#pragma once
#include "Core/NoodleCore.h"

namespace
{
	static const uint32 sMaxKeyInputs = 256;
	static const uint32 sMaxPointerInputs = 3;
}

namespace Input
{
	enum class eKey
	{
		UNKOWN,
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		SHIFT,
		CTRL,
		ALT,
		TAB,
		ESC,
		SPACE,
		ENTER,
		MOUSE_L,
		MOUSE_R,
		MOUSE_M,
		NUM_KEYS
	};

	enum class eButtonState
	{
		UP = 0,
		PRESSED,
		HELD,
		RELEASED
	};

	struct InputState
	{
		bool currentKeys[(uint64)eKey::NUM_KEYS]{};
		bool previousKeys[(uint64)eKey::NUM_KEYS]{};
		eButtonState keys[(uint64)eKey::NUM_KEYS]{};

		int32 x;
		int32 y;
		float32 deltaX;
		float32 deltaY;
	};

	InputState& GetMutableInput();
	const InputState& GetInput();
	void BeginFrame();
	void UpdateButtonStates();
}