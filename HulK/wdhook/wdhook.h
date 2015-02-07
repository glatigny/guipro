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

#ifndef WINDOW_HOOK_H
#define WINDOW_HOOK_H

#ifdef _WINDLL
#define DLLDEC __declspec(dllexport)
#else
#define DLLDEC __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

DLLDEC BOOL initPlugin(HWND p_owner, UINT id);
DLLDEC BOOL releasePlugin();
DLLDEC BOOL activatePlugin();
DLLDEC wchar_t* getText();
DLLDEC BOOL setOption(wchar_t* name, wchar_t* value);

LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam);

#ifdef __cplusplus
}
#endif

#endif /* WINDOW_HOOK_H */
