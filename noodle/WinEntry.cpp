#include "NoodlePch.h"
#include "NoodleEntry.h"

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