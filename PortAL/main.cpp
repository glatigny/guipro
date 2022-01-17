/*
	PortAL - GUIPro Project ( http://glatigny.github.io/guipro/ )

	Author : Glatigny Jérôme <jerome@darksage.fr>

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
#include "hotKey.h"
#include "resource.h"
#include "menu.h"
#include "launch.h"
#include "updater.h"
#include "iconManager.h"
#include "config.h"

/* ------------------------------------------------------------------------------------------------- */

#define REGISTER_MESSAGE		L"TaskbarCreated"

/* ------------------------------------------------------------------------------------------------- */

const wchar_t g_szMainWnd[]		= L"_Portal90_PortAL_MainWnd_";
const wchar_t g_szWindowName[]	= L"PortAL";

HINSTANCE g_hInst	= NULL;
HWND g_hwndMain		= NULL;
std::vector<HICON> g_IconTray;
LPWSTR g_loadingmessage = NULL;
UINT g_currentMenu	= 0;
DWORD windowsVersion = 0;
BOOL g_aboutbaloon = 0;

/* ------------------------------------------------------------------------------------------------- */

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
// Special debugging code in order to have
#ifndef NDEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
flag |= _CRTDBG_LEAK_CHECK_DF;
_CrtSetDbgFlag(flag);
#endif
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	// Keep the Instance of the application
	g_hInst = hInstance;

	// Creation of a special window class which will be ower main invisible window
	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX);
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

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	CoUninitialize();

	return 0;
}

