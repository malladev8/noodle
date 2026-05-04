#include "NoodlePch.h"
#include "NoodleEntry.h"
#include "NoodleInput.h"

#if defined(WINDOWS)
static HWND sHwnd = nullptr;
static bool sRunning = true;

Input::eKey TranslateWinKey(WPARAM wParam)
{
    switch (wParam)
    {
    case 'A':
        return Input::eKey::A;
    case 'B':
        return Input::eKey::B;
    case 'C':
        return Input::eKey::C;
    case 'D':
        return Input::eKey::D;
    case 'E':
        return Input::eKey::E;
    case 'F':
        return Input::eKey::F;
    case 'G':
        return Input::eKey::G;
    case 'H':
        return Input::eKey::H;
    case 'I':
        return Input::eKey::I;
    case 'J':
        return Input::eKey::J;
    case 'K':
        return Input::eKey::K;
    case 'L':
        return Input::eKey::L;
    case 'M':
        return Input::eKey::M;
    case 'N':
        return Input::eKey::N;
    case 'O':
        return Input::eKey::O;
    case 'P':
        return Input::eKey::P;
    case 'Q':
        return Input::eKey::Q;
    case 'R':
        return Input::eKey::R;
    case 'S':
        return Input::eKey::S;
    case 'T':
        return Input::eKey::T;
    case 'U':
        return Input::eKey::U;
    case 'V':
        return Input::eKey::V;
    case 'W':
        return Input::eKey::W;
    case 'X':
        return Input::eKey::X;
    case 'Y':
        return Input::eKey::Y;
    case 'Z':
        return Input::eKey::Z;
    case VK_SHIFT:
        return Input::eKey::SHIFT;
    case VK_CONTROL:
        return Input::eKey::CTRL;
    case VK_MENU:
        return Input::eKey::ALT;
    case VK_TAB:
        return Input::eKey::TAB;
    case VK_ESCAPE:
        return Input::eKey::ESC;
    case VK_SPACE:
        return Input::eKey::SPACE;
    case VK_RETURN:
        return Input::eKey::ENTER;
    default:
        return Input::eKey::UNKOWN;
    }
}

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
        Input::GetMutableInput().currentKeys[(int)TranslateWinKey(wParam)] = true;
        return 0;
    }
    case WM_KEYUP:
    {
        Input::GetMutableInput().currentKeys[(int)TranslateWinKey(wParam)] = false;
        return 0;
    }
    case WM_LBUTTONDOWN:
    {
        Input::GetMutableInput().currentKeys[(int)Input::eKey::MOUSE_L] = true;
        return 0;
    }
    case WM_LBUTTONUP:
    {
        Input::GetMutableInput().currentKeys[(int)Input::eKey::MOUSE_L] = false;
        return 0;
    }
    case WM_RBUTTONDOWN:
    {
        Input::GetMutableInput().currentKeys[(int)Input::eKey::MOUSE_R] = true;
        return 0;
    }
    case WM_RBUTTONUP:
    {
        Input::GetMutableInput().currentKeys[(int)Input::eKey::MOUSE_R] = false;
        return 0;
    }
    case WM_MBUTTONDOWN:
    {
        Input::GetMutableInput().currentKeys[(int)Input::eKey::MOUSE_M] = true;
        return 0;
    }
    case WM_MBUTTONUP:
    {
        Input::GetMutableInput().currentKeys[(int)Input::eKey::MOUSE_M] = false;
        return 0;
    }
    case WM_MOUSEMOVE:
    {
        Input::InputState& input = Input::GetMutableInput();

        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);

        input.deltaX = (float32)x - input.x;
        input.deltaY = (float32)y - input.y;
        input.x = x;
        input.y = y;

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
#if defined(DEBUG)
    FreeConsole();
#endif
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
#if defined(DEBUG)
    CreateDebugConsole();
#endif
    return NoodleMain();
}
#endif