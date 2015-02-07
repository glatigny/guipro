/*  HulK - GUIPro Project ( http://guipro.sourceforge.net/ )

    Author : DarkSage  aka  Glatigny Jérôme <darksage@darksage.fr>

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
#include "iconize.h"
#include "hotKey.h"
#include "resource.h"
#include "menu.h"
#include "config.h"
#include "mouse.h"
#include "windowHK.h"
#include "plugin.h"
#include <crtdbg.h>

/* ------------------------------------------------------------------------------------------------- */

const WCHAR g_szMainWnd[]		= L"_HK90_Hulk_MainWnd_";
const WCHAR g_szWindowName[]	= L"Hulk";

HINSTANCE g_hInst	= NULL;
HWND g_hwndMain		= NULL;
HICON g_hIconTray	= NULL;
DWORD windows_version = 0;

HWND g_hwndShell	= NULL;
HWND g_deskWindow	= NULL;
HWND g_shell_tray	= NULL;

/* ------------------------------------------------------------------------------------------------- */

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
#ifndef NDEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
flag |= _CRTDBG_LEAK_CHECK_DF;
_CrtSetDbgFlag(flag);
#endif 

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
#ifdef HULK_USE_SHELLHOOK
	static UINT s_uShellHook = 0;
#endif

	switch(uMsg)
	{
	case WM_CREATE:
		s_uTaskBarCreated = RegisterWindowMessage(L"TaskbarCreated");
#ifdef HULK_USE_SHELLHOOK
		s_uShellHook = RegisterWindowMessage(L"SHELLHOOK");
#endif
		g_hwndMain = hwnd;

		g_hwndShell = GetShellWindow();
		g_deskWindow = GetDesktopWindow();
		g_shell_tray = FindWindow(L"Shell_TrayWnd",L"");

		// Set icons
		SendMessage(hwnd, WM_SETICON, ICON_BIG, 
			(LONG)(LONG_PTR)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_MAIN_ICON), IMAGE_ICON, 
			GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR));

		SendMessage(hwnd, WM_SETICON, ICON_SMALL, 
			(LONG)(LONG_PTR)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_TRAY_ICON), IMAGE_ICON, 
			GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR));

		windows_version = GetDllVersion(TEXT("Shell32.dll"));
		
		// Configuration Loading
		//  bycursor and borderless options
		// (see below this file for more informations about theses options)
		openConfig();

		// Set the Tray Icon
		ReloadExTrayIcon();

		InitMenuVars();

		if( !registerConfig(TRUE) )
		{
			DeleteTrayIcon(g_hwndMain, IDI_MAIN_ICON);
			PostQuitMessage(0);
			return 0;
		}
		
		// Set up the drag window feature
		installHookMouse();

#ifdef HULK_USE_SHELLHOOK
		// Register for shell hook
		RegisterShellHookWindow(g_hwndMain);
