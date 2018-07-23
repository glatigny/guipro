/*
	HulK - GUIPro Project ( http://glatigny.github.io/guipro/ )

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
#include "desktops.h"
#include "desktopmanager.h"
#include "../plugin.inc.h"

HINSTANCE g_hModule =  NULL;
wchar_t* text = NULL;
UINT g_id = 0;

#pragma data_seg(".SHARED")
HWND g_owner = NULL;
HWND g_sysmenu_currHwnd = NULL;
HHOOK g_callWndHook = NULL;
HHOOK g_getMsgHook = NULL;
#pragma data_seg()
#pragma comment(linker, "/SECTION:.SHARED,RWS")

void installCallbacks();
void uninstallCallbacks();

/* ------------------------------------------------------------------------------------------------- */

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if(!g_hModule)
		g_hModule = (HINSTANCE)hModule;
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			// releasePlugin();
			break;
	}
	return TRUE;
}

/* ------------------------------------------------------------------------------------------------- */

BOOL initPlugin(HWND p_owner, UINT id)
{
	if (!isWindows10())
		return FALSE;

	g_owner = p_owner;
	g_id = id;

	g_callWndHook = NULL;
	g_getMsgHook = NULL;
	installCallbacks();

	return TRUE;
}

/* ------------------------------------------------------------------------------------------------- */

BOOL releasePlugin()
{
	BOOL ret = TRUE;

	if( text )
	{
		free(text);
		text = NULL;
	}
	
	ReleaseVirtualDesktopManager();
	uninstallCallbacks();

	return ret;
}

/* ------------------------------------------------------------------------------------------------- */

BOOL activatePlugin()
{
	return TRUE;
}

/* ------------------------------------------------------------------------------------------------- */

void installCallbacks()
{
	if (g_callWndHook)
		return;

	g_callWndHook = SetWindowsHookEx(WH_CALLWNDPROC, (HOOKPROC)CallWndProc, g_hModule, 0);
	g_getMsgHook = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)GetMsgProc, g_hModule, 0);
	PostMessage(HWND_BROADCAST, WM_NULL, 0, 0);
}

void uninstallCallbacks()
{
	if (g_callWndHook)
		UnhookWindowsHookEx(g_callWndHook);
	if (g_getMsgHook)
		UnhookWindowsHookEx(g_getMsgHook);
	g_callWndHook = NULL;
	g_getMsgHook = NULL;
	PostMessage(HWND_BROADCAST, WM_NULL, 0, 0);
}

/* ------------------------------------------------------------------------------------------------- */

// Return of this function will be delete
wchar_t* getText()
{
	wchar_t* tmp = text;
	text = NULL;
	return tmp;
}

/* ------------------------------------------------------------------------------------------------- */

BOOL setOption(const wchar_t* name, const wchar_t* value)
{
	if (!_wcsicmp(name, L"activate"))
	{
		if (!_wcsicmp(value, L"false"))
		{
			if (g_callWndHook)
			{
				installCallbacks();
				free(text);
				text = NULL;
			}
			return TRUE;
		}
		else if (!_wcsicmp(value, L"true"))
		{
			if (!g_callWndHook)
			{
				activatePlugin();
				free(text);
				text = NULL;
			}
			return TRUE;
		}
	}
	return FALSE;
}

/* ------------------------------------------------------------------------------------------------- */

UINT getAction(const wchar_t* function)
{
	UINT ret = 0;
	wchar_t* l_wc = _wcsdup(function);
	_wcslwr_s(l_wc, wcslen(l_wc) + 1);

#define DEF_FCT(name, id)	if(!wcscmp(l_wc, L ## name)) { ret = id; }

	DEF_FCT("desktopleft", 1)
	else DEF_FCT("desktopright", 2)
	else DEF_FCT("moveleft", 3)
	else DEF_FCT("moveright", 4)
	else DEF_FCT("switchleft", 5)
	else DEF_FCT("switchright", 6)

#undef DEF_FCT

	free(l_wc);
	return ret;
}

/* ------------------------------------------------------------------------------------------------- */

UINT callAction(UINT function, HWND target)
{
	if(function >= 3 && function <= 6 && !g_callWndHook)
		installCallbacks();

	switch (function)
	{
	case 1:
		switchVirtualDesktop(deskDirection::left);
		break;
	case 2:
		switchVirtualDesktop(deskDirection::right);
		break;
	// Post message which will be handle by CallWndProc
	// We cannot call directly "moveWindowToNextDesktop" since we are in the "hulk" context and no authority on the target
	case 3:
		PostMessage(target, WM_SYSCOMMAND, SC_DESKTOP_SEND_LEFT, 0);
		break;
	case 4:
		PostMessage(target, WM_SYSCOMMAND, SC_DESKTOP_SEND_RIGHT, 0);
		break;
	case 5:
		PostMessage(target, WM_SYSCOMMAND, SC_DESKTOP_MOVE_LEFT, 0);
		break;
	case 6:
		PostMessage(target, WM_SYSCOMMAND, SC_DESKTOP_MOVE_RIGHT, 0);
		break;
	}
	return 0;
}

/* ------------------------------------------------------------------------------------------------- */