/* ------------------------------------------------------------------------------------------------- */

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static UINT s_uTaskBarCreated = 0;

	switch(uMsg)
	{
	case WM_CREATE:
		{
			g_hwndMain = hwnd;
			// Register task bar (re)creation in order to reload systray icons after explorer crashes
			s_uTaskBarCreated = RegisterWindowMessage(REGISTER_MESSAGE);

			// Set Icons
			SendMessage(hwnd, WM_SETICON, ICON_BIG, 
				(LONG)(LONG_PTR)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_MAIN_ICON), IMAGE_ICON, 
				GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR));

			SendMessage(hwnd, WM_SETICON, ICON_SMALL, 
				(LONG)(LONG_PTR)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_MAIN_ICON), IMAGE_ICON, 
				GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR));

			ShowWindow(hwnd, SW_HIDE);

			// Clear the list
			g_IconTray.clear();

			installHookKeyboard();

			// Open the configuration
			if (openConfig() == FALSE)
			{
				uninstallHookKeyboard();
				clearMenuSkin();

				MessageBox(g_hwndMain, L"Configuration file not found or invalid", ERR_MSGBOX_TITLE, NULL);

				PostQuitMessage(0);
				return 0;
			}

			// Retreive the version of Windows (in order to know if it is a win200 or superior)
			windowsVersion = GetWindowsVersion();

			// Initialisation of variables for menus
			InitMenuVars();

			// Add the system tray icons (depends of the configuration)
			ShowTrayIcons();

			// Show loading error messages
			if (g_loadingmessage != NULL) {
				ShowBalloon(ERR_MSGBOX_TITLE, g_loadingmessage, 0, NIIF_ERROR);
				free(g_loadingmessage);
				g_loadingmessage = NULL;
			}

			// Launch application in "autorun" mode
			fireEvent(PROG_EVENT_START);

			FlushMemory();
		}
		break;

	case WM_HOTKEY:
		if( (wParam >= IDH_HOTKEY_MENU) && (wParam <= (IDH_HOTKEY_MENU + g_portal->hotkeys.size()) ) )
		{
			// Detect application item or menu item in order to open a menu or launch the application
			PortalProg* l_sp = g_portal->hotkeys[ wParam - IDH_HOTKEY_MENU ];

			while( l_sp )
			{
				if( l_sp->progs.size() > 0 )
				{
					ShowTrayMenu(l_sp, PORTAL_MENU_HOTKEY);
				}
				else
				{
					l_sp->run(false);
					// Some commands could call quitPortal. In this case g_portal is set to NULL.
					if(g_portal == NULL)
						return 0;
				}
				l_sp = l_sp->nextSameHotkey;
			}
		}
		FlushMemory();
		break;

	case WM_DESTROY:
		{
			quitPortal();
		}
		break;

	case WM_MYTRAYMSG:
		if ((wParam >= IDI_MAIN_ICON) && g_portal && (wParam <= (IDI_MAIN_ICON + g_portal->menus.size())))
		{
			// Find the clicked systray
			int pos = (int)wParam - IDI_MAIN_ICON;
			if( ((UINT)lParam == WM_RBUTTONUP) || ((UINT)lParam == WM_LBUTTONUP) || ((UINT)lParam == WM_MOUSEMOVE) )
			{
				// Left click : minimalist menu
				// Right click : Complete menu (with About/Reload/Quit)
				// Auto Open : Complete menu (like right click) with a automatic dismiss
				int param = PORTAL_MENU_SYSTRAY;
				if((UINT)lParam == WM_RBUTTONUP)
				{
					param = PORTAL_MENU_SYSTRAY_CMD;
				}
				
				if( ((UINT)lParam == WM_MOUSEMOVE) )
				{
					if( (g_portal->menus[pos] != NULL) && (g_portal->menus[pos]->options & PROG_OPTION_AUTOOPEN) )
					{
						param = PORTAL_MENU_SYSTRAY_AUTO;
					}
					else
					{
						return 0;
					}
				}

				// Detect if the icon is for a menu or a application
				if( (g_portal->menus[pos] != NULL) && ( g_portal->menus[pos]->progs.size() > 0 ) )
				{
					ShowTrayMenu(g_portal->menus[pos], param);
					g_currentMenu = pos;
				}
				else if( (g_portal->menus[pos] != NULL) && (g_portal->menus[pos]->isProgExe()))
				{
					if((UINT)lParam == WM_LBUTTONUP)
					{
						g_portal->menus[pos]->run(true);
						g_currentMenu = pos;
					}
					else
					{
						ShowTrayMenu(g_portal->menus[pos], param);
						g_currentMenu = pos;
					}
				}
				else
				{
					ShowTrayMenu(NULL, param);
					g_currentMenu = 0;
				}
				FlushMemory();
			}
			else if( (UINT)lParam == NIN_BALLOONUSERCLICK )
			{
				if(g_aboutbaloon)
					checkGuiproVersion();
				FlushMemory();
				return 0;
			}
		}
		break;

	case WM_MYREDRAWMENU:
		DrawMenuBar(g_hwndMain);
		break;

	case WM_MYREDRAWITEM:
		RefreshTrayMenuItem((PortalMenuItem*)lParam);
		break;

	case WM_INITMENUPOPUP :
		OnInitMenuPopup(hwnd, wParam, lParam);
		break;

	case WM_UNINITMENUPOPUP:
		OnUninitMenuPopup(hwnd, wParam, lParam);
		break;

	case WM_MEASUREITEM :
		OnMeasureItem(hwnd, wParam, lParam);
		break;

	case WM_DRAWITEM :
		OnDrawItem(hwnd, wParam, lParam);
		break;

	case WM_MENUCHAR:
		return OnMenuCharItem(hwnd, wParam, lParam);

	case WM_COMMAND:
		// Retreive a menu command
		if( wParam == IDM_EXIT )
		{
			quitPortal();
		}
		else if( wParam == IDM_RELOAD )
		{
			reloadPortalConfig();
		}
		else if( wParam == IDM_ABOUT )
		{
			ShowAbout(g_currentMenu);
			FlushMemory();
		}
		else if( ((UINT)wParam >= (PORTAL_HK_ID)) && ( (UINT)wParam <= (UINT)(PORTAL_HK_ID + menuGetNbElem()) )  )
		{
			PortalProg* l_sp = menuGetElem( (int)(wParam - 1 - PORTAL_HK_ID) );
			l_sp->run(true);
			FlushMemory();
		}
		else if( ((UINT)wParam >= (PORTAL_FILE_ID)) && ( (UINT)wParam < (UINT)(PORTAL_FILE_ID + menuGetNbFiles()) ) )
		{
			PortalProg* l_sp = menuGetFile( (int)(wParam - PORTAL_FILE_ID) );
			l_sp->run(true);
			FlushMemory();

			for(PortalProgVector::iterator i = g_portal_files.begin(); i != g_portal_files.end(); i++)
			{
				delete (*i);
			}
			g_portal_files.clear();
		}
		break;

	default:
		if( uMsg == s_uTaskBarCreated )
		{
			// Traskbar (re)creation, we have to reset systray icons
			ReloadTrayIcons();
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

void quitPortal()
{
	fireEvent(PROG_EVENT_QUIT);

	DestroyMyMenu(true);
	uninstallFileNotification();
	unregisterHotkeys();
	uninstallHookKeyboard();
	clearMenuSkin();
#ifdef ICON_MANAGER
	unloaderIconManager();
#endif

	delete( g_portal );
	g_portal = NULL;

	for(PortalProgVector::iterator i = g_portal_files.begin(); i != g_portal_files.end(); i++) { delete (*i); }
	g_portal_files.clear();

	UnInitMenuVars();
	DeleteTrayIcons();
	PostQuitMessage(0);
}

/* ------------------------------------------------------------------------------------------------- */

void reloadPortalConfig()
{
	uninstallFileNotification();

	int ret = openConfig();
	if( ret == TRUE )
	{
		ReloadTrayIcons();
		ShowBalloon(WC_PORTAL_ABOUT_TEXT_TITLE, WC_PORTAL_RLDCONF_TEXT_OK, 0, NIIF_INFO);
	}
	else if (ret == FALSE)
	{
		ShowBalloon(WC_PORTAL_ABOUT_TEXT_TITLE, WC_PORTAL_RLDCONF_TEXT_ERR, 0, NIIF_ERROR);
	}

	installFileNotification();
	FlushMemory();
}

/* ------------------------------------------------------------------------------------------------- */

void FlushMemory()
{
	SetProcessWorkingSetSize(GetCurrentProcess(), 102400, 614400);
}

/* ------------------------------------------------------------------------------------------------- */
#ifdef DEV_MENU_DESIGN_SUBCLASSING_FOR_BORDER
HHOOK callWndHandle = NULL;

LRESULT WINAPI CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		CWPSTRUCT* pTemp = (CWPSTRUCT*)lParam;
	//	if (pTemp->hwnd == g_hwndMain)
		{
			//	HDC hdc = GetDC(g_hwndMain);
			switch (pTemp->message)
			{
			case WM_NCPAINT:
			case WM_ERASEBKGND:
			//	LRESULT ret = CallWindowProc(callWndHandle, pTemp->hwnd, pTemp->message, pTemp->wParam, pTemp->lParam);
				OnNcPaint(pTemp->hwnd, wParam, lParam);
				return 1;
			}
			//	ReleaseDC(g_hwndMain, hdc);
		}
	}
	return CallNextHookEx(callWndHandle, nCode, wParam, lParam);
}

/* ------------------------------------------------------------------------------------------------- */

void installCallWndHook()
{
	if (!callWndHandle)
	{
		callWndHandle = SetWindowsHookEx(WH_CALLWNDPROC, (HOOKPROC)CallWndProc, (HINSTANCE)NULL, GetCurrentThreadId());
	}
}

/* ------------------------------------------------------------------------------------------------- */

void uninstallCallWndHook()
{
	if (callWndHandle)
	{
		UnhookWindowsHookEx(callWndHandle);
		callWndHandle = NULL;
	}
}
#endif
/* ------------------------------------------------------------------------------------------------- */