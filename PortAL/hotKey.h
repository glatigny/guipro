/*
	PortAL - GUIPro Project ( http://obsidev.github.io/guipro/ )

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

#include <windows.h>
#define IDH_HOTKEY_MENU			(1000)

// Special definition of the function in order to activate Hooking in an executable program
// in general, hooking must be in a library (dll)
__declspec(dllexport) LRESULT CALLBACK hookKeyboard(int nCode, WPARAM wParam, LPARAM lParam);

bool myRegisterHotKey(UINT key, UINT mod, int id, bool overriding);
bool myUnregisterHotKey(int id);

UINT getHotKeyCode(const wchar_t* p_ascii);
UINT getModifier(const wchar_t* p_ascii);

wchar_t* getHotkeyText(UINT mod, UINT key);
wchar_t* getInvModifier(UINT mod);
wchar_t* getInvHotKeyCode(UINT key);

void installHookKeyboard();
void uninstallHookKeyboard();

bool addOverrideKey(DWORD c, int id);
bool delOverrideKey(DWORD c);
bool delOverrideKey(int id);

#endif /* HOTKEY_H */
