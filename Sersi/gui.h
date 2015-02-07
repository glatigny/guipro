/*  Sersi - GUIPro Project ( http://guipro.sourceforge.net/ )

    Author : DarkSage  aka  Glatigny Jérôme <darksage@darksage.info>

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

#ifndef GUI_H
#define GUI_H

#ifdef WINDOWMODE

#include <windows.h>

# pragma warning(disable:4996)

# include <commctrl.h>

# define ID_TREEVIEW								0x1
# define SET_RESSOURCE_ICON(icon_size,icon_index)	SendMessage(hwnd, WM_SETICON, icon_size, (LONG)(LONG_PTR)LoadImage(g_hInst, MAKEINTRESOURCE(icon_index), IMAGE_ICON, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR))

# define IDM_MENU_NEW_ELEM							305
# define IDM_MENU_REM_ELEM							306
# define IDM_MENU_SET_PROGRAM						307
# define IDM_MENU_SET_DIRECTORY						308
# define IDM_MENU_REM_DIRECTORY						309
# define IDM_MENU_SET_PARAMETER						310
# define IDM_MENU_REM_PARAMETER						311
# define IDM_MENU_SET_AUTORESTART					312
# define IDM_MENU_REM_AUTORESTART					313
# define IDM_MENU_SET_DAILYRESTART					314
# define IDM_MENU_REM_DAILYRESTART					315

extern HINSTANCE g_hInst;
extern HWND g_hwndMain;

struct tvItem
{
	wchar_t* name;
	LPARAM param;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void createWindow();
void TreeViewMenu(HTREEITEM item, UINT flags);
void insertItem(const wchar_t* itemName, const wchar_t* progname, const wchar_t* dirname, const wchar_t* params, const DWORD autoRestart, const DWORD dailyRestart);
struct tvItem* getTreeViewItem();
HTREEITEM selectTreeViewItem(wchar_t* progname, LPARAM type);
int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
wchar_t* BrowseForDirectory(wchar_t* directory);
wchar_t* InputText(wchar_t* text);
wchar_t* BrowseForFile(wchar_t* filename);
void updateWindow();


#endif /* WINDOWMODE */

#endif /* GUI_H */
