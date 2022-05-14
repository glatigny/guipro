/*
	PortAL - GUIPro Project ( http://glatigny.github.io/guipro/ )

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

#include "common.h"

#ifdef ICON_MANAGER

#include "config.h"
#include "menu.h"
#include "iconManager.h"
#include "trayIcon.h"
#include "main.h"
#include <Windows.h>
#include <ShellAPI.h>

iconsLoader iconManager;

/* ------------------------------------------------------------------------------------------------- */

DWORD WINAPI preloaderIcon(LPVOID param);

/* ------------------------------------------------------------------------------------------------- */

HICON* retrieveIconFrom(iconLoader* loader, LPDRAWITEMSTRUCT ptDrawItem)
{
	if (loader->thread == NULL || loader->thread->thread == NULL)
	{
		if (loader->icon != NULL)
			return &(loader->icon);
		return NULL;
	}

	if (ptDrawItem == NULL)
		return NULL;

	HANDLE mutex = loader->thread->mutex;
	DWORD waitFor = WaitForSingleObject(mutex, 10L);
	if( waitFor == WAIT_OBJECT_0 )
	{
		if (loader->thread->thread == NULL || loader->icon != NULL)
		{
			ReleaseMutex(mutex);
			if (loader->icon != NULL)
				return &(loader->icon);
			return NULL;
		}

		loader->thread->items->insert(loader->thread->items->end(), (PortalMenuItem*)ptDrawItem->itemData);
		ReleaseMutex(mutex);
	}

	return NULL;
}

/* ------------------------------------------------------------------------------------------------- */

static void _loadIcon(iconLoader* loader)
{
	wchar_t* pos = 0;
	if (loader->ext == NULL && (wcsstr(loader->filename, L".exe") != NULL || wcsstr(loader->filename, L".dll") != NULL))
	{
		pos = wcschr(loader->filename, L',');
	}
	if (pos > 0)
	{
		wchar_t icoPath[512];
		int i = 0;
		wcsncpy_s(icoPath, 512, loader->filename, size_t(pos - loader->filename));
		i = _wtoi(&pos[1]);
		ExtractIconEx(icoPath, i, NULL, &loader->icon, 1);
	}
	else
	{
		DWORD attribs = GetFileAttributes(loader->filename);
		if (attribs != (DWORD)-1)
		{
			SHFILEINFO tSHFileInfo;
			ZeroMemory(&tSHFileInfo, sizeof(tSHFileInfo));

			if (!wcscmp(loader->filename, L"/") || (attribs & FILE_ATTRIBUTE_DIRECTORY))
			{
				SHGetFileInfo(loader->filename, FILE_ATTRIBUTE_DIRECTORY, &tSHFileInfo, sizeof(tSHFileInfo), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON);
			}
			else
			{
				SHGetFileInfo(loader->filename, 0, &tSHFileInfo, sizeof(tSHFileInfo), SHGFI_ICON | SHGFI_SMALLICON);
			}
			loader->icon = tSHFileInfo.hIcon;
		}
	}
}