#endif

		// Clean the memory in order to have a light application
		FlushMemory();
		break;

	case WM_HOTKEY:
		{
			HWND l_hwnd = NULL;

			if( (wParam >= IDH_HOTKEY_PLUGIN_BASE) && (wParam <= IDH_HOTKEY_PLUGIN_BASE + g_pluginId) )
			{
				pluginHotkey( (UINT) wParam);
			}
			else
			{
				switch( wParam )
				{
					case IDH_HOTKEY_BASE + HK_ACT_MINIMISE_POINTED:
							l_hwnd = HulkGetPointedWindow();
							minimizeWindow(l_hwnd);
						break;

					case IDH_HOTKEY_BASE + HK_ACT_MINIMISE_CURRENT:
							l_hwnd = HulkGetActiveWindow();
							minimizeWindow(l_hwnd);
						break;

					case IDH_HOTKEY_BASE + HK_ACT_CLOSE_POINTED:
							l_hwnd = HulkGetPointedWindow();
							closeWindow(l_hwnd);
						break;

					case IDH_HOTKEY_BASE + HK_ACT_CLOSE_CURRENT:
							l_hwnd = HulkGetActiveWindow();
							closeWindow(l_hwnd);
						break;

					case IDH_HOTKEY_BASE + HK_ACT_MAXIMISE_POINTED:
							l_hwnd = HulkGetPointedWindow();
							maximizeWindow(l_hwnd);
						break;

					case IDH_HOTKEY_BASE + HK_ACT_MAXIMISE_CURRENT:
							l_hwnd = HulkGetActiveWindow();
							maximizeWindow(l_hwnd);
						break;

					case IDH_HOTKEY_BASE + HK_ACT_ICONIZE_POINTED:
							l_hwnd = HulkGetPointedWindow();
							iconizeWindow(l_hwnd);
						break;

					case IDH_HOTKEY_BASE + HK_ACT_ICONIZE_CURRENT:
							l_hwnd = HulkGetActiveWindow();
							iconizeWindow(l_hwnd);
						break;

					case IDH_HOTKEY_BASE + HK_ACT_TRAYNIZE_POINTED:
							l_hwnd = HulkGetPointedWindow();
							traynizeWindow(l_hwnd);
						break;

					case IDH_HOTKEY_BASE + HK_ACT_TRAYNIZE_CURRENT:
							l_hwnd = HulkGetActiveWindow();
							traynizeWindow(l_hwnd);
						break;

					case IDH_HOTKEY_BASE + HK_ACT_UNICONIZE:
							UnIconizeLast();
						break;

					case IDH_HOTKEY_BASE + HK_ACT_ALWTOP_POINTED:
							l_hwnd = HulkGetPointedWindow();
							alwaysOnTop(l_hwnd);
						break;

					case IDH_HOTKEY_BASE + HK_ACT_ALWTOP_CURRENT:
							l_hwnd = HulkGetActiveWindow();
							alwaysOnTop(l_hwnd);
						break;

					case IDH_HOTKEY_BASE + HK_ACT_QUIT:
							QuitHulK();
						break;
				}
			}
		}
		FlushMemory();
		break;

	case WM_DESTROY:
		QuitHulK();
		break;

	case WM_MYTRAYMSG:
		// Tray Icon event
		if (wParam == IDI_MAIN_ICON)
		{
			if( ((UINT)lParam == WM_RBUTTONUP) || ((UINT)lParam == WM_LBUTTONUP) ) 
			{
				ShowTrayMenu();
				FlushMemory();
			}
		}
		else
		{
			if( (UINT)lParam == WM_LBUTTONUP )
			{
				UnIconize( (UINT)wParam );
			}
			else if( (UINT)lParam == WM_RBUTTONUP )
			{
				ShowIconizeMenu( (UINT)wParam );
			}
		}

		FlushMemory();
		break;

	case WM_PLUGIN_EVENT:
		switch( lParam )
		{
			case PORTAL_MINIMIZE:
				minimizeWindow( (HWND)wParam );
				break;
			case PORTAL_MAXIMISE:
				maximizeWindow( (HWND)wParam );
				break;
			case PORTAL_CLOSE:
				closeWindow( (HWND)wParam );
				break;
			case PORTAL_ICONIZE:
				iconizeWindow( (HWND)wParam );
				break;
			case PORTAL_TRAYNIZE:
				traynizeWindow( (HWND)wParam );
				break;
			case PORTAL_UNICONIZE:
				UnIconizeLast();
				break;
			case PORTAL_ONTOP:
				alwaysOnTop( (HWND)wParam );
				break;
			case PORTAL_BALLOON:
				pluginBalloon(pluginGetModule( (UINT)wParam ), PORTAL_PLUGIN_RETURN_TXT);
				break;
		}
		FlushMemory();
		break;

	case WM_MEASUREITEM:
		OnMeasureItem(hwnd, wParam, lParam);
		break;

	case WM_DRAWITEM:
		OnDrawItem(hwnd, wParam, lParam);
		break;

	case WM_TIMER:
		if( wParam == IDT_TIMERSYSTRAY )
		{
			CheckAllIconize();
		}
		break;

	default:
		if( uMsg == s_uTaskBarCreated )
		{
			ReloadExTrayIcon();
			FlushMemory();
		}
#ifdef HULK_USE_SHELLHOOK
		else if( uMsg == s_uShellHook )
		{
			// http://msdn.microsoft.com/en-us/library/ms644989(v=VS.85).aspx
			if( (HWND)lParam != hwnd && lParam != NULL )
			{
				switch(wParam)
				{
					case HSHELL_WINDOWCREATED:
						// We may send this information to some plugins.
						break;
					case HSHELL_WINDOWDESTROYED:
						// And we may send this information to some plugins too.
						break;
				}
			}
		}
