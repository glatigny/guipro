/*
	HulK - GUIPro Project ( http://obsidev.github.io/guipro/ )

	Author : Glatigny Jérôme <jerome@obsidev.com> - http://www.obsidev.com/

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
#include "../plugin.inc.h"

#define SC_SMSEP 2
#define SC_TRAY 3
#define SC_ICONIZE 4

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
	g_hModule = (HINSTANCE)hModule;
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
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
					if(g_sysmenu_currHwnd == (HWND)wps->hwnd){
						HMENU sysMenu = GetSystemMenu(g_sysmenu_currHwnd, FALSE);
						RemoveMenu(sysMenu, SC_SMSEP, MF_BYCOMMAND);
						RemoveMenu(sysMenu, SC_TRAY, MF_BYCOMMAND);
						RemoveMenu(sysMenu, SC_ICONIZE, MF_BYCOMMAND);
					}
				}
			}
			break;

			case WM_CONTEXTMENU:
			case WM_INITMENUPOPUP:
			{
				// get the menu handle
				HMENU hMenu = (HMENU)wps->wParam;
				// check if it is a menu
				if(IsMenu(hMenu) && (HIWORD(wps->lParam) == TRUE) )
				{				
					g_sysmenu_currHwnd = (HWND)wps->hwnd;
					HMENU sysMenu = GetSystemMenu(g_sysmenu_currHwnd, FALSE);
					AppendMenu(sysMenu, MF_BYPOSITION|MF_SEPARATOR, SC_SMSEP, NULL);
					AppendMenu(sysMenu, MF_BYPOSITION|MF_STRING, SC_TRAY, L"Tray");
					AppendMenu(sysMenu, MF_BYPOSITION|MF_STRING, SC_ICONIZE, L"Iconize");
				}
			}
			break;
		}
	}
	return CallNextHookEx(0, nCode, wParam, lParam);
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
		}
	}
	return CallNextHookEx(0, nCode, wParam, lParam);
}

/* ------------------------------------------------------------------------------------------------- */