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
#include "trayIcon.h"
#include "main.h"
#include "resource.h"
#include "config.h"

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
	if( hIcon != NULL )
	{
		nid.hIcon = hIcon;
		nid.uFlags |= NIF_ICON;
	}
	if( szTip != NULL )
	{
		lstrcpyn(nid.szTip, szTip, SIZEOF_ARRAY(nid.szTip));
		nid.uFlags |= NIF_TIP;
	}
	return(Shell_NotifyIcon(NIM_MODIFY, &nid));
}

/* ------------------------------------------------------------------------------------------------- */

void ShowAbout(int pos)
{
	ShowBalloon(WC_PORTAL_ABOUT_TEXT_TITLE, WC_PORTAL_ABOUT_TEXT, pos, NIIF_USER);
	g_aboutbaloon = 1;
}

/* ------------------------------------------------------------------------------------------------- */

void HideBalloon(int pos)
{
	ShowBalloon(NULL, NULL, pos, NIIF_USER);
}

/* ------------------------------------------------------------------------------------------------- */

void ShowBalloon(wchar_t* title, wchar_t* text, int pos, DWORD type)
{
	NOTIFYICONDATA nid;
	g_aboutbaloon = 0;

	// Set special size in order to be compatible with win2000
	if(windowsVersion <= WINVER_2000)
	{
		nid.cbSize = NOTIFYICONDATA_V2_SIZE;
		if( type == NIIF_USER )
			nid.dwInfoFlags = NIIF_INFO;
		else
			nid.dwInfoFlags = type;
		// Display 10 seconds
		nid.uTimeout = 15;
	}
	else
	{
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.dwInfoFlags = type;
		nid.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_MAIN_ICON));
	}

	nid.hWnd = g_hwndMain;
	nid.uID = IDI_MAIN_ICON + pos;
	nid.uFlags = NIF_INFO;
	
	ZeroMemory(nid.szInfoTitle, SIZEOF_ARRAY(nid.szInfoTitle));
	ZeroMemory(nid.szInfo, SIZEOF_ARRAY(nid.szInfo));

	// Remove the old ballon
	Shell_NotifyIcon(NIM_MODIFY, &nid);

	if( title != NULL && text != NULL )
	{
		if( title != NULL )
			lstrcpyn(nid.szInfoTitle, title, SIZEOF_ARRAY(nid.szInfoTitle));
		if( text != NULL )
			lstrcpyn(nid.szInfo, text, SIZEOF_ARRAY(nid.szInfo));
	
		// Display the balloon
		Shell_NotifyIcon(NIM_MODIFY, &nid);
	}
}

/* ------------------------------------------------------------------------------------------------- */

void ShowTrayIcon(int pos)
{
	DeleteTrayIcon(g_hwndMain, IDI_MAIN_ICON + pos);

	if( (int)g_IconTray.size() > pos )
	{
		DestroyIcon(g_IconTray[pos]);
	}
	else
	{
		g_IconTray.push_back(NULL);
	}

	loadSystrayIcon(pos);

	const WCHAR* name = g_portal->menus[pos]->progName;
	if( (name == NULL) || name[0] == '\0' )
	{
		name = g_szWindowName;
	}
	AddTrayIcon(g_hwndMain, IDI_MAIN_ICON + pos, g_IconTray[pos], name);
}

/* ------------------------------------------------------------------------------------------------- */

void ShowTrayIcons()
{
	if( g_IconTray.size() > 0 )
	{
		DeleteTrayIcons();
	}

	int size = (int)g_portal->menus.size();
	for( int i = 0; i < size; i++ )
	{
		ShowTrayIcon(i);
	}
}

/* ------------------------------------------------------------------------------------------------- */

void ReloadTrayIcons()
{
	int size = (int)g_portal->menus.size();
	if( g_IconTray.size() == size )
	{
		const WCHAR* name = NULL;
		
		for( int i = 0; i < size; i++ )
		{
			loadSystrayIcon(i);

			name = g_portal->menus[i]->progName;
			if( (name == NULL) || name[0] == '\0' )
			{
				name = g_szWindowName;
			}

			if( !ReloadTrayIcon(g_hwndMain, IDI_MAIN_ICON + i, g_IconTray[i], name) )
			{
				AddTrayIcon(g_hwndMain, IDI_MAIN_ICON + i, g_IconTray[i], name);
			}
		}
	}
	else
	{
		DeleteTrayIcons();
		ShowTrayIcons();
	}
}

/* ------------------------------------------------------------------------------------------------- */

void DeleteTrayIcons()
{
	int size = (int)g_IconTray.size();
	for( int i = 0; i < size; i++ )
	{
		DeleteTrayIcon(g_hwndMain, IDI_MAIN_ICON + i);
	}

	for( std::vector<HICON>::iterator i = g_IconTray.begin(); i != g_IconTray.end(); i++ )
	{
		DestroyIcon( *i );
	}
	g_IconTray.clear();
}

/* ------------------------------------------------------------------------------------------------- */

void loadSystrayIcon(int pos)
{
	if( g_portal->menus[pos]->icoPath )
	{
		SHFILEINFO tSHFileInfo;
		ZeroMemory(&tSHFileInfo, sizeof(tSHFileInfo));
		SHGetFileInfo( g_portal->menus[pos]->icoPath , 0, &tSHFileInfo, sizeof(tSHFileInfo), SHGFI_ICON | SHGFI_SMALLICON);

		g_IconTray[pos] = tSHFileInfo.hIcon;
	}
	else if( g_portal->menus[pos]->progExe )
	{
		SHFILEINFO tSHFileInfo;
		ZeroMemory(&tSHFileInfo, sizeof(tSHFileInfo));
		DWORD_PTR ret = SHGetFileInfo( g_portal->menus[pos]->progExe , 0, &tSHFileInfo, sizeof(tSHFileInfo), SHGFI_ICON | SHGFI_SMALLICON);

		if( ret != 0 )
		{
			g_IconTray[pos] = tSHFileInfo.hIcon;
		}
		else
		{
			g_IconTray[pos] = (HICON)LoadImage(	g_hInst, MAKEINTRESOURCE(IDI_MAIN_ICON), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
		}
	}
	else
	{
		g_IconTray[pos] = (HICON)LoadImage(	g_hInst, MAKEINTRESOURCE(IDI_MAIN_ICON), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	}
}

/* ------------------------------------------------------------------------------------------------- */
