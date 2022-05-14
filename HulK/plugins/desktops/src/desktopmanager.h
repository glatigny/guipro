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

#pragma once

#define SC_DESKTOP_SEND_LEFT			0x5001
#define SC_DESKTOP_SEND_RIGHT			0x5002
#define SC_DESKTOP_SEND_RIGHT_NEW		0x5003
#define SC_DESKTOP_MOVE_LEFT			0x5004
#define SC_DESKTOP_MOVE_RIGHT			0x5005
#define SC_DESKTOP_MOVE_RIGHT_NEW		0x5006

#define SC_DESKTOP_SEP					0x5007

#define SC_DESKTOP_MOVE_TO				0x5100

#define MAX_DESKTOPS					50

enum deskDirection {
	left = 0,
	right = 1
};

HRESULT GetVirtualDesktopManager();
HRESULT ReleaseVirtualDesktopManager();

HRESULT switchVirtualDesktop(deskDirection dir);
HRESULT createVirtualDesktop();
UINT getVirtualDesktopCount();

HRESULT moveWindowToAdjacentDesktop(HWND hwnd, deskDirection dir);
HRESULT moveWindowToAdjacentDesktop(HWND hwnd, deskDirection dir, bool create);
HRESULT moveWindowToPreviousDesktop(HWND hwnd);
HRESULT moveWindowToNextDesktop(HWND hwnd);

HRESULT moveWindowToDesktopId(HWND hwnd, UINT id);