#include "NoodlePch.h"
#include "NoodleEntry.h"

#if defined(WINDOWS)
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    return NoodleMain();
}
#endif