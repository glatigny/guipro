/*  PortAL - GUIPro Project ( http://guipro.sourceforge.net/ )

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
#  define _WIN32_WINNT 0x0501
#endif

#ifndef _WIN32_IE
#  define _WIN32_IE 0x0600
#endif

#define MAX_FILE_LEN	(1024)

#define WDHOOK_ACTIVATE_TEXT		L"Automatic Borderless Windows\nActivated"
#define WDHOOK_DEACTIVATE_TEXT		L"Automatic Borderless Windows\nDeactivated"

#include <windows.h>

void setBorderless(HWND p_hwnd);
void setBorderfull(HWND p_hwnd);

#endif /* COMMON_H */
