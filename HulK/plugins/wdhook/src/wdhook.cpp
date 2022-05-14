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

#include "common.h"
#include "wdhook.h"
#include <plugin.inc.h>

HINSTANCE g_hModule =  NULL;
wchar_t* text = NULL;
UINT g_id = 0;

#pragma data_seg(".WindowHookDll")
HWND g_owner = NULL;
HHOOK g_hook = NULL;
#pragma data_seg()
#pragma comment(linker, "/section:.WindowHookDll,rws")

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
	g_hook = SetWindowsHookEx(WH_CBT, CBTProc, g_hModule, 0);
	return TRUE;
}

/* ------------------------------------------------------------------------------------------------- */

BOOL releasePlugin()
{
	BOOL ret = TRUE;
	if( g_hook )
	{
		ret = UnhookWindowsHookEx(g_hook);
		g_hook = NULL;
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
	if( g_hook )
	{
		UnhookWindowsHookEx(g_hook);
		g_hook = NULL;
		
		if( text )
		{
			free(text);
		}

		text = _wcsdup(WDHOOK_DEACTIVATE_TEXT);

		return PORTAL_PLUGIN_ERROR_TXT;
	}
	else
	{
		g_hook = SetWindowsHookEx(WH_CBT, CBTProc, g_hModule, 0);
		
		if( text )
		{
			free(text);
		}

		text = _wcsdup(WDHOOK_ACTIVATE_TEXT);

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
			if( g_hook )
			{
				activatePlugin();
				free(text);
				text = NULL;
			}
			return TRUE;
		}
		else if( !_wcsicmp(value, L"true") )
		{
			if( !g_hook )
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

LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if( nCode >= 0 )
	{
		if( (nCode == HCBT_MINMAX) ) 
		{
			if( LOWORD(lParam) == SW_MAXIMIZE )
			{
				setBorderless( (HWND) wParam);
			}
			else if( LOWORD(lParam) == SW_RESTORE )
			{
				setBorderfull( (HWND) wParam);
			}
		}
		return 0;
	}
	return CallNextHookEx(0, nCode, wParam, lParam);
}

/* ------------------------------------------------------------------------------------------------- */
