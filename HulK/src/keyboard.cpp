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

#include "common.h"
#include "keyboard.h"

/* ------------------------------------------------------------------------------------------------- */

UINT key_send_code(WORD keycode, DWORD flags)
{
	INPUT ip;

	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	ip.ki.wVk = keycode;
	ip.ki.dwFlags = flags; /* 0 | KEYEVENTF_KEYUP */

	// Handle extended keys
	//
	switch (keycode)
	{
		case VK_SHIFT:
		case VK_RSHIFT:
		case VK_LSHIFT:
		case VK_MENU:
		case VK_RMENU:
		case VK_LMENU:
		case VK_CONTROL:
		case VK_RCONTROL:
		case VK_LCONTROL:
		case VK_UP:
		case VK_DOWN:
		case VK_LEFT:
		case VK_RIGHT:
		case VK_PRIOR:
		case VK_NEXT:
			ip.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
			break;
	}

	return SendInput(1, &ip, sizeof(INPUT));
}

/* ------------------------------------------------------------------------------------------------- */

UINT key_send_scan(WORD scan, DWORD flags)
{
	INPUT ip;

	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = scan;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	ip.ki.wVk = 0;
	ip.ki.dwFlags = flags; /* 0 | KEYEVENTF_KEYUP */

	if ((scan & 0xFF00) == 0xE000)
	{
		ip.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
	}

	return SendInput(1, &ip, sizeof(INPUT));
}

/* ------------------------------------------------------------------------------------------------- */

void key_press_code(WORD keycode)
{
	key_send_code(keycode, 0);
	key_send_code(keycode, KEYEVENTF_KEYUP);
}

/* ------------------------------------------------------------------------------------------------- */

void key_press_scan(WORD scan)
{
	key_send_scan(scan, 0);
	key_send_scan(scan, KEYEVENTF_KEYUP);
}

/* ------------------------------------------------------------------------------------------------- */