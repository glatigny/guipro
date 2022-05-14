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

#ifndef MAIN_H
#define MAIN_H

#include <windows.h>
#include "trayIcon.h"

extern const WCHAR g_szMainWnd[];
extern const WCHAR g_szExecuteurWnd[];
extern const WCHAR g_szWindowName[];

extern HINSTANCE g_hInst;
extern HWND g_hwndMain;
extern HICON g_hIconTray;
extern DWORD windows_version;

//
extern LPWSTR g_loadingmessage;

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void QuitHulK();
void FlushMemory();

HWND HulkGetPointedWindow();
HWND HulkGetActiveWindow();

#endif /* MAIN_H */
