// dwmext.cpp : Definiert den Einstiegspunkt für die Anwendung.
//

#include "framework.h"
#include "dwmext.h"
#include <cstdio>
#include <iostream>

HWND relative_move_hwnd = nullptr;
POINT relative_move_diff = { 0 };

HWND relative_size_hwnd = nullptr;
RECT relative_size_rect = { 0 };
POINT relative_size_mouse_position = { 0 };

int exit_key = VK_F11;
int relative_move_key = VK_LMENU;
int relative_size_key = VK_CONTROL;

HHOOK mouse_hook;
HHOOK keyboard_hook;

#define WIDTH(rect) (rect.right - rect.left)
#define HEIGHT(rect) (rect.bottom - rect.top)

HWND get_top_level_parent(HWND hwnd)
{
	if (GetWindowLongPtr(hwnd, GWL_STYLE) & WS_CHILD)
	{
		return GetAncestor(hwnd, GA_ROOT);
	}
	return hwnd;
}

void unhook() {
	UnhookWindowsHookEx(mouse_hook);
	UnhookWindowsHookEx(keyboard_hook);
}

LRESULT CALLBACK mouse_hook_proc(int n_code, WPARAM w_param, LPARAM l_param) {

	if (n_code < 0)
	{
		return CallNextHookEx(NULL, n_code, w_param, l_param);
	}
	bool handled = false;
	auto mouse_info = reinterpret_cast<MSLLHOOKSTRUCT*>(l_param);

	if (w_param == WM_LBUTTONDOWN) {

		if (GetKeyState(relative_move_key) & 0x8000)
		{
			relative_move_hwnd = get_top_level_parent(WindowFromPoint(mouse_info->pt));
			RECT rect = { 0 };
			GetWindowRect(relative_move_hwnd, &rect);

			relative_move_diff = {
				mouse_info->pt.x - rect.left,
				mouse_info->pt.y - rect.top,
			};
			handled = true;
		}

		if (GetKeyState(relative_size_key) & 0x8000)
		{
			relative_size_hwnd = get_top_level_parent(WindowFromPoint(mouse_info->pt));
			GetWindowRect(relative_size_hwnd, &relative_size_rect);
			relative_size_mouse_position = mouse_info->pt;
			handled = true;
		}
	}
	if (w_param == WM_LBUTTONUP)
	{
		if (relative_move_hwnd || relative_size_hwnd)
		{
			handled = true;
		}

		relative_move_hwnd = nullptr;
		relative_size_hwnd = nullptr;

		
	}
	if (w_param == WM_MOUSEMOVE)
	{
		if (relative_move_hwnd)
		{
			SetWindowPos(relative_move_hwnd, nullptr, mouse_info->pt.x - relative_move_diff.x, mouse_info->pt.y - relative_move_diff.y, 0, 0, SWP_NOSIZE);
		}

		if (relative_size_hwnd)
		{
			RECT current_rect = { 0 };
			GetWindowRect(relative_size_hwnd, &current_rect);

			POINT diff = {
				mouse_info->pt.x - relative_size_mouse_position.x,
				mouse_info->pt.y - relative_size_mouse_position.y,
			};


			POINT relative = {
				relative_size_mouse_position.x - relative_size_rect.left,
				relative_size_mouse_position.y - relative_size_rect.top,
			};

			// top-left corner
			if (relative.x < WIDTH(current_rect) / 2 && relative.y < HEIGHT(current_rect) / 2)
			{
				current_rect.left = relative_size_rect.left + diff.x;
				current_rect.top = relative_size_rect.top + diff.y;
			}

			// top-right corner
			if (relative.x > WIDTH(current_rect) / 2 && relative.y < HEIGHT(current_rect) / 2)
			{
				current_rect.top = relative_size_rect.top + diff.y;
				current_rect.right = relative_size_rect.right + diff.x;
			}

			// bottom-left corner
			if (relative.x < WIDTH(current_rect) / 2 && relative.y > HEIGHT(current_rect) / 2)
			{
				current_rect.left = relative_size_rect.left + diff.x;
				current_rect.bottom = relative_size_rect.bottom + diff.y;
			}

			// bottom-right corner
			if (relative.x > WIDTH(current_rect) / 2 && relative.y > HEIGHT(current_rect) / 2)
			{
				current_rect.right = relative_size_rect.right + diff.x;
				current_rect.bottom = relative_size_rect.bottom + diff.y;
			}

			MoveWindow(relative_size_hwnd, current_rect.left, current_rect.top, WIDTH(current_rect), HEIGHT(current_rect), true);
		}
	}


	return handled ? 1 : CallNextHookEx(NULL, n_code, w_param, l_param);
}

LRESULT CALLBACK keyboard_hook_proc(int n_code, WPARAM w_param, LPARAM l_param)
{
	if (n_code >= 0)
	{
		auto keyboard_info = reinterpret_cast<KBDLLHOOKSTRUCT*>(l_param);

		if (w_param == WM_KEYDOWN && keyboard_info->vkCode == exit_key)
		{
			MessageBox(nullptr, L"Dwmext exiting.", L"dwmext", MB_ICONINFORMATION);
			unhook();
			PostQuitMessage(0);
		}
	}

	return CallNextHookEx(NULL, n_code, w_param, l_param);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
#if _DEBUG
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	MessageBox(nullptr, L"Dwmext is running.\n CTRL + F11 to exit.", L"dwmext", MB_ICONINFORMATION);
#endif

	printf("running...\n");

	mouse_hook = SetWindowsHookEx(WH_MOUSE_LL, mouse_hook_proc, NULL, 0);
	keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboard_hook_proc, NULL, 0);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

exit:
	printf("unhooking...\n");
	unhook();
	return 0;
}
