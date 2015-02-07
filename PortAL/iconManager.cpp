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

#include "common.h"
#include "menu.h"

#ifdef ICON_MANAGER

#include "iconManager.h"
#include "main.h"
#include <Windows.h>
#include <ShellAPI.h>

iconsLoader iconManager;

/* ------------------------------------------------------------------------------------------------- */

DWORD WINAPI preloaderIcon(LPVOID param);

/* ------------------------------------------------------------------------------------------------- */
#ifdef ICON_MANAGER_THREAD
HICON* retrieveIconFrom(iconLoader* loader, WPARAM wParam, LPARAM lParam)
{
	/*
	//Maybe we should add some semaphore checks
	DWORD waitFor = WaitForSingleObject( loader->semaphore, 0L );
	*/

	if( loader->thread == NULL)
	{
		if( loader->icon != NULL )
			return &(loader->icon);
		return NULL;
	}

	if( wParam != NULL || lParam != NULL )
	{
		refreshParam* param = (refreshParam*)malloc(sizeof(refreshParam));
		memset(param, 0, sizeof(refreshParam));
		param->lParam = lParam;
		param->wParam = wParam;

		DWORD waitFor = WaitForSingleObject( loader->semaphore, INFINITE );
		if( waitFor == WAIT_OBJECT_0 )
		{
			loader->refreshs->insert( loader->refreshs->end(), param );
		}
		else
		{
			PostMessageW(g_hwndMain, WM_DRAWITEM, lParam, wParam);
		}
	}

	return NULL;
}
#endif
/* ------------------------------------------------------------------------------------------------- */

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
	if( ext != NULL )
		loader->ext = _wcsdup(ext);
	loader->filename = _wcsdup(filename);
#ifndef ICON_MANAGER_THREAD
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
		ExtractIconEx( icoPath, i, NULL, &loader->icon, 1 );
	}
	else
	{
		DWORD attribs = GetFileAttributes(loader->filename);
		if( attribs != (DWORD)-1 )
		{
			SHFILEINFO tSHFileInfo;
			ZeroMemory(&tSHFileInfo, sizeof(tSHFileInfo));

			if( !wcscmp(loader->filename, L"/") || (attribs & FILE_ATTRIBUTE_DIRECTORY) )
			{
				SHGetFileInfo(loader->filename, FILE_ATTRIBUTE_DIRECTORY, &tSHFileInfo, sizeof(tSHFileInfo), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON);
			}
			else
			{
				SHGetFileInfo(loader->filename , 0, &tSHFileInfo, sizeof(tSHFileInfo), SHGFI_ICON | SHGFI_SMALLICON);
			}
			loader->icon = tSHFileInfo.hIcon;
		}
	}
#else
	// Load icon in a thread (still experimental)
	//
	DWORD id;
	loader->icon = NULL;
	loader->refreshs = new refreshParams();
	loader->semaphore = CreateSemaphore(NULL,10,10,NULL);
	loader->thread = CreateThread(
						NULL,
						0,
						preloaderIcon,
						(LPVOID)loader,
						0,
						&id);
#endif
	iconManager.insert( iconManager.end(), loader);

	return (UINT)(iconManager.size()-1);
}

/* ------------------------------------------------------------------------------------------------- */

HICON* getIcon(wchar_t* filename, WPARAM wParam, LPARAM lParam, bool count)
{
	UINT id = preloadIcon(filename);
	if( id >= 0 && id <= iconManager.size() )
	{
		iconLoader* loader = iconManager.at( id );
#ifndef ICON_MANAGER_THREAD
		return &loader->icon;
#else
		return retrieveIconFrom(loader, wParam, lParam);
#endif
	}
	return NULL;
}

/* ------------------------------------------------------------------------------------------------- */

