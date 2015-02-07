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

#ifndef HOTKEY_H
#define HOTKEY_H

#include "common.h"

#define IDH_HOTKEY_BASE			(1000)

#define IDH_HOTKEY_PLUGIN_BASE	(1100)

bool RegisterHK(UINT key, UINT mod, int id);
bool UnregisterHK(int id);

UINT getModifier(const wchar_t* p_ascii);
wchar_t* getInverseHotKeyCode(UINT key);

UINT getHotKeyCode(const wchar_t* p_ascii);
wchar_t* getInverseModifier(UINT mod);

UINT getMouseBtn( wchar_t* p_ascii );

#endif /* HOTKEY_H */
