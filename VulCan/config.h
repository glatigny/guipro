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

#ifndef CONFIG_H
#define CONFIG_H

#include <windows.h>

/* ------------------------------------------------------------------------------------------------- */

#define VL_TYPE_NONE				(0x00)
//
#define VL_TYPE_HK					(0x01)
#define VL_TYPE_HK_TXT				L"key"
#define VL_TYPE_MOUSE				(0x02)
#define VL_TYPE_MOUSE_TXT			L"mouse"

#define VL_OPT_VOLUME_STEP			L"step"

#define VL_ACT_NONE					(0x00)
//
#define VL_ACT_VOL_UP				(0x01)
#define VL_TXT_VOL_UP				L"[UP]"
#define VL_ACT_VOL_DOWN				(0x02)
#define VL_TXT_VOL_DOWN				L"[DOWN]"
#define VL_ACT_VOL_MUTE				(0x03)
#define VL_TXT_VOL_MUTE				L"[MUTE]"
//
#define VL_ACT_LAST					(0x04)

#define VL_MOUSE_BTN_LEFT			(0x01)
#define VL_MOUSE_TXT_LEFT			L"left"
#define VL_MOUSE_BTN_RIGHT			(0x02)
#define VL_MOUSE_TXT_RIGHT			L"right"
#define VL_MOUSE_BTN_MIDDLE			(0x03)
#define VL_MOUSE_TXT_MIDDLE			L"middle"
#define VL_MOUSE_BTN_WHEEL_UP		(0x04)
#define VL_MOUSE_TXT_WHEEL_UP		L"wup"
#define VL_MOUSE_BTN_WHEEL_DOWN		(0x05)
#define VL_MOUSE_TXT_WHEEL_DOWN		L"wdown"
#define VL_MOUSE_BTN_WHEEL_LEFT		(0x06)
#define VL_MOUSE_TXT_WHEEL_LEFT		L"wleft"
#define VL_MOUSE_BTN_WHEEL_RIGHT	(0x07)
#define VL_MOUSE_TXT_WHEEL_RIGHT	L"wright"

#define VL_MOUSE_BTN_MAX			(0x08)

/* ------------------------------------------------------------------------------------------------- */

typedef struct _vlConfig {
	UCHAR type;
	UINT key;
	UINT mod;
	UINT action;
} vlConfig;

extern vlConfig g_vlConfig[VL_ACT_LAST];

/* ------------------------------------------------------------------------------------------------- */

int openConfig();
void clearConfig();

int registerConfig(int alert);
int registerHotkeys(LPWSTR p_registerErrors);

#endif /* CONFIG_H */