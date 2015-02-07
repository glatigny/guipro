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

#ifndef HOTKEY_H
#define HOTKEY_H

#include <windows.h>

#define IDH_HOTKEY_VOL_UP		(1000)
#define IDH_HOTKEY_VOL_DOWN		(1001)
#define IDH_HOTKEY_VOL_MUTE		(1002)

bool myRegisterHotKey(UINT key, UINT mod, int id);
bool myUnregisterHotKey(int id);

#endif /* HOTKEY_H */