HICON* getIcon(UINT id, WPARAM wParam, LPARAM lParam)
{
	if( id >= 0 && id < iconManager.size() )
	{
		iconLoader* loader = iconManager.at( id );
#ifndef ICON_MANAGER_THREAD
		return &loader->icon;
#else
		return retrieveIconFrom(loader, wParam, lParam);
#endif
	}
	return NULL;
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
#ifdef ICON_MANAGER_THREAD
			if( loader->thread )
			{
				CloseHandle( loader->thread );
			}
			if( loader->semaphore )
			{
				CloseHandle( loader->semaphore );
			}
			if( loader->refreshs )
				delete(loader->refreshs);
#endif
			DestroyIcon( loader->icon );
			loader->icon = NULL;
			free(loader->ext);
			free(loader->filename);

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
#ifdef ICON_MANAGER_THREAD
			if( loader->thread )
			{
				CloseHandle( loader->thread );
			}
			if( loader->semaphore )
			{
				CloseHandle( loader->semaphore );
			}
			if( loader->refreshs )
				delete(loader->refreshs);
#endif
			DestroyIcon( loader->icon );
			loader->icon = NULL;
			free(loader->ext);
			free(loader->filename);


			iconManager.erase( iconManager.begin() + id );
		}
	}
}

/* ------------------------------------------------------------------------------------------------- */
#ifdef ICON_MANAGER_THREAD
DWORD WINAPI preloaderIcon(LPVOID param)
{
	bool processed = false;
	DWORD waitFor;
	SHFILEINFO tSHFileInfo;
	iconLoader* loader = (iconLoader*)param;

	ZeroMemory(&tSHFileInfo, sizeof(tSHFileInfo));

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
			if( !wcscmp(loader->filename, L"/") || (attribs & FILE_ATTRIBUTE_DIRECTORY) )
			{
				SHGetFileInfo(loader->filename, FILE_ATTRIBUTE_DIRECTORY, &tSHFileInfo, sizeof(tSHFileInfo), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON);
			}
			else
			{
				SHGetFileInfo(loader->filename , 0, &tSHFileInfo, sizeof(tSHFileInfo), SHGFI_ICON | SHGFI_SMALLICON);
			}
		}
	}

	while(!processed)
	{
		waitFor = WaitForSingleObject( loader->semaphore, 100L );
		if( waitFor == WAIT_OBJECT_0 )
		{
			// Set the icon
			loader->icon = tSHFileInfo.hIcon;

			processed = true;

			// Call redraw
			//
			for(refreshParams::iterator i = loader->refreshs->begin(); i != loader->refreshs->end(); i++)
			{
				PostMessageW(g_hwndMain, WM_DRAWITEM, (*i)->lParam, (*i)->wParam);
				free( (*i) );
			}
			delete loader->refreshs;
			loader->refreshs = NULL;
			loader->thread = NULL;

			ReleaseSemaphore( loader->semaphore, 1, NULL );
			CloseHandle( loader->semaphore );
			loader->semaphore = NULL;
		}
		else
		{
			if( loader->semaphore == NULL )
				processed = true;
		}
	}
	if( loader->semaphore != NULL )
	{
		ReleaseSemaphore( loader->semaphore, 1, NULL );
		CloseHandle( loader->semaphore );
		loader->semaphore = NULL;
	}
	if( loader->refreshs != NULL )
	{
		for(refreshParams::iterator i = loader->refreshs->begin(); i != loader->refreshs->end(); i++)
		{
			free( (*i) );
		}
		delete loader->refreshs;
		loader->refreshs = NULL;
	}
	loader->thread = NULL;
	return TRUE;
}
#endif

/* ------------------------------------------------------------------------------------------------- */

void unloaderIconManager()
{
	for(iconsLoader::iterator i = iconManager.begin(); i != iconManager.end(); i++)
	{
		DestroyIcon( (*i)->icon );
		(*i)->icon = NULL;
		free((*i)->ext);
		free((*i)->filename);
		free( (*i) );
	}
	iconManager.clear();
}

/* ------------------------------------------------------------------------------------------------- */

#endif