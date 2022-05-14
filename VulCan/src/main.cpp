/*
	VulCan - GUIPro Project ( http://glatigny.github.io/guipro/ )

	Author : Glatigny Jérôme <jerome@obsi.dev>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "common.h"
#include "main.h"
#include "resource.h"
#include "hotKey.h"
#include "volume.h"
#include "menu.h"
#include "config.h"
#include "mouse.h"
#include <crtdbg.h>

/* ------------------------------------------------------------------------------------------------- */

const WCHAR g_szMainWnd[]		= L"_VL90_VulCan_MainWnd_";
const WCHAR g_szWindowName[]	= L"VulCan";

HINSTANCE g_hInst	= NULL;
HWND g_hwndMain		= NULL;
HICON g_hIconTray	= NULL;
DWORD windows_version = 0;
BOOL g_aboutballoon = 0;

/* ------------------------------------------------------------------------------------------------- */

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
#ifndef NDEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
flag |= _CRTDBG_LEAK_CHECK_DF;
_CrtSetDbgFlag(flag);
#endif 
	// Get the Version of Windows
	windows_version = GetWindowsVersion();

	if( windows_version < WINVER_VISTA )
	{
		MessageBox(NULL, TEXT("his program requires Windows Vista or newer."), TEXT("VulCan"), NULL);
		return 1;
	}

	// Keep the instance handle of the application
	g_hInst = hInstance;

	// Creation and registration of the application class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= 0;
	wcex.lpfnWndProc	= (WNDPROC) MainWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= g_hInst;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= g_szMainWnd;
	wcex.hIconSm		= NULL;

	if( !RegisterClassEx(&wcex) )
		return 1;

	if( !CreateWindowEx(WS_EX_TOOLWINDOW, g_szMainWnd, g_szWindowName, 0, 0, 0, 0, 0, NULL, NULL, g_hInst, NULL) )
		return 1;

	// Main loop.
	// Get and send messages for the application.
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

/* ------------------------------------------------------------------------------------------------- */

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static UINT s_uTaskBarCreated = 0;

	switch(uMsg)
	{
	case WM_CREATE:
		s_uTaskBarCreated = RegisterWindowMessage(L"TaskbarCreated");
		g_hwndMain = hwnd;

		// Set icons
		SendMessage(hwnd, WM_SETICON, ICON_BIG, 
			(LONG)(LONG_PTR)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_MAIN_ICON), IMAGE_ICON, 
			GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR));

		SendMessage(hwnd, WM_SETICON, ICON_SMALL, 
			(LONG)(LONG_PTR)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_TRAY_ICON), IMAGE_ICON, 
			GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR));

		// call uninstall hook keyboard to clean the override key array
		uninstallHookKeyboard();

		// Set the Tray Icon
		ReloadExTrayIcon();

		if (openConfig() == 1)
		{
			MessageBox(g_hwndMain, L"No config file", L"Error", NULL);
			GUIProQuit();
			return 0;
		}

		if (!registerConfig(TRUE))
		{
			GUIProQuit();
			return 0;
		}

		/*
		myRegisterHotKey(VK_ADD, MOD_CONTROL | MOD_WIN, IDH_HOTKEY_VOL_UP, true);
		myRegisterHotKey(VK_SUBTRACT, MOD_CONTROL | MOD_WIN, IDH_HOTKEY_VOL_DOWN, true);
		myRegisterHotKey(VK_NUMPAD0, MOD_CONTROL | MOD_WIN, IDH_HOTKEY_VOL_MUTE, true);
		*/
		// Clean the memory in order to have a light application
		FlushMemory();
		break;

	case WM_DESTROY:
		GUIProQuit();
		break;

	case WM_HOTKEY:
		{
			int ret = -2;
			switch (wParam) {
				case IDH_HOTKEY_VOL_UP:
					ret = changeVolumeUp();
					break;
				case IDH_HOTKEY_VOL_DOWN:
					ret = changeVolumeDown();
					break;
				case IDH_HOTKEY_VOL_MUTE:
					ret = changeVolumeMute();
					if (ret > 0)
						ret = getVolume();
					break;
			}

			if (ret >= 0)
			{
				ShowVolumeBalloon(ret);
			}

			unloadVolumeInterface();
			FlushMemory();
		}
		break;

	case WM_MYTRAYMSG:
		// Tray Icon event
		if (wParam == IDI_MAIN_ICON)
		{
			if ((UINT)lParam == WM_RBUTTONUP) 
			{
				ShowTrayMenu();
				FlushMemory();
			}
			else if ((UINT)lParam == WM_LBUTTONUP)
			{
				ShowVolumeBalloon(-1);
				FlushMemory();
			}
		}
		break;

	default:
		if( uMsg == s_uTaskBarCreated )
		{
			ReloadExTrayIcon();
			FlushMemory();
		}
		else
		{
			return(DefWindowProc(hwnd, uMsg, wParam, lParam));
		}
		break;
	}
	return 0;
}

/* ------------------------------------------------------------------------------------------------- */

void GUIProQuit()
{
	myUnregisterHotKey(IDH_HOTKEY_VOL_UP);
	myUnregisterHotKey(IDH_HOTKEY_VOL_DOWN);
	uninstallHookMouse();

	unloadVolumeInterface();

	DeleteTrayIcon(g_hwndMain, IDI_MAIN_ICON);
	PostQuitMessage(0);
}

/* ------------------------------------------------------------------------------------------------- */

/** Flush Memory
 * Just a little function to call in order to reduce application memory usage.
 * The operating system (microsoft windows) will remove non use memory allocated for the application. 
 */
void FlushMemory()
{
	SetProcessWorkingSetSize(GetCurrentProcess(), 102400, 409600);
}

/* ------------------------------------------------------------------------------------------------- */
