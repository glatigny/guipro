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

#ifndef COMMON_H
#define COMMON_H

/*
  When compiling without the WinXP support.
*/
#ifndef _USING_V110_SDK71_ // WIN8_SUPPORT

#ifndef _WIN32_WINNT
#  define _WIN32_WINNT 0x0601 // Windows 7
#endif
#ifndef _WIN32_IE
#  define _WIN32_IE 0x0800 // IE 8
#endif

#else /* _USING_V110_SDK71_ / WIN8_SUPPORT */

#ifndef _WIN32_WINNT
#  define _WIN32_WINNT 0x0501 // Windows XP
#endif
#ifndef _WIN32_IE
#  define _WIN32_IE 0x0600 // IE 6
#endif

#endif /* _USING_V110_SDK71_ / WIN8_SUPPORT */

/* ------------------------------------------------------------------------------------------------- */

#define ICON_MANAGER								// Icon manager system.
//#define PORTAL_REFUSE_HOTKEY_ERROR				// Old rules for the loading of the configuration.
//#define USE_GDI_MENU								// Use GDI+ to handle and display the icons (experimental)
//#define DEV_MENU_DESIGN_SUBCLASSING_FOR_BORDER	// Add an extra layer to draw the menu and avoid the white borders (experimental)
//#define USE_PATH_STORAGE							// Path Storage feature to reduce memory footprint (experimental)

/* ------------------------------------------------------------------------------------------------- */

#define PORTAL_HK_ID			400
#define PORTAL_FILE_ID			1000

/* ------------------------------------------------------------------------------------------------- */

#define PORTAL_CONFIG_LOAD_PARTIAL	2

/* MENU DEFINES */
#define PORTAL_MENU_SYSTRAY			0
#define PORTAL_MENU_SYSTRAY_CMD		1
#define PORTAL_MENU_SYSTRAY_AUTO	2
#define PORTAL_MENU_HOTKEY			3

#define PORTAL_CFG_OPT_QUIT			(0x01)
#define PORTAL_CFG_OPT_RELOAD		(0x02)
#define PORTAL_CFG_OPT_ABOUT		(0x04)
// Total of "CFG OPT"
#define PORTAL_CFG_OPT				(0x07)

#define PORTAL_ICON_RESOURCE		0x00100000

/* ------------------------------------------------------------------------------------------------- */

#define	ERR_MSGBOX_TITLE		L"Error"
#define ERR_HOTKEYS_MSG			L"Error during hotkeys registration.\nPortAL will keep the old correct configuration if possible.\nMaybe PortAL already launched or another application use some PortAL's hotkeys.\nKeys : "

/* ------------------------------------------------------------------------------------------------- */

#define MAX_SCLEN					30
#define MAX_LANGLEN					256
#define MAX_ERRHKLEN				30
#define MAX_FILE_LEN				(1024)
#define PARAMS_BUFFER_LNG			2048
#define CLIPBOARD_BUFFER_LNG		2048
#define SIZEOF_ARRAY(p)				(sizeof(p) /sizeof(p[0]))

/* ------------------------------------------------------------------------------------------------- */

#define WC_PORTAL_XML_FILENAME			L"portal.xml"

#define WC_PORTAL_ABOUT_ITEM_TEXT		L"About"
#define WC_PORTAL_QUIT_ITEM_TEXT		L"Quit"

#define PORTAL_VERSION					L"1.5.3"
#define WC_PORTAL_ABOUT_TEXT_TITLE		L"PortAL " PORTAL_VERSION
#define WC_PORTAL_ABOUT_TEXT			L"Portable Application Launcher\nhttp://glatigny.github.io/guipro/"

#define WC_PORTAL_RLDCONF_TEXT_OK		L"Configuration reloaded with success"
#define WC_PORTAL_RLDCONF_TEXT_ERR		L"Error while reloading the configuration"

/* ------------------------------------------------------------------------------------------------- */

#include <windows.h>

#define WINVER_unknow	0
#define WINVER_2000		1
#define WINVER_XP		2
#define WINVER_VISTA	3
#define WINVER_7		4
#define WINVER_8		5
#define WINVER_10		6

DWORD GetWindowsVersion();
bool fileExists(wchar_t* filename);
wchar_t* specialDirs(const wchar_t* cTemp, int mode = 0);
wchar_t* getClipboard(bool multiple = false);

#endif /* COMMON_H */
