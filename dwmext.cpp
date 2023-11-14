// dwmext.cpp : Definiert den Einstiegspunkt für die Anwendung.
//

#include "framework.h"
#include "dwmext.h"
#include <cstdio>
#include <iostream>

HWND relative_move_hwnd = nullptr;
POINT relative_move_diff = {0};

int move_key = VK_LMENU;
HHOOK mouse_hook;

HWND get_top_level_parent(HWND hwnd)
{
    if (GetWindowLongPtr(hwnd, GWL_STYLE) & WS_CHILD)
    {
        return GetAncestor(hwnd, GA_ROOT);
    }
    return hwnd;
}

LRESULT CALLBACK mouse_hook_proc(int n_code, WPARAM w_param, LPARAM l_param) {
    if (n_code >= 0) {
        auto mouse_info = reinterpret_cast<MSLLHOOKSTRUCT*>(l_param);

        if (w_param == WM_LBUTTONDOWN) {
            
            if (GetKeyState(move_key) & 0x8000)
            {
                relative_move_hwnd = get_top_level_parent(WindowFromPoint(mouse_info->pt));
                RECT rect = {0};
                GetWindowRect(relative_move_hwnd, &rect);

                relative_move_diff = {
                    mouse_info->pt.x - rect.left,
                    mouse_info->pt.y - rect.top,
                };
            }
        }
        if (w_param == WM_LBUTTONUP)
        {
            relative_move_hwnd = nullptr;
        }
        if (w_param == WM_MOUSEMOVE)
        {
            if (relative_move_hwnd)
            {
                SetWindowPos(relative_move_hwnd, nullptr, mouse_info->pt.x - relative_move_diff.x, mouse_info->pt.y - relative_move_diff.y, 0, 0, SWP_NOSIZE);

            }
        }
    }

    return CallNextHookEx(mouse_hook, n_code, w_param, l_param);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    AllocConsole();
    freopen("CONOUT$", "w", stdout);

    mouse_hook = SetWindowsHookEx(WH_MOUSE_LL, mouse_hook_proc, NULL, 0);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnhookWindowsHookEx(mouse_hook);
    return 0;

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
