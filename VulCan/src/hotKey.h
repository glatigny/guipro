/*
	VulCan - GUIPro Project ( http://glatigny.github.io/guipro/ )

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

#ifndef HOTKEY_H
#define HOTKEY_H

#include <windows.h>

#define IDH_HOTKEY_VOL_UP		(1000)
#define IDH_HOTKEY_VOL_DOWN		(1001)
#define IDH_HOTKEY_VOL_MUTE		(1002)

// Special definition of the function in order to activate Hooking in an executable program
// in general, hooking must be in a library (dll)
__declspec(dllexport) LRESULT CALLBACK hookKeyboard(int nCode, WPARAM wParam, LPARAM lParam);

bool myRegisterHotKey(UINT key, UINT mod, int id, bool overriding);
bool myUnregisterHotKey(int id);

UINT getHotKeyCode(const wchar_t* p_ascii);
wchar_t* getInverseHotKeyCode(UINT key);
UINT getModifier(const wchar_t* p_ascii);
wchar_t* getInverseModifier(UINT mod);

void installHookKeyboard();
void uninstallHookKeyboard();

typedef struct _vlHk {
	DWORD key;
	int id;
} vlHk;
vlHk* getOverrideHotkey(DWORD key, bool create = false);

int getEventFromId(int id);

bool addOverrideKey(DWORD c, int id);
bool delOverrideKey(DWORD c);
bool delOverrideKey(int id);

#endif /* HOTKEY_H */