#endif
		else
		{
			return(DefWindowProc(hwnd, uMsg, wParam, lParam));
		}
		break;
	}
	return 0;
}

/* ------------------------------------------------------------------------------------------------- */

void QuitHulK()
{
	DeleteTrayIcon(g_hwndMain, IDI_MAIN_ICON);
	UnIconizeAll();
	uninstallHookMouse();
	uninstallPlugins();
	clearConfig();
#ifdef HULK_USE_SHELLHOOK
	DeregisterShellHookWindow(g_hwndMain);
#endif
	PostQuitMessage(0);
}

/* ------------------------------------------------------------------------------------------------- */

/** Flush Memory
 * Just a little function to call in order to reduce application memory usage.
 * The operating system (microsoft windows) will remove non use memory allocated for the application. 
 */
void FlushMemory()
{
	SetProcessWorkingSetSize(GetCurrentProcess(), 102400, 614400);
}

/* ------------------------------------------------------------------------------------------------- */

BOOL isBanishedHandle(HWND hwnd)
{
	WCHAR buffer[512];
	int ret = GetClassName(hwnd, buffer, 512);
	if( ret > 0 )
	{
		buffer[ret] = '\0';
	}

	// TaskSwitcherWnd / TaskSwitcherOverlayWnd / Shell_TrayWnd / WorkerW

	if( !wcscmp(buffer, L"TaskSwitcherWnd") )
		return true;
	if( !wcscmp(buffer, L"TaskSwitcherOverlayWnd") )
		return true;
	if( !wcscmp(buffer, L"Shell_TrayWnd") )
		return true;
	if( !wcscmp(buffer, L"WorkerW") )
		return true;
	if( !wcscmp(buffer, L"IPTip_Main_Window") )
		return true;

	// Windows 8 specific: ImmersiveLauncher / Windows.UI.Core.CoreWindow

	if( !wcscmp(buffer, L"ImmersiveLauncher") )
		return true;
	if( !wcscmp(buffer, L"ImmersiveSplashScreenWindowClass") )
		return true;
	if( !wcscmp(buffer, L"Windows.UI.Core.CoreWindow") )
		return true;

	return false;
}

/* ------------------------------------------------------------------------------------------------- */

/** Hulk Get Window
 * This function returns the handle of the window that hulk will "modify".
 * It get the current window (with focus) or the window pointed by the mouse cursor.
 * It depends of the user options.
 */

HWND HulkGetPointedWindow()
{
	POINT l_pos_cursor;
	HWND hWndRoot;

	if( g_hwndShell == NULL )	g_hwndShell = GetShellWindow();
	if( g_deskWindow == NULL )	g_deskWindow = GetDesktopWindow();
	if( g_shell_tray == NULL )	g_shell_tray = FindWindow(L"Shell_TrayWnd",L"");

	if (GetCursorPos(&l_pos_cursor) )
	{
		HWND l_pointed_hwnd = WindowFromPoint( l_pos_cursor );

		hWndRoot = GetAncestor( l_pointed_hwnd, GA_ROOT);
		if( (hWndRoot != g_hwndShell ) && (hWndRoot != g_deskWindow) && (hWndRoot != g_shell_tray) && !isBanishedHandle(hWndRoot) )
		{
			return hWndRoot;
		}
	}

	hWndRoot = HulkGetActiveWindow();
	if( hWndRoot > 0 && (hWndRoot != g_hwndShell ) && (hWndRoot != g_deskWindow) && (hWndRoot != g_shell_tray) && !isBanishedHandle(hWndRoot) )
	{
		return hWndRoot;
	}
	return NULL;
}

/* ------------------------------------------------------------------------------------------------- */

HWND HulkGetActiveWindow()
{
	HWND hWndRoot = GetForegroundWindow();
	
	if( g_hwndShell == NULL )	g_hwndShell = GetShellWindow();
	if( g_deskWindow == NULL )	g_deskWindow = GetDesktopWindow();
	if( g_shell_tray == NULL )	g_shell_tray = FindWindow(L"Shell_TrayWnd",L"");

	if( (hWndRoot != g_hwndShell ) && (hWndRoot != g_deskWindow) && (hWndRoot != g_shell_tray) && !isBanishedHandle(hWndRoot) )
	{
		return hWndRoot;
	}
	return NULL;
}

/* ------------------------------------------------------------------------------------------------- */
