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

#ifndef ICONIZE_H
#define ICONIZE_H

#define ICONIZE_OPT_SYSTRAY		(1)
#define ICONIZE_OPT_INTERNAL	(2)

#define ICONIZE_MAX_APPS		256

typedef struct trayIconElem {
	UINT id;
	HWND hwnd;
	HICON icon;
	UINT option;
	wchar_t* name;
	struct trayIconElem* next;
} TrayIconElem;

extern TrayIconElem* g_iconizeTrays;

void Iconize(HWND l_hwnd, UINT option);
void DeleteIconize(TrayIconElem *elem, TrayIconElem *previous, WPARAM wParam);
void RemoveIconize(UINT id, WPARAM wParam);
void CheckAllIconize();
void UnIconize(UINT id);
void CloseIconize(UINT id);
void UnIconizeLast();
void UnIconizeAll();

#endif /* ICONIZE_H */