UINT preloadIcon(wchar_t* filename)
{
	if( filename == NULL || filename[0] == L'\0' )
		return PORTAL_ICON_RESOURCE;

	wchar_t* ext = wcsrchr(filename, L'.');
	UINT j = 0;

	if( ext > 0 )
	{
		// Check if it is a : exe / ico / a dll with ","
		//
		if( wcsncmp(ext, L".exe", 4) == 0 || wcsncmp(ext, L".dll,", 5) == 0 || wcsncmp(ext, L".ico", 4) == 0 )
		{
			ext = NULL;
		}
	}

	if( ext == NULL )
	{
		// Return directory icon.. Or maybe another
		//
		for(iconsLoader::iterator i = iconManager.begin(); i != iconManager.end(); i++)
		{
			if( !wcscmp( (*i)->filename, filename ) )
			{
				(*i)->counter++;
				return j;
			}
			j++;
		}
	}
	else
	{
		// Check if icon already loaded (if the extension is known).
		//
		for(iconsLoader::iterator i = iconManager.begin(); i != iconManager.end(); i++)
		{
			if( (*i)->ext != NULL && !wcscmp( (*i)->ext, ext ) )
			{
				(*i)->counter++;
				return j;
			}
			j++;
		}
	}

	iconLoader* loader = (iconLoader*)malloc(sizeof(iconLoader));
	memset(loader, 0, sizeof(iconLoader));
	loader->counter = 1;
	loader->ext = NULL;
	loader->thread = NULL;
	if( ext != NULL )
		loader->ext = _wcsdup(ext);
	loader->filename = _wcsdup(filename);

	if (!g_portal->iconloaderThread) {
		_loadIcon(loader);
	} else {
		DWORD id;
		loader->icon = NULL;
		loader->thread = (iconLoaderThread*)calloc(1, sizeof(iconLoaderThread));
		loader->thread->mutex = CreateMutex(NULL, FALSE, NULL);
		loader->thread->items = new std::vector<PortalMenuItem*>();
		loader->thread->items->clear();
		// loader->thread->semaphore = CreateSemaphore(NULL, 10, 10, NULL);
		loader->thread->thread = CreateThread(
			NULL,
			0,
			preloaderIcon,
			(LPVOID)loader,
			0,
			&id);
	}
	iconManager.insert(iconManager.end(), loader);
	return (UINT)(iconManager.size()-1);
}

/* ------------------------------------------------------------------------------------------------- */

HICON* getIcon(wchar_t* filename)
{
	UINT id = preloadIcon(filename);
	if( id >= 0 && id <= iconManager.size() )
	{
		iconLoader* loader = iconManager.at( id );
		return (loader->thread == NULL) ? &loader->icon : retrieveIconFrom(loader, NULL);
	}
	return NULL;
}

/* ------------------------------------------------------------------------------------------------- */

HICON* getIcon(UINT id, LPDRAWITEMSTRUCT ptDrawItem)
{
	if( id >= 0 && id < iconManager.size() )
	{
		iconLoader* loader = iconManager.at( id );
		return (loader->thread == NULL) ? &loader->icon : retrieveIconFrom(loader, ptDrawItem);
	}
	return NULL;
}

/* ------------------------------------------------------------------------------------------------- */

static void _unloadIconLoader(iconLoader *loader)
{
	if (loader == NULL)
		return;

	if (loader->thread != NULL)
	{
		if (loader->thread->thread != NULL) {
			TerminateThread(loader->thread->thread, 0);
			CloseHandle(loader->thread->thread);
		}
		if (loader->thread->mutex != NULL)
			CloseHandle(loader->thread->mutex);
		if (loader->thread->items != NULL)
		{
			loader->thread->items->clear();
			delete(loader->thread->items);
			loader->thread->items = NULL;
		}
	}
	DestroyIcon(loader->icon);
	loader->icon = NULL;
	free(loader->ext);
	free(loader->filename);
}

/* ------------------------------------------------------------------------------------------------- */

void unloadIcon(wchar_t* filename)
{
	iconLoader* loader = NULL;
	iconsLoader::iterator it;
	for(it = iconManager.begin(); it != iconManager.end(); it++)
	{
		if( wcscmp((*it)->filename, filename) == 0 )
		{
			loader = (*it);
			break;
		}
	}
	if( loader != NULL )
	{
		loader->counter--;
		if( loader->counter == 0 )
		{
			_unloadIconLoader(loader);
			free(loader);
			iconManager.erase(it);
		}
	}
}

/* ------------------------------------------------------------------------------------------------- */

void unloadIcon(UINT id)
{
	if( id < 0 || id >= iconManager.size() )
		return;

	iconLoader* loader = iconManager.at(id);
	if( loader != NULL )
	{
		loader->counter--;
		if( loader->counter == 0 )
		{
			_unloadIconLoader(loader);
			free(loader);

			iconManager.erase( iconManager.begin() + id );
		}
	}
}

/* ------------------------------------------------------------------------------------------------- */

