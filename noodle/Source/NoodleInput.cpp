#include "NoodlePch.h"
#include "NoodleInput.h"

void InputManager::BeginFrame()
{
	m_InputState.cursorDeltaX = 0;
	m_InputState.cursorDeltaY = 0;

	for (uint32 i = 0; i < (uint32)eKey::NUM_KEYS; ++i)
	{
		switch (m_InputState.keyStates[i])
		{
		case eKeyState::PRESSED:
			m_InputState.keyStates[i] = eKeyState::HELD;
			break;

		case eKeyState::RELEASED:
			m_InputState.keyStates[i] = eKeyState::UP;
			break;
		}
	}
}

bool InputManager::IsKeyDown(eKey key) const
{
	eKeyState state = m_InputState.keyStates[(uint32)key];
	return state == eKeyState::PRESSED || state == eKeyState::HELD;
}

bool InputManager::WasKeyPressed(eKey key) const
{
	return m_InputState.keyStates[(uint32)key] == eKeyState::PRESSED;
}

bool InputManager::WasKeyReleased(eKey key) const
{
	return m_InputState.keyStates[(uint32)key] == eKeyState::RELEASED;
}

void InputManager::GetCursorCoordinates(int32& outCursorX, int32& outCursorY) const
{
    outCursorX = m_InputState.cursorX;
    outCursorY = m_InputState.cursorY;
}

void InputManager::GetCursorDelta(int32& outCursorDeltaX, int32& outCursorDeltaY) const
{
    outCursorDeltaX = m_InputState.cursorDeltaX;
    outCursorDeltaY = m_InputState.cursorDeltaY;
}

void InputManager::OnKeyDown(eKey key)
{
    eKeyState& state = m_InputState.keyStates[(uint32)key];
    if (state == eKeyState::UP || state == eKeyState::RELEASED)
    {
        state = eKeyState::PRESSED;
    }
}

void InputManager::OnKeyUp(eKey key)
{
    m_InputState.keyStates[(uint32)key] = eKeyState::RELEASED;
}

void InputManager::OnCursorMove(int32 updatedX, int32 updatedY)
{
    m_InputState.cursorDeltaX = updatedX - m_InputState.cursorX;
    m_InputState.cursorX = updatedX;
    m_InputState.cursorDeltaY = updatedY - m_InputState.cursorY;
    m_InputState.cursorY = updatedY;
}

#if defined(WINDOWS)
static eKey sTranslateWinKey(WPARAM wParam)
{
    switch (wParam)
    {
    case 'A':
        return eKey::A;
    case 'B':
        return eKey::B;
    case 'C':
        return eKey::C;
    case 'D':
        return eKey::D;
    case 'E':
        return eKey::E;
    case 'F':
        return eKey::F;
    case 'G':
        return eKey::G;
    case 'H':
        return eKey::H;
    case 'I':
        return eKey::I;
    case 'J':
        return eKey::J;
    case 'K':
        return eKey::K;
    case 'L':
        return eKey::L;
    case 'M':
        return eKey::M;
    case 'N':
        return eKey::N;
    case 'O':
        return eKey::O;
    case 'P':
        return eKey::P;
    case 'Q':
        return eKey::Q;
    case 'R':
        return eKey::R;
    case 'S':
        return eKey::S;
    case 'T':
        return eKey::T;
    case 'U':
        return eKey::U;
    case 'V':
        return eKey::V;
    case 'W':
        return eKey::W;
    case 'X':
        return eKey::X;
    case 'Y':
        return eKey::Y;
    case 'Z':
        return eKey::Z;
    case VK_SHIFT:
        return eKey::SHIFT;
    case VK_CONTROL:
        return eKey::CTRL;
    case VK_MENU:
        return eKey::ALT;
    case VK_TAB:
        return eKey::TAB;
    case VK_ESCAPE:
        return eKey::ESC;
    case VK_SPACE:
        return eKey::SPACE;
    case VK_RETURN:
        return eKey::ENTER;
    default:
        return eKey::UNKNOWN;
    }
}

void InputManager::ProcessPlatformMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_KEYDOWN:
        {
            OnKeyDown(sTranslateWinKey(wParam));
            break;
        }
        case WM_KEYUP:
        {
            OnKeyUp(sTranslateWinKey(wParam));
            break;
        }
        case WM_LBUTTONDOWN:
        {
            OnKeyDown(eKey::MOUSE_L);
            break;
        }
        case WM_LBUTTONUP:
        {
            OnKeyUp(eKey::MOUSE_L);
            break;
        }
        case WM_RBUTTONDOWN:
        {
            OnKeyDown(eKey::MOUSE_R);
            break;
        }
        case WM_RBUTTONUP:
        {
            OnKeyUp(eKey::MOUSE_R);
            break;
        }
        case WM_MBUTTONDOWN:
        {
            OnKeyDown(eKey::MOUSE_M);
            break;
        }
        case WM_MBUTTONUP:
        {
            OnKeyUp(eKey::MOUSE_M);
            break;
        }
        case WM_MOUSEMOVE:
        {
            int32 x = (int32)GET_X_LPARAM(lParam);
            int32 y = (int32)GET_Y_LPARAM(lParam);
            OnCursorMove(x, y);
            break;
        }
    }
}
#endif