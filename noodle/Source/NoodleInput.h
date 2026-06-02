#pragma once
#include "Core/NoodleCore.h"

//namespace
//{
//	static const uint32 sMaxKeyInputs = 256;
//	static const uint32 sMaxPointerInputs = 3;
//}

enum class eKey : uint32
{
	UNKNOWN,
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

enum class eKeyState
{
	UP = 0,
	PRESSED,
	HELD,
	RELEASED
};

struct InputState
{
	eKeyState keyStates[(uint32)eKey::NUM_KEYS]{};
	int32 cursorX = 0;
	int32 cursorY = 0;
	int32 cursorDeltaX = 0;
	int32 cursorDeltaY = 0;
};

class InputManager
{
	friend class Engine;
public:
	void BeginFrame();

	bool IsKeyDown(eKey key) const;
	bool WasKeyPressed(eKey key) const;
	bool WasKeyReleased(eKey key) const;
	
	void GetCursorCoordinates(int32& x, int32& y) const;
	void GetCursorDelta(float32& deltaX, float32& deltaY) const;

	// Handle input messages for each supported platform
#if defined(WINDOWS)
	void ProcessPlatformMessage(UINT msg, WPARAM wParam, LPARAM lParam);
#endif

protected:
private:
	InputState m_InputState;

	void OnKeyDown(eKey key);
	void OnKeyUp(eKey key);
	void OnCursorMove(int32 updatedX, int32 updatedY);
};