static bool _shGetFileInfo(iconLoader* loader, DWORD attribs, SHFILEINFO* ptSHFileInfo)
{
	DWORD_PTR dwptr;
	if (!wcscmp(loader->filename, L"/") || (attribs & FILE_ATTRIBUTE_DIRECTORY))
	{
		dwptr = SHGetFileInfo(loader->filename, FILE_ATTRIBUTE_DIRECTORY, ptSHFileInfo,
			sizeof(*ptSHFileInfo), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON);
	}
	else
	{
		dwptr = SHGetFileInfo(loader->filename, FILE_ATTRIBUTE_NORMAL, ptSHFileInfo,
			sizeof(*ptSHFileInfo), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON);
	}
	if (FAILED(dwptr)) {
		return false;
	}
	return true;
}

DWORD WINAPI preloaderIcon(LPVOID param)
{
	bool processed = false;
	DWORD waitFor;
	SHFILEINFO tSHFileInfo = { 0 };
	iconLoader* loader = (iconLoader*)param;

	CoInitializeEx(NULL, COINITBASE_MULTITHREADED);

	// Load Icon
	//
	wchar_t* pos = 0;
	if( loader->ext == NULL && (wcsstr(loader->filename, L".exe") != NULL || wcsstr(loader->filename, L".dll") != NULL) )
	{
		pos = wcschr(loader->filename, L',');
	}
	if( pos > 0 )
	{
		wchar_t icoPath[512];
		int i = 0;
		wcsncpy_s( icoPath, 512, loader->filename, size_t(pos - loader->filename) );
		i = _wtoi( &pos[1] );
		ExtractIconEx( icoPath, i, NULL, &tSHFileInfo.hIcon, 1 );
	}
	else
	{
		DWORD attribs = GetFileAttributes(loader->filename);
		if( attribs == (DWORD)-1 )
		{
			processed = true;
		}
		else
		{
			/* We need to perform a loop with a sleep if we did not retrieve the hIcon of an element.
			 * The values are just something which is working on the test machine.
			 * The reason is that the first times the function is called, there is no hIcon, but just a few miliseconds after that, it's working.
			 * More strange, that kind of issue doesn't arrived when we're in the main thread ; even with CoInitializeEx. */
			int cpt = 0;
			while (cpt++ < 30) {
				_shGetFileInfo(loader, attribs, &tSHFileInfo);
				if (tSHFileInfo.hIcon == NULL) {
					Sleep(5);
				} else {
					break;
				}
			}
		}
	}

	while(!processed)
	{
		waitFor = WaitForSingleObject( loader->thread->mutex, 100L );
		if( waitFor == WAIT_OBJECT_0 )
		{
			// Set the icon
			if (tSHFileInfo.hIcon != NULL) {
				loader->icon = CopyIcon(tSHFileInfo.hIcon);
				DestroyIcon(tSHFileInfo.hIcon);
			} else {
				loader->icon = NULL;
			}

			processed = true;

			// Call redraw using a special internal message since it's not possible to do it otherwise.
			for (std::vector<PortalMenuItem*>::iterator i = loader->thread->items->begin(); i != loader->thread->items->end(); i++)
			{
				PostMessageW(g_hwndMain, WM_MYREDRAWITEM, NULL, (LPARAM)(*i));
			}
			loader->thread->items->clear();
			delete(loader->thread->items);
			loader->thread->items = NULL;
			loader->thread->thread = NULL;

			// We finished our modification in the object, releasing the mutex
			// ReleaseMutex(loader->thread->mutex);
			CloseHandle(loader->thread->mutex);
			loader->thread->mutex = NULL;

			// Force the redraw of the entire menu
			PostMessageW(g_hwndMain, WM_MYREDRAWMENU, 0, 0);
		}
	}

	CoUninitialize();
	return TRUE;
}

/* ------------------------------------------------------------------------------------------------- */

void unloaderIconManager()
{
	for(iconsLoader::iterator i = iconManager.begin(); i != iconManager.end(); i++)
	{
		_unloadIconLoader((*i));
		free( (*i) );
	}
	iconManager.clear();
}

/* ------------------------------------------------------------------------------------------------- */

#endif