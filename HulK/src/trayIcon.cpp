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
#include "trayIcon.h"
#include "main.h"
#include "resource.h"

#ifndef NIIF_USER
#define NIIF_USER       0x00000004
#endif

/* ------------------------------------------------------------------------------------------------- */

BOOL AddTrayIcon(HWND hwnd, UINT uID, HICON hIcon, PCWSTR szTip)
{
	NOTIFYICONDATA nid; 
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.hWnd = hwnd;
	nid.uID = uID;
	nid.uCallbackMessage = WM_MYTRAYMSG;
	nid.hIcon = hIcon;
	lstrcpyn(nid.szTip, szTip, SIZEOF_ARRAY(nid.szTip));

	return(Shell_NotifyIcon(NIM_ADD, &nid));
}

/* ------------------------------------------------------------------------------------------------- */

BOOL DeleteTrayIcon(HWND hwnd, UINT uID)
{
	NOTIFYICONDATA nid; 
	nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = uID;
	return(Shell_NotifyIcon(NIM_DELETE, &nid));
}

/* ------------------------------------------------------------------------------------------------- */

BOOL ReloadTrayIcon(HWND hwnd, UINT uID)
{
	NOTIFYICONDATA nid; 
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.uFlags = NIF_STATE;
	nid.hWnd = hwnd;
	nid.uID = uID;
	nid.dwStateMask = NIS_HIDDEN;
	nid.dwState = 0;
	return(Shell_NotifyIcon(NIM_MODIFY, &nid));
}

/* ------------------------------------------------------------------------------------------------- */

void ShowAbout()
{
	ShowBalloon(WC_HULK_ABOUT_TEXT_TITLE, WC_HULK_ABOUT_TEXT, NIIF_USER);
}

/* ------------------------------------------------------------------------------------------------- */

void ShowBalloon(wchar_t* title, wchar_t* text, DWORD type)
{
	NOTIFYICONDATA nid;

	// Set special size in order to be compatible with win2000
	if(windows_version < WINVERSION(6,0))
	{
		nid.cbSize = NOTIFYICONDATA_V2_SIZE;
		if( type == NIIF_USER )
			nid.dwInfoFlags = NIIF_INFO;
		else
			nid.dwInfoFlags = type;
	}
	else
	{
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.dwInfoFlags = type | NIIF_NOSOUND;
	}

	nid.hWnd = g_hwndMain;
	nid.uID = IDI_MAIN_ICON;
	nid.uFlags = NIF_INFO;
	
	ZeroMemory(nid.szInfoTitle, SIZEOF_ARRAY(nid.szInfoTitle));
	lstrcpyn(nid.szInfoTitle, title, SIZEOF_ARRAY(nid.szInfoTitle));
	
	ZeroMemory(nid.szInfo, SIZEOF_ARRAY(nid.szInfo));
	lstrcpyn(nid.szInfo, text, SIZEOF_ARRAY(nid.szInfo));
	
	Shell_NotifyIcon(NIM_MODIFY, &nid);
}

/* ------------------------------------------------------------------------------------------------- */

void ReloadExTrayIcon()
{
	if( !ReloadTrayIcon(g_hwndMain, IDI_MAIN_ICON) )
	{
		ShowTrayIcon();
	}
}

/* ------------------------------------------------------------------------------------------------- */

void ShowTrayIcon()
{
	DeleteTrayIcon(g_hwndMain, IDI_MAIN_ICON);

	if( g_hIconTray )
	{
		DestroyIcon(g_hIconTray);
		g_hIconTray = NULL;
	}

	g_hIconTray = (HICON)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_TRAY_ICON), 
		IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);

	AddTrayIcon(g_hwndMain, IDI_MAIN_ICON, g_hIconTray, g_szWindowName);
}

/* ------------------------------------------------------------------------------------------------- */
