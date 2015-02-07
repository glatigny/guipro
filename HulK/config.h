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

#ifndef CONFIG_H
#define CONFIG_H

#include <windows.h>

#define TYPE_HK					(0x01)
#define TYPE_MOUSE				(0x02)

// Keep it at first element
#define HK_ACT_CONFIG			(0x00)
#define HK_TXT_CONFIG			L"[CONFIG]"
#define HK_ACT_MINIMISE_POINTED	(0x01)
#define HK_TXT_MINIMISE_POINTED	L"[MINIMIZE POINTED]"
#define HK_ACT_MINIMISE_CURRENT	(0x02)
#define HK_TXT_MINIMISE_CURRENT	L"[MINIMIZE CURRENT]"
#define HK_ACT_CLOSE_POINTED	(0x03)
#define HK_TXT_CLOSE_POINTED	L"[CLOSE POINTED]"
#define HK_ACT_CLOSE_CURRENT	(0x04)
#define HK_TXT_CLOSE_CURRENT	L"[CLOSE CURRENT]"
#define HK_ACT_MAXIMISE_POINTED	(0x05)
#define HK_TXT_MAXIMISE_POINTED	L"[MAXIMISE POINTED]"
#define HK_ACT_MAXIMISE_CURRENT	(0x06)
#define HK_TXT_MAXIMISE_CURRENT	L"[MAXIMISE CURRENT]"
#define HK_ACT_ICONIZE_POINTED	(0x07)
#define HK_TXT_ICONIZE_POINTED	L"[ICONIZE POINTED]"
#define HK_ACT_ICONIZE_CURRENT	(0x08)
#define HK_TXT_ICONIZE_CURRENT	L"[ICONIZE CURRENT]"
#define HK_ACT_TRAYNIZE_POINTED	(0x09)
#define HK_TXT_TRAYNIZE_POINTED	L"[TRAYNIZE POINTED]"
#define HK_ACT_TRAYNIZE_CURRENT	(0x0A)
#define HK_TXT_TRAYNIZE_CURRENT	L"[TRAYNIZE CURRENT]"
#define HK_ACT_UNICONIZE		(0x0B)
#define HK_TXT_UNICONIZE		L"[UNICONIZE]"
#define HK_ACT_ALWTOP_POINTED	(0x0C)
#define HK_TXT_ALWTOP_POINTED	L"[ALWAYS ON TOP POINTED]"
#define HK_ACT_ALWTOP_CURRENT	(0x0D)
#define HK_TXT_ALWTOP_CURRENT	L"[ALWAYS ON TOP CURRENT]"

#define HK_ACT_PLUGIN			(0x0E)
#define HK_TXT_PLUGIN			L"[PLUGIN "

#define HK_ACT_QUIT				(0x0F)
#define HK_TXT_QUIT				L"[QUIT]"

// Keep it at last element
#define HK_ACT_NONE				(0x10)

typedef struct _hkConfig {
	UINT key;
	UINT mod;
	UINT action;
	struct _hkConfig* next;
} hkConfig;

#define MOUSE_ACT_DRAG			(0x01)
#define MOUSE_TXT_DRAG			L"[DRAG]"
#define MOUSE_ACT_RESIZE		(0x02)
#define MOUSE_TXT_RESIZE		L"[RESIZE]"
#define MOUSE_ACT_SWITCH		(0x03)
#define MOUSE_TXT_SWITCH		L"[SWITCH]"

#define MOUSE_ACT_NONE			(0x03)

#define MOUSE_BTN_LEFT			(0x01)
#define MOUSE_BTN_RIGHT			(0x02)
#define MOUSE_BTN_MIDDLE		(0x03)
#define MOUSE_BTN_WHEEL			(0x04)

typedef struct _mouseConfig {
	UINT btn;
	UINT mod;
} mouseConfig;

extern mouseConfig g_mouse_options[MOUSE_ACT_NONE];
extern hkConfig* g_hkConfig;

int openConfig();
int registerConfig(int alert);
int registerHotkeys(LPWSTR p_registerErrors);
void clearConfig();

#endif /* CONFIG_H */
