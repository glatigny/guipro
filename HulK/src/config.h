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

#ifndef CONFIG_H
#define CONFIG_H

#include <windows.h>
#include <vector>

//
//
//
#define TYPE_HK					(0x01)
#define TYPE_MOUSE				(0x02)

//
//
//
#define HK_TARGET_NONE			(0x00)
#define HK_TARGET_HWND_CURRENT	(0x01)
#define HK_TARGET_HWND_POINTED	(0x02)

//
//
//
#define HK_ACT_CONFIG			(0x00)	// Keep it at first element
#define HK_CFG_TXT_CONFIG		L"config"
//
#define HK_ACT_QUIT				(0x01)
#define HK_CFG_TXT_QUIT			L"quit"
#define HK_ACT_MINIMIZE			(0x02)
#define HK_CFG_TXT_MINIMIZE		L"minimize"
#define HK_ACT_CLOSE			(0x03)
#define HK_CFG_TXT_CLOSE		L"close"
#define HK_ACT_MAXIMIZE			(0x04)
#define HK_CFG_TXT_MAXIMIZE		L"maximize"
#define HK_ACT_ICONIZE			(0x05)
#define HK_CFG_TXT_ICONIZE		L"iconize"
#define HK_ACT_TRAYNIZE			(0x06)
#define HK_CFG_TXT_TRAYNIZE		L"traynize"
#define HK_ACT_UNICONIZE		(0x07)
#define HK_CFG_TXT_UNICONIZE	L"uniconize"
#define HK_ACT_ALWTOP			(0x08)
#define HK_CFG_TXT_ALWTOP		L"ontop"
#define HK_ACT_BORDERLESS		(0x09)
#define HK_CFG_TXT_BORDERLESS	L"borderless"
//
#define HK_ACT_NONE				(0x09)	// Keep it at last element

typedef struct _hkConfig {
	UINT action;
	UINT8 plugin;
	UINT key;
	UINT mod;
	UINT8 target;
	// struct _hkConfig* next;
} hkConfig;
typedef std::vector<hkConfig*> hkConfigVector;

//
//
//
#define MOUSE_ACT_DRAG			(0x01)
#define MOUSE_ACT_RESIZE		(0x02)
#define MOUSE_ACT_SWITCH		(0x03)

#define MOUSE_ACT_NONE			(0x04)

#define MOUSE_BTN_LEFT			(0x01)
#define MOUSE_BTN_RIGHT			(0x02)
#define MOUSE_BTN_MIDDLE		(0x03)
#define MOUSE_BTN_WHEEL			(0x04)

typedef struct _mouseConfig {
	UINT8 btn;
	UINT16 mod;
} mouseConfig;
typedef std::vector<mouseConfig*> mouseConfigVector;

//
//
//
typedef struct _hulkConfig {
	hkConfigVector		hotkeys;
	mouseConfig			mouses[MOUSE_ACT_NONE];
} HulkConfig;

extern HulkConfig* g_hulk;

//
//
//
int openConfig();
int registerConfig(int alert);
int registerHotkeys(LPWSTR p_registerErrors);
void clearConfig();
wchar_t* getConfigurationFilename();

#endif /* CONFIG_H */
