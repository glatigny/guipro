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

#ifndef COMMON_H
#define COMMON_H

#ifndef _WIN32_WINNT
#  define _WIN32_WINNT 0x0501
#endif

#ifndef _WIN32_IE
#  define _WIN32_IE 0x0600
#endif

/* ------------------------------------------------------------------------------------------------- */

#define MAX_PLUGIN_LEN			128
#define MAX_FILE_LEN			256
#define LINE_MAX_SIZE			1024

/* ------------------------------------------------------------------------------------------------- */

#define IDM_TRAYS				200
#define IDM_EXIT				303
#define IDM_ABOUT				304
#define IDM_OPEN_ICONIZE		305
#define IDM_CLOSE_ICONIZE		306
#define IDM_ICONIZE_MENU		307

/* ------------------------------------------------------------------------------------------------- */

#define IDT_TIMERSYSTRAY		001

/* ------------------------------------------------------------------------------------------------- */

#define HULK_MIN_SIZE_PX		5

/* ------------------------------------------------------------------------------------------------- */

#define WC_HULK_ABOUT_TEXT_TITLE	L"HulK 0.9.5"
#define WC_HULK_ABOUT_TEXT			L"HotKey UnLimited\nhttp://obsidev.github.io/guipro/"
#define WC_HULK_ERROR_MAX_APPS		L"Iconized windows limit reached"

/* ------------------------------------------------------------------------------------------------- */

#define	ERR_MSGBOX_TITLE		L"Error"
#define ERR_HOTKEYS_MSG			L"Error during hotkeys registration.\nMaybe HulK already launched or another application use some HulK's hotkeys.\nKeys : "

/* ------------------------------------------------------------------------------------------------- */

#define MAX_LENGTH					256
#define MAX_ERRHKLEN				30
#define SIZEOF_ARRAY(p)				(sizeof(p) /sizeof(p[0]))

#include <windows.h>

LPSTR UnicodeToAnsi(LPCWSTR s);
wchar_t* AnsiToUnicode(const char* cTemp);

#define WINVERSION(major,minor) MAKELONG(minor,major)
DWORD GetDllVersion(LPCTSTR lpszDllName);

#endif /* COMMON_H */
