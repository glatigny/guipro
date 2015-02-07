/*  NoMad - GUIPro Project ( http://guipro.sourceforge.net/ )

    Author : DarkSage  aka  Glatigny Jérôme <darksage@darksage.fr>

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
#  define _WIN32_WINNT 0x0601
#endif

#ifndef _WIN32_IE
#  define _WIN32_IE 0x0800
#endif

/* ------------------------------------------------------------------------------------------------- */

#define WINVER_unknow	0
#define WINVER_2000		1
#define WINVER_XP		2
#define WINVER_VISTA	3
#define WINVER_7		4

/* ------------------------------------------------------------------------------------------------- */

#define WC_VULCAN_ABOUT_TEXT_TITLE		L"VulCan 0.9"
#define WC_VULCAN_ABOUT_TEXT			L"VulCan\nhttp://guipro.sourceforge.net"

/* ------------------------------------------------------------------------------------------------- */

#define	ERR_MSGBOX_TITLE		L"Error"
#define ERR_HOTKEYS_MSG			L"Error during hotkeys registration.\nMaybe VulCan already launched or another application use some VulCan's hotkeys.\nKeys : "

/* ------------------------------------------------------------------------------------------------- */

#define SET_RESSOURCE_ICON(icon_size,icon_index)	SendMessage(hwnd, WM_SETICON, icon_size, (LONG)(LONG_PTR)LoadImage(g_hInst, MAKEINTRESOURCE(icon_index), IMAGE_ICON, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR))
#define SIZEOF_ARRAY(p) (sizeof(p) /sizeof(p[0]))
#define uint unsigned int
#define ulong unsigned long

#include <windows.h>
#include <commctrl.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

DWORD GetWindowsVersion();

#endif /* COMMON_H */
