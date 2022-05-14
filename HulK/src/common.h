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

#ifndef COMMON_H
#define COMMON_H

#ifdef WIN10_SUPPORT

#ifndef _WIN32_WINNT
#  define _WIN32_WINNT 0x0603 // Windows 8.1
#endif
#ifndef _WIN32_IE
#  define _WIN32_IE 0x0800 // IE 8
#endif

#else /* WIN10_SUPPORT */

#ifndef _WIN32_WINNT
#  define _WIN32_WINNT 0x0501 // Windows XP
#endif
#define TEST _WIN32_WINNT_WINBLUE

#ifndef _WIN32_IE
#  define _WIN32_IE 0x0600 // IE 6
#endif

#endif /* WIN10_SUPPORT */

/* ------------------------------------------------------------------------------------------------- */

#define MAX_PLUGIN_LEN			128
#define MAX_FILE_LEN			256
#define LINE_MAX_SIZE			1024

/* ------------------------------------------------------------------------------------------------- */

#define IDM_TRAYS				200
#define IDM_EXIT				303
//#define IDM_ABOUT				304
#define IDM_OPEN_ICONIZE		305
#define IDM_CLOSE_ICONIZE		306
#define IDM_ICONIZE_MENU		307

/* ------------------------------------------------------------------------------------------------- */

#define IDT_TIMERSYSTRAY		001

/* ------------------------------------------------------------------------------------------------- */

#define HULK_MIN_SIZE_PX		5

/* ------------------------------------------------------------------------------------------------- */

#define HULK_CONFIG_LOAD_PARTIAL	2

/* ------------------------------------------------------------------------------------------------- */

#define WC_HULK_XML_FILENAME		L"hulk.xml"

#define WC_HULK_ABOUT_ITEM_TEXT		L"About"
#define WC_HULK_QUIT_ITEM_TEXT		L"Quit"

#define HULK_VERSION				L"0.9.9"
#define WC_HULK_ABOUT_TEXT_TITLE	L"HulK " HULK_VERSION
#define WC_HULK_ABOUT_TEXT			L"HotKey UnLimited\nhttp://glatigny.github.io/guipro/"

#define WC_HULK_ERROR_MAX_APPS		L"Iconized windows limit reached"
#define WC_HULK_RLDCONF_TEXT_OK		L"Configuration reloaded with success"
#define WC_HULK_RLDCONF_TEXT_ERR	L"Error while reloading the configuration"

/* ------------------------------------------------------------------------------------------------- */

#define	ERR_MSGBOX_TITLE		L"Error"
#define ERR_HOTKEYS_MSG			L"Error during hotkeys registration.\nMaybe HulK already launched or another application use some HulK's hotkeys.\nKeys : "

/* ------------------------------------------------------------------------------------------------- */

#define MAX_LENGTH					256
#define MAX_ERRHKLEN				30
#define SIZEOF_ARRAY(p)				(sizeof(p) /sizeof(p[0]))

#include <windows.h>

#ifndef NDEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DBG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DBG_NEW
#define DBG_MALLOC(size) _malloc_dbg(size, _NORMAL_BLOCK, __FILE__, __LINE__ )
#undef malloc
#define malloc(size) DBG_MALLOC(size)
#define DBG_FREE(el) _free_dbg(el, _NORMAL_BLOCK)
#undef free
#define free(el) DBG_FREE(el)
#endif

LPSTR UnicodeToAnsi(LPCWSTR s);
wchar_t* AnsiToUnicode(const char* cTemp);

#define WINVERSION(major,minor) MAKELONG(minor,major)
DWORD GetDllVersion(LPCTSTR lpszDllName);

bool fileExists(wchar_t* filename);
wchar_t* specialDirs(const wchar_t* cTemp, int mode = 0);

#endif /* COMMON_H */
