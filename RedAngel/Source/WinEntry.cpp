#include "NoodlePch.h"
#include "NoodleEngine.h"
#include "NoodleInput.h"
#include "NoodleWindow.h"
#include "Event/EventManager.h"
#include "Event/WindowResizeEvent.h"
#include "RedAngelApp.h"
#include "EngineContext.h"
#include "Rendering/D3D12/D3D12Renderer.h"

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
            // For future reference, wParam contains resize type. i.e. SIZE_RESTORED, SIZE_MINIMIZED, SIZE_MAXIMIZED

            uint32 width = static_cast<uint32>(LOWORD(lParam));
            uint32 height = static_cast<uint32>(HIWORD(lParam));
            WindowResizeEvent event(width, height);
            Engine::Get().GetContext().eventManager.QueueEvent(event);
            return 0;
        }
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MOUSEMOVE:
        {
            Engine::Get().GetContext().inputManager.ProcessPlatformMessage(msg, wParam, lParam);
            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static void Win32LogOutput(const char* msg)
{
    printf("%s\n", msg);
    OutputDebugStringA(msg);
    OutputDebugStringA("\n");
}

void PlatformInitLogger()
{
    LogRegisterCallback(Win32LogOutput);
}

#if defined(DEBUG)
void CreateDebugConsole()
{
    AllocConsole();

    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);

    printf("Noodle Console\n");
}
#endif

bool PlatformCreateWindow(const Window& windowDesc, void*& outHwnd)
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
        L"Red Angel",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowDesc.GetWidth(),
        windowDesc.GetHeight(),
        nullptr,
        nullptr,
        instance,
        nullptr);

    ShowWindow(sHwnd, SW_SHOW);
    outHwnd = sHwnd;

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
#if defined(DEBUG)
    FreeConsole();
#endif
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
#if defined(DEBUG)
    CreateDebugConsole();
#endif
    Engine& engine = Engine::Get();
    engine.Run(std::make_unique<RedAngelApp>(engine.GetContext()), std::make_unique<D3D12Renderer>());
    return 0;
}
#endif