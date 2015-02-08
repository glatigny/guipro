/*
	VulCan - GUIPro Project ( http://obsidev.github.io/guipro/ )

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

#ifndef MOUSE_H
#define MOUSE_H

#include "common.h"
#include "config.h"

// Special definition of the function in order to activate Hooking in an executable program
// in general, hooking must be in a library (dll)
extern "C" __declspec(dllexport) LRESULT CALLBACK hookMouse(int nCode, WPARAM wParam, LPARAM lParam);

extern bool g_mouseUsed[VL_MOUSE_BTN_MAX];

void installHookMouse();
void uninstallHookMouse();

UINT mouseEvent(UINT p_event, bool status, LPARAM lParam);
UINT mouseWheelEvent(UINT direction, LPARAM lParam);

#endif /* MOUSE_H */
