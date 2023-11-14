// dwmext.cpp : Definiert den Einstiegspunkt für die Anwendung.
//

#include "framework.h"
#include "dwmext.h"
#include <cstdio>

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    AllocConsole();
    freopen("CONOUT$", "w", stdout);


    
    while (true)
    {
        printf("%d\n", GetKeyState(VK_LMENU) & 0x8000);
        if (GetKeyState(VK_END) & 0x8000)
        {
            MessageBox(nullptr, L"ok", nullptr, 0);
            return 0;
        }
        Sleep(100);
    }
    return 0;
}