void addSysMenuItems(HWND hwnd, HMENU menu)
{
	if (g_sysmenu_currHwnd == hwnd)
		return;
	g_sysmenu_currHwnd = hwnd;
	HMENU sysMenu = GetSystemMenu(g_sysmenu_currHwnd, FALSE);
	if (menu != INVALID_HANDLE_VALUE && menu != sysMenu)
		return;
	//
	InsertMenu(sysMenu, SC_CLOSE, MF_BYCOMMAND | MF_STRING, SC_DESKTOP_MOVE_LEFT, L"Move left");
	InsertMenu(sysMenu, SC_CLOSE, MF_BYCOMMAND | MF_STRING, SC_DESKTOP_MOVE_RIGHT, L"Move right");
	InsertMenu(sysMenu, SC_CLOSE, MF_BYCOMMAND | MF_SEPARATOR, SC_DESKTOP_SEP, NULL);
}

void removeSysMenuItems(HWND hwnd, HMENU menu)
{
	if (g_sysmenu_currHwnd != hwnd)
		return;
	HMENU sysMenu = GetSystemMenu(g_sysmenu_currHwnd, FALSE);
	if (menu != INVALID_HANDLE_VALUE && menu != sysMenu)
		return;
	DeleteMenu(sysMenu, SC_DESKTOP_MOVE_LEFT, MF_BYCOMMAND);
	DeleteMenu(sysMenu, SC_DESKTOP_MOVE_RIGHT, MF_BYCOMMAND);
	DeleteMenu(sysMenu, SC_DESKTOP_SEP, MF_BYCOMMAND);
	g_sysmenu_currHwnd = NULL;
}

/* ------------------------------------------------------------------------------------------------- */

void processSysAction(HWND target, WORD action)
{
	HRESULT hr = S_FALSE;
	switch (action)
	{
		case SC_DESKTOP_SEND_LEFT:
			moveWindowToPreviousDesktop(target);
			return;
		case SC_DESKTOP_SEND_RIGHT:
			moveWindowToNextDesktop(target);
			return;
		case SC_DESKTOP_MOVE_LEFT:
		case SC_DESKTOP_MOVE_RIGHT:
		{
			// TODO : add a parameter to moveWindowToAdjacentDesktop to perform switch too.
			deskDirection dir = (action == SC_DESKTOP_MOVE_LEFT) ? deskDirection::left : deskDirection::right;
			hr = moveWindowToAdjacentDesktop(target, dir);
			if (SUCCEEDED(hr))
				switchVirtualDesktop(dir);
			return;
		}
		case SC_DESKTOP_MOVE_RIGHT_NEW:
			// TODO
			moveWindowToAdjacentDesktop(target, deskDirection::right);
			return;
	}
}

/* ------------------------------------------------------------------------------------------------- */

LRESULT CALLBACK CallWndProc(INT code, WPARAM wParam, LPARAM lParam)
{
	if (code >= HC_ACTION)
	{
		CWPSTRUCT *wps = (CWPSTRUCT*)lParam;

		switch (wps->message)
		{
			case WM_MENUSELECT:
			{
				// menu is being closed
				if (wps->lParam == NULL && HIWORD(wps->wParam) == 0xFFFF)
				{
					removeSysMenuItems(wps->hwnd, (HMENU)INVALID_HANDLE_VALUE);
				}
			}
			break;

			case WM_UNINITMENUPOPUP:
			{
				removeSysMenuItems(wps->hwnd, (HMENU)wps->wParam);
			}
			break;

			case WM_ENTERIDLE:
			{
				// Some applications trigger WM_INITMENUPOPUP never or to late, thats why we use WM_ENTERIDLE
				// CF: https://github.com/Eun/MoveToDesktop/
				if (wps->wParam == MSGF_MENU)
				{
					addSysMenuItems(wps->hwnd, (HMENU)INVALID_HANDLE_VALUE);
				}
			}
			break;

			case WM_CONTEXTMENU:
			case WM_INITMENUPOPUP:
			{
				// Get the menu handle
				HMENU hMenu = (HMENU)wps->wParam;
				// Check if it is a menu
				if (IsMenu(hMenu) && (HIWORD(wps->lParam) == TRUE) && g_sysmenu_currHwnd != (HWND)wps->hwnd)
				{
					addSysMenuItems(wps->hwnd, hMenu);
				}
			}
			break;

			case WM_SYSCOMMAND:
			{
				WORD l_wp = LOWORD(((PCWPSTRUCT)lParam)->wParam);
				if ((l_wp >= SC_DESKTOP_MOVE_LEFT && l_wp <= SC_DESKTOP_MOVE_RIGHT_NEW) || (l_wp >= SC_DESKTOP_MOVE_TO && l_wp <= SC_DESKTOP_MOVE_TO + MAX_DESKTOPS))
					processSysAction(((PCWPSTRUCT)lParam)->hwnd, l_wp);
			}
			break;
		}
	}
	return CallNextHookEx(g_callWndHook, code, wParam, lParam);
}

/* ------------------------------------------------------------------------------------------------- */

LRESULT CALLBACK GetMsgProc(INT nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= HC_ACTION)
	{
		MSG *msg = (MSG*)lParam;
		if (msg->message == WM_SYSCOMMAND)
		{
			WORD dw = LOWORD(msg->wParam);
			if ((dw >= SC_DESKTOP_MOVE_LEFT && dw <= SC_DESKTOP_MOVE_RIGHT_NEW) || (dw >= SC_DESKTOP_MOVE_TO && dw <= SC_DESKTOP_MOVE_TO + MAX_DESKTOPS))
			{
				processSysAction((HWND)msg->hwnd, dw);
			}
		}
	}
	return CallNextHookEx(g_getMsgHook, nCode, wParam, lParam);
}

/* ------------------------------------------------------------------------------------------------- */