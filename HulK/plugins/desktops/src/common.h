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

#ifndef _WIN32_WINNT
#  define _WIN32_WINNT 0x0501
#endif

#ifndef _WIN32_IE
#  define _WIN32_IE 0x0600
#endif

#define MAX_FILE_LEN	(1024)

#define SYSTEMMENU_ACTIVATE_TEXT		L"Automatic System Menu\nActivated"
#define SYSTEMMENU_DEACTIVATE_TEXT		L"Automatic System Menu\nDeactivated"

#include <windows.h>

UINT16 GetWinBuildNumber();
bool isWindows10();
void FlushMemory();

#endif /* COMMON_H */
