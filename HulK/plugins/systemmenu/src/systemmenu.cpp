/*
	HulK - GUIPro Project ( http://glatigny.github.io/guipro/ )

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

// http://www.codeproject.com/KB/system/TrayMe.aspx?msg=3142302

#include "common.h"
#include "systemmenu.h"
#include <plugin.inc.h>

#define SC_SMSEP	0xA001
#define SC_TRAY		0xA002
#define SC_ICONIZE	0xA003
#define SC_ONTOP	0xA004

HINSTANCE g_hModule =  NULL;
wchar_t* text = NULL;
UINT g_id = 0;

#pragma data_seg(".SHARED")
HWND g_owner = NULL;
HWND g_sysmenu_currHwnd = NULL;
HHOOK g_hook1 = NULL;
HHOOK g_hook2 = NULL;
#pragma data_seg()
#pragma comment(linker, "/SECTION:.SHARED,RWS")

/* ------------------------------------------------------------------------------------------------- */

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);

	g_hModule = (HINSTANCE)hModule;
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			//releasePlugin();
			break;
	}
	return TRUE;
}

/* ------------------------------------------------------------------------------------------------- */

BOOL initPlugin(HWND p_owner, UINT id)
{
	g_owner = p_owner;
	g_id = id;
	g_hook1 = SetWindowsHookEx(WH_CALLWNDPROC, (HOOKPROC)CALLProc, g_hModule, 0);
	g_hook2 = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)GETMSGProc, g_hModule, 0);
	return TRUE;
}

/* ------------------------------------------------------------------------------------------------- */

BOOL releasePlugin()
{
	BOOL ret = TRUE;
	if( g_hook1 )
	{
		ret = UnhookWindowsHookEx(g_hook1);
		g_hook1 = NULL;
	}

	if( g_hook2 )
	{
		ret &= UnhookWindowsHookEx(g_hook2);
		g_hook2 = NULL;
	}

	if( text )
	{
		free(text);
		text = NULL;
	}

	return ret;
}

/* ------------------------------------------------------------------------------------------------- */

BOOL activatePlugin()
{
	if( g_hook1 )
	{
		UnhookWindowsHookEx(g_hook1);
		g_hook1 = NULL;

		UnhookWindowsHookEx(g_hook2);
		g_hook2 = NULL;

		PostMessage(HWND_BROADCAST, WM_NULL, 0, 0);

		if( text )
		{
			free(text);
		}

		text = _wcsdup(SYSTEMMENU_DEACTIVATE_TEXT);

		return PORTAL_PLUGIN_ERROR_TXT;
	}
	else
	{
		g_hook1 = SetWindowsHookEx(WH_CALLWNDPROC, (HOOKPROC)CALLProc, g_hModule, 0);
		g_hook2 = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)GETMSGProc, g_hModule, 0);
		PostMessage(HWND_BROADCAST, WM_NULL, 0, 0);

		if( text )
		{
			free(text);
		}

		text = _wcsdup(SYSTEMMENU_ACTIVATE_TEXT);

		return PORTAL_PLUGIN_RETURN_TXT;
	}
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

