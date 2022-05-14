/*
	PortAL - GUIPro Project ( http://glatigny.github.io/guipro/ )

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

#ifndef __TRAYICON_H__
#define __TRAYICON_H__

#include <windows.h>
#include <ShellAPI.h>

#define WM_MYTRAYMSG    (WM_USER + 100)
#define WM_MYREDRAWMENU (WM_USER + 101)
#define WM_MYREDRAWITEM (WM_USER + 102)

BOOL AddTrayIcon(HWND hwnd, UINT uID, HICON hIcon, PCWSTR szTip);
BOOL DeleteTrayIcon(HWND hwnd, UINT uID);
BOOL ReloadTrayIcon(HWND hwnd, UINT uID, HICON hIcon, PCWSTR szTip);

void ShowAbout(int pos);
void ShowBalloon(wchar_t* title, wchar_t* text, int pos, DWORD type);
void HideBalloon(int pos);

void ReloadTrayIcons();

void ShowTrayIcons();
void ShowTrayIcon(int pos);

void DeleteTrayIcons();

void loadSystrayIcon(size_t pos);

#endif /* __TRAYICON_H__ */
