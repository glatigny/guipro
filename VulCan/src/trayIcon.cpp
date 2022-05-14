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
#include "trayIcon.h"
#include "main.h"
#include "volume.h"
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

BOOL ReloadTrayIcon(HWND hwnd, UINT uID, HICON hIcon, PCWSTR szTip)
{
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.uFlags = NIF_STATE;
	nid.hWnd = hwnd;
	nid.uID = uID;
	if (hIcon != NULL)
	{
		nid.hIcon = hIcon;
		nid.uFlags |= NIF_ICON;
	}
	if (szTip != NULL)
	{
		lstrcpyn(nid.szTip, szTip, SIZEOF_ARRAY(nid.szTip));
		nid.uFlags |= NIF_TIP;
	}
	return(Shell_NotifyIcon(NIM_MODIFY, &nid));
}

/* ------------------------------------------------------------------------------------------------- */

void ShowAbout()
{
	ShowBalloon(WC_VULCAN_ABOUT_TEXT_TITLE, WC_VULCAN_ABOUT_TEXT, NIIF_USER);
	g_aboutballoon = 1;
}

/* ------------------------------------------------------------------------------------------------- */

void ShowVolumeBalloon(int vol) {
	if (vol < 0)
		vol = getVolume();

	if (vol > 0)
	{
		size_t size = sizeof(wchar_t) * (wcslen(WC_VULCAN_VOLUME_CHANGE) + 5);
		wchar_t* l_volume = (wchar_t*)malloc(size);
		memset(l_volume, 0, size);
		wsprintfW(l_volume, WC_VULCAN_VOLUME_CHANGE, vol);
		ShowBalloon(WC_VULCAN_VOLUME_TITLE, l_volume, NIIF_USER);
		free(l_volume);
	}
	else if (vol == 0)
	{
		ShowBalloon(WC_VULCAN_VOLUME_TITLE, WC_VULCAN_VOLUME_MUTE, NIIF_USER);
	}
}

/* ------------------------------------------------------------------------------------------------- */

void ShowBalloon(wchar_t* title, wchar_t* text, DWORD type)
{
	NOTIFYICONDATA nid;
	g_aboutballoon = 0;

	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.dwInfoFlags = type | NIIF_NOSOUND | NIIF_LARGE_ICON;
	nid.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_TRAY_ICON));
	nid.uVersion = NOTIFYICON_VERSION_4;
	nid.hBalloonIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_MAIN_ICON));

	nid.hWnd = g_hwndMain;
	nid.uID = IDI_MAIN_ICON;
	nid.uFlags = NIF_INFO | NIF_REALTIME;

	ZeroMemory(nid.szInfoTitle, SIZEOF_ARRAY(nid.szInfoTitle));
	ZeroMemory(nid.szInfo, SIZEOF_ARRAY(nid.szInfo));
	
//	// Remove the old ballon
//	Shell_NotifyIcon(NIM_MODIFY, &nid);

	if (title != NULL && text != NULL)
	{
		if (title != NULL)
			lstrcpyn(nid.szInfoTitle, title, SIZEOF_ARRAY(nid.szInfoTitle));
		if (text != NULL)
			lstrcpyn(nid.szInfo, text, SIZEOF_ARRAY(nid.szInfo));
	}

	// Display the balloon
	Shell_NotifyIcon(NIM_MODIFY, &nid);
}

/* ------------------------------------------------------------------------------------------------- */

void HideBalloon()
{
	ShowBalloon(NULL, NULL, NIIF_USER);
}

/* ------------------------------------------------------------------------------------------------- */

void ReloadExTrayIcon()
{
	if( !ReloadTrayIcon(g_hwndMain, IDI_MAIN_ICON, NULL, NULL) )
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
