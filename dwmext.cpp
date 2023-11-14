// dwmext.cpp : Definiert den Einstiegspunkt für die Anwendung.
//

#include "framework.h"
#include "dwmext.h"
#include <cstdio>

int move_key = VK_LMENU;


HWND get_top_level_parent(HWND hwnd)
{
    if (GetWindowLongPtr(hwnd, GWL_STYLE) & WS_CHILD)
    {
        return GetAncestor(hwnd, GA_ROOT);
    }
    return hwnd;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    

    bool last_moving = false;
    HWND moving_hwnd = nullptr;
    POINT moving_difference = {0};
    RECT moving_window_rect = {0};

    
    while (true)
    {
        POINT mouse_position = {0};
        GetCursorPos(&mouse_position);
        
        bool moving = GetKeyState(move_key) & 0x8000;
        
        if (moving && !last_moving)
        {
            moving_hwnd = get_top_level_parent(WindowFromPoint(mouse_position));
            GetWindowRect(moving_hwnd, &moving_window_rect);

            moving_difference = {
                mouse_position.x - moving_window_rect.left,
                mouse_position.y - moving_window_rect.top,
            };
        }

        if (moving)
        {
            SetWindowPos(moving_hwnd, nullptr, mouse_position.x - moving_difference.x, mouse_position.y - moving_difference.y, 0, 0, SWP_NOSIZE);
        }

        if (GetKeyState(VK_SPACE) & 0x8000)
        {
            MessageBox(nullptr, L"dwmext exiting", nullptr, 0);
            return 0;
        }

        last_moving = moving;
        Sleep(1);
    }
    return 0;
}
