#include "NoodlePch.h"
#include "NoodleEntry.h"
#include "NoodleInput.h"

#if defined(WINDOWS)
static HWND sHwnd = nullptr;
static bool sRunning = true;

LRESULT CALLBACK Win32WndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
    case WM_DESTROY:
    {
        sRunning = false;
        PostQuitMessage(0);
        return 0;
    }
    case WM_SIZE:
    {
        // TODO: Notify renderer about resize
        return 0;
    }
    case WM_KEYDOWN:
    {
        Input::GetMutableInput().keyboardState.currentKeys[wParam] = true;
        return 0;
    }
    case WM_KEYUP:
    {
        Input::GetMutableInput().keyboardState.currentKeys[wParam] = false;
        return 0;
    }
    case WM_LBUTTONDOWN:
    {
        Input::GetMutableInput().pointerState.currentButtons[(int)Input::eMouseButtons::LEFT] = true;
        return 0;
    }
    case WM_LBUTTONUP:
    {
        Input::GetMutableInput().pointerState.currentButtons[(int)Input::eMouseButtons::LEFT] = false;
        return 0;
    }
    case WM_RBUTTONDOWN:
    {
        Input::GetMutableInput().pointerState.currentButtons[(int)Input::eMouseButtons::RIGHT] = true;
        return 0;
    }
    case WM_RBUTTONUP:
    {
        Input::GetMutableInput().pointerState.currentButtons[(int)Input::eMouseButtons::RIGHT] = false;
        return 0;
    }
    case WM_MOUSEMOVE:
    {
        Input::InputState& input = Input::GetMutableInput();

        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);

        input.pointerState.deltaX = (float32)x - input.pointerState.x;
        input.pointerState.deltaY = (float32)y - input.pointerState.y;
        input.pointerState.x = x;
        input.pointerState.y = y;

        return 0;
    }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool PlatformCreateWindow(const NoodleWindowDesc& windowDesc)
{
    HINSTANCE instance = GetModuleHandle(nullptr);

    WNDCLASS wc = {};
    wc.lpfnWndProc = Win32WndProc;
    wc.hInstance = instance;
    wc.lpszClassName = L"EngineWindowClass";

    RegisterClass(&wc);

    sHwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        L"Noodle",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowDesc.width,
        windowDesc.height,
        nullptr,
        nullptr,
        instance,
        nullptr);

    ShowWindow(sHwnd, SW_SHOW);

    return sHwnd != nullptr;
}

void PlatformDispatchMessages()
{
    MSG msg;

    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

bool PlatformShouldExit()
{
    return !sRunning;
}

void PlatformShutdown()
{
    if (sHwnd)
    {
        DestroyWindow(sHwnd);
        sHwnd = nullptr;
    }
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    return NoodleMain();
}
#endif