BOOL setOption(wchar_t* name, wchar_t* value)
{
	if( !_wcsicmp(name, L"activate") )
	{
		if( !_wcsicmp(value, L"false") )
		{
			if( g_hook1 )
			{
				activatePlugin();
				free(text);
				text = NULL;
			}
			return TRUE;
		}
		else if( !_wcsicmp(value, L"true") )
		{
			if( !g_hook1 )
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

void addSysMenuItems(HWND hwnd, HMENU menu)
{
	if (g_sysmenu_currHwnd == hwnd)
		return;
	g_sysmenu_currHwnd = hwnd;
	HMENU sysMenu = GetSystemMenu(g_sysmenu_currHwnd, FALSE);
	if (menu != INVALID_HANDLE_VALUE && menu != sysMenu)
		return;

	LONG_PTR lpExStyle = GetWindowLongPtr(g_sysmenu_currHwnd, GWL_EXSTYLE);

	// Insert is made in reverse
	InsertMenu(sysMenu, SC_CLOSE, MF_BYCOMMAND | MF_STRING, SC_ICONIZE, L"Iconize");
	InsertMenu(sysMenu, SC_CLOSE, MF_BYCOMMAND | MF_STRING, SC_TRAY, L"Tray");
	if (lpExStyle & WS_EX_TOPMOST)
		InsertMenu(sysMenu, SC_CLOSE, MF_BYCOMMAND | MF_STRING | MF_CHECKED, SC_ONTOP, L"On top");
	else
		InsertMenu(sysMenu, SC_CLOSE, MF_BYCOMMAND | MF_STRING, SC_ONTOP, L"On top");
	InsertMenu(sysMenu, SC_CLOSE, MF_BYCOMMAND | MF_SEPARATOR, SC_SMSEP, NULL);
}

void removeSysMenuItems(HWND hwnd, HMENU menu)
{
	if (g_sysmenu_currHwnd != hwnd)
		return;
	HMENU sysMenu = GetSystemMenu(g_sysmenu_currHwnd, FALSE);
	if (menu != INVALID_HANDLE_VALUE && menu != sysMenu)
		return;
	DeleteMenu(sysMenu, SC_SMSEP, MF_BYCOMMAND);
	DeleteMenu(sysMenu, SC_ONTOP, MF_BYCOMMAND);
	DeleteMenu(sysMenu, SC_TRAY, MF_BYCOMMAND);
	DeleteMenu(sysMenu, SC_ICONIZE, MF_BYCOMMAND);
	g_sysmenu_currHwnd = NULL;
}

/* ------------------------------------------------------------------------------------------------- */

LRESULT CALLBACK CALLProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if( nCode >= 0 )
	{
		CWPSTRUCT *wps = (CWPSTRUCT*)lParam;

		switch (wps->message)
		{
			case WM_MENUSELECT:
			{
				// menu is being closed
				if(wps->lParam == NULL && HIWORD(wps->wParam) == 0xFFFF)
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
				// get the menu handle
				HMENU hMenu = (HMENU)wps->wParam;
				// check if it is a menu
				if(IsMenu(hMenu) && (HIWORD(wps->lParam) == TRUE) && g_sysmenu_currHwnd != (HWND)wps->hwnd)
				{
					addSysMenuItems(wps->hwnd, hMenu);
				}
			}
			break;
		}
	}
	return CallNextHookEx(g_hook1, nCode, wParam, lParam);
}

/* ------------------------------------------------------------------------------------------------- */

LRESULT CALLBACK GETMSGProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if( nCode >= 0 )
	{
		MSG *msg = (MSG*)lParam;
		if( msg->message == WM_SYSCOMMAND )
		{
			if(LOWORD(msg->wParam) == SC_TRAY )
			{
				// tray
				// (HWND)msg->hwnd
				SendMessage( g_owner, WM_PLUGIN_EVENT, (WPARAM)msg->hwnd , PORTAL_TRAYNIZE );
			}
			else if(LOWORD(msg->wParam) == SC_ICONIZE )
			{
				// tray
				// (HWND)msg->hwnd
				SendMessage( g_owner, WM_PLUGIN_EVENT, (WPARAM)msg->hwnd , PORTAL_ICONIZE );
			}
			else if (LOWORD(msg->wParam) == SC_ONTOP)
			{
				// tray
				// (HWND)msg->hwnd
				SendMessage(g_owner, WM_PLUGIN_EVENT, (WPARAM)msg->hwnd, PORTAL_ONTOP);
			}
		}
	}
	return CallNextHookEx(g_hook2, nCode, wParam, lParam);
}

/* ------------------------------------------------------------------------------------------------- */