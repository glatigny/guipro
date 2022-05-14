/*
	PortAL - GUIPro Project ( http://glatigny.github.io/guipro/ )

	Author : Glatigny J�r�me <jerome@obsi.dev>

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
#include "config.h"
#include <shlobj.h>
#ifndef LEGACY
#include <VersionHelpers.h>
#endif

extern HWND g_hwndMain;

/* --------------------------------------------------------------------------------------------- */

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);

/**
 * GetWindowsVersion
 */
DWORD GetWindowsVersion()
{
#ifdef LEGACY
	OSVERSIONINFOEX osvi;
	SYSTEM_INFO si;
	PGNSI pGNSI;
	BOOL bOsVersionInfoEx;

	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	// Deprecated, use "VerifyVersionInfo" instead
	if( !(bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*) &osvi) ) )
		return WINVER_unknow;

	pGNSI = (PGNSI) GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo");
	if( pGNSI != NULL )
		pGNSI(&si);
	else 
		GetSystemInfo(&si);

	if( VER_PLATFORM_WIN32_NT == osvi.dwPlatformId )
	{
		if( osvi.dwMajorVersion == 6 )
		{
			if( osvi.dwMinorVersion == 0 )
				return WINVER_VISTA;
			if (osvi.dwMinorVersion == 1)
				return WINVER_7;
			return WINVER_8;
		}
		else if( osvi.dwMajorVersion == 5 )
		{
			if( osvi.dwMinorVersion == 0 )
				return WINVER_2000;
			return WINVER_XP;
		}
	}
	
	return WINVER_unknow;
#else
	/* The older version we want to recognize is Win2k */
	if (!IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN2K), LOBYTE(_WIN32_WINNT_WIN2K), 0))
		return WINVER_unknow;

	if (IsWindows10OrGreater())
		return WINVER_10;
	if (IsWindows8OrGreater())
		return WINVER_8;
	if (IsWindows7OrGreater())
		return WINVER_7;
	if (IsWindowsVistaOrGreater())
		return WINVER_VISTA;
	if (IsWindowsVistaOrGreater())
		return WINVER_VISTA;
	return WINVER_2000;
#endif
}

/* --------------------------------------------------------------------------------------------- */

/**
 * fileExists
 */
bool fileExists(wchar_t* filename)
{
	HANDLE h = CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
	bool ret = ( h != INVALID_HANDLE_VALUE );
	CloseHandle(h);
	return ret;
}

/* --------------------------------------------------------------------------------------------- */

/**
 * specialDirs
 *
 * Dup Or Null with special directories replacement
 *
 * Doc : http://msdn2.microsoft.com/en-us/library/bb762181(VS.85).aspx
 * Doc : http://msdn2.microsoft.com/en-us/library/bb762494(VS.85).aspx
 *
 *   CSIDL_SYSTEM
 *   CSIDL_WINDOWS
 *   CSIDL_SYSTEMX86
 *   CSIDL_MYMUSIC
 *   CSIDL_DESKTOPDIRECTORY
 *   CSIDL_PROGRAM_FILES
 *   CSIDL_MYVIDEO
 *   CSIDL_PERSONAL =~ CSIDL_MYDOCUMENTS
 *   CSIDL_LOCAL_APPDATA
 *
 *   CSIDL_ADMINTOOLS
 *   CSIDL_APPDATA
 *   CSIDL_COMMON_ADMINTOOLS
 *   CSIDL_COMMON_APPDATA
 *   CSIDL_COMMON_DOCUMENTS
 *   CSIDL_COOKIES
 *   CSIDL_HISTORY
 *   CSIDL_INTERNET_CACHE
 *   CSIDL_MYPICTURES
 *   CSIDL_PROGRAM_FILES_COMMON
 *   
 *   CSIDL_COMMON_DESKTOPDIRECTORY
 *   CSIDL_COMMON_PICTURES
 *   CSIDL_COMMON_PROGRAMS
 *   CSIDL_COMMON_VIDEO
 *   CSIDL_DRIVES
 *   CSIDL_DESKTOP
 *   CSIDL_NETHOOD (user nethood)
 *   CSIDL_NETWORK
 *   CSIDL_PRINTERS
 *   CSIDL_RECENT
 */
wchar_t* specialDirs(const wchar_t* cTemp, int mode)
{
	if( cTemp == NULL )
	{
		return NULL;
	}

	wchar_t* cur = wcschr( (wchar_t*)cTemp, L'%');
	if( cur == NULL )
		return _wcsdup(cTemp);

	bool special = false;
	wchar_t lTemp[MAX_FILE_LEN];
	wchar_t realPath[MAX_FILE_LEN];
	wchar_t *lcur = NULL;
	size_t pos = (size_t)(cur - cTemp);

	memset(lTemp, 0, MAX_FILE_LEN);
	wcsncat_s(lTemp, MAX_FILE_LEN, cTemp, pos );

	while( cur != NULL )
	{
		int lenght = 0;
		int CSIDL = 0;

#define D(a,b,c) if( !wcsncmp(cur, L ## a, b) ) { \
	lenght = b; CSIDL = c; }
#define D64(a,b,c,d) if( !wcsncmp(cur, L ## a, b) ) { \
	lenght = b; \
	if( mode == 86 ) CSIDL = c; else { CSIDL = d; special = true; } }

		D("%win%", 5, CSIDL_WINDOWS);
		D64("%programfiles%", 14, CSIDL_PROGRAM_FILESX86, CSIDL_PROGRAM_FILES);
		D("%programfiles86%", 16, CSIDL_PROGRAM_FILESX86);
		D64("%system%", 8, CSIDL_SYSTEMX86, CSIDL_SYSTEM);
		D("%system86%", 10, CSIDL_SYSTEMX86);
		D("%mydocs%", 8, CSIDL_PERSONAL);
		D("%mymusic%", 9, CSIDL_MYMUSIC);
		D("%myvideo%", 8, CSIDL_MYVIDEO);
		D("%desktop%", 9, CSIDL_DESKTOPDIRECTORY);
		D("%appdata%", 9, CSIDL_LOCAL_APPDATA);

#undef D
#undef D64

		if( (lenght > 0) && (CSIDL != 0) )
		{
			wchar_t buff[MAX_PATH];
			HRESULT result = SHGetFolderPathW(NULL, CSIDL, NULL, SHGFP_TYPE_CURRENT, buff);
					
			if( result == S_OK )
			{
				wcscat_s(lTemp, MAX_FILE_LEN, (wchar_t*)buff);
			}

			cur += lenght;
		}
		else if( !wcsncmp(cur, L"%portal%", 8) )
		{
			// portaldir
			wchar_t buff[MAX_PATH];
			int n = GetModuleFileNameW(NULL, buff, MAX_PATH);
			if (n != 0 && n < MAX_PATH) {
				while (n >= 0 && buff[n] != L'\\') n--;
				wcsncat_s(lTemp, MAX_FILE_LEN, buff, n);
				cur += 8;
			}
		}
		else if( !wcsncmp(cur, L"%config%", 8) )
		{
			wchar_t* config = getConfigurationFilename();
			wcscat_s(lTemp, MAX_FILE_LEN, config);
			free(config);
			cur += 8;
		}
		else
		{
			lcur = wcschr(cur + 1, '%');
			wchar_t *lsep = wcschr(cur + 1, ' ');
			if (lcur != NULL && (lsep == NULL || lsep > lcur))
			{
				bool found = false;
				if (g_variables)
				{
					size_t l = lcur - cur - 1;
					for (PortalVariableVector::iterator i = g_variables->begin(); !found && i != g_variables->end(); i++)
					{
						size_t vl = wcslen((*i)->key);
						if (l == vl && !wcsncmp(cur + 1, (*i)->key, l))
						{
							wcscat_s(lTemp, MAX_FILE_LEN, (*i)->value);
							cur = lcur + 1;
							found = true;
						}
					}
				}

				// Skip
				if (!found) {
					wcsncat_s(lTemp, MAX_FILE_LEN, cur, lcur - cur + 1);
					cur = lcur + 1;
				}
			}
		}

		lcur = wcschr( cur+1, '%');
		if( lcur != NULL ) {
			wcsncat_s(lTemp, MAX_FILE_LEN, cur, lcur-cur );
			cur = lcur;
		}
		else
		{
			wcscat_s(lTemp, MAX_FILE_LEN, cur);
			cur = NULL;
		}
	}

	if(lTemp[1] == L':' && (wcsstr(lTemp, L"..\\") || wcsstr(lTemp, L"../")))
	{
		memset(realPath, 0, MAX_FILE_LEN);
		if( GetFullPathNameW(lTemp, MAX_FILE_LEN, realPath, NULL) > 0 )
		{
			wcscpy_s(lTemp, realPath);
		}
	}

	// TODO support "," after extension to test if file exists
	//
	HANDLE h = CreateFileW(lTemp, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
	if( h == INVALID_HANDLE_VALUE && mode == 0 && special)
	{
		CloseHandle(h);
		return specialDirs(cTemp, 86);
	}
	CloseHandle(h);
	return _wcsdup(lTemp);
}

/* --------------------------------------------------------------------------------------------- */

static wchar_t* _getClipboardText(HANDLE clipHwnd)
{
	wchar_t* ret = NULL;
	wchar_t* data = (wchar_t*)GlobalLock(clipHwnd);
	if (data != NULL)
	{
		ret = _wcsdup(data);
	}
	GlobalUnlock(data);
	return ret;
}

static wchar_t* _getClipboardFile(HANDLE clipHwnd, bool multiple)
{
	wchar_t* ret = NULL;
	HDROP drop = (HDROP)GlobalLock(clipHwnd);
	if (drop == NULL)
	{
		return ret;
	}

	int nbFiles = DragQueryFileW(drop, 0xFFFFFFFF, NULL, 0);
	if (nbFiles <= 0)
	{
		GlobalUnlock(drop);
		return ret;
	}
	
	// Handle the files in the clipboard
	// With a special (small) way if we are not in "multiple" mode.
	if (!multiple)
	{
		// We only allocate a small range of memory in that case
		ret = (wchar_t*)malloc(sizeof(wchar_t) * MAX_PATH);
		if (ret != NULL)
		{
			memset(ret, 0, sizeof(wchar_t) * MAX_PATH);
			DragQueryFileW(drop, 0, ret, MAX_PATH);
		}
	}
	else
	{
		// We need to list all files in the clipboard without going outside of the memory.
		ret = (wchar_t*)malloc(sizeof(wchar_t) * CLIPBOARD_BUFFER_LNG);
		if (ret != NULL)
		{
			int pos = 0;
			wchar_t* curr = ret;

			memset(ret, 0, sizeof(wchar_t) * CLIPBOARD_BUFFER_LNG);

			for (int i = 0; i < nbFiles && pos < CLIPBOARD_BUFFER_LNG; i++)
			{
				int n = DragQueryFileW(drop, i, curr, CLIPBOARD_BUFFER_LNG - pos);
				if (n > 0) {
					pos += n;
					curr += n;
				}
				
				// We add a space between the files
				if ((i + 1) < nbFiles && pos < CLIPBOARD_BUFFER_LNG)
				{
					curr[0] = ' ';
					curr++;
				}
			}
		}
	}

	GlobalUnlock(drop);
	return ret;
}

/**
 * getClipboard
 */
wchar_t* getClipboard(bool multiple)
{
	wchar_t* ret = NULL;
	UINT clipboardType = 0;
	if (IsClipboardFormatAvailable(CF_UNICODETEXT))
		clipboardType = CF_UNICODETEXT;
	else if (IsClipboardFormatAvailable(CF_HDROP))
		clipboardType = CF_HDROP;

	if (clipboardType == 0)
		return ret;

	OpenClipboard(g_hwndMain);
	HANDLE clipHwnd = GetClipboardData(clipboardType);
	if (clipHwnd == NULL)
	{
		CloseClipboard();
		return ret;
	}

	if (clipboardType == CF_UNICODETEXT)
	{
		// Text Clipboard
		//
		ret = _getClipboardText(clipHwnd);
#if 0
		wchar_t* data = (wchar_t*)GlobalLock(clipHwnd);
		if (data != NULL)
		{
			ret = _wcsdup(data);
		}
		GlobalUnlock(data);
		CloseClipboard();
		return ret;
#endif
	}
	else if (clipboardType == CF_HDROP)
	{
		// File Clipboard
		//
		ret = _getClipboardFile(clipHwnd, multiple);
#if 0
		// Get the clipboard lock
		HDROP drop = (HDROP)GlobalLock(clipHwnd);
		if (drop == NULL)
		{
			CloseClipboard();
			return ret;
		}

		// Check the number of files in the clipboard
		int nbFiles = DragQueryFileW(drop, 0xFFFFFFFF, NULL, 0);
		if (nbFiles <= 0)
		{
			GlobalUnlock(drop);
			CloseClipboard();
			return ret;
		}

		// Handle the files in the clipboard
		// With a special (small) way if we are not in "multiple" mode.
		if (!multiple)
		{
			// We only allocate a small range of memory in that case
			ret = (wchar_t*)malloc(sizeof(wchar_t) * MAX_PATH);
			memset(ret, 0, sizeof(wchar_t) * MAX_PATH);
			DragQueryFileW(drop, 0, ret, MAX_PATH);
		}
		else
		{
			// We need to list all files in the clipboard without going outside of the memory.
			int pos = 0;
			ret = (wchar_t*)malloc(sizeof(wchar_t) * CLIPBOARD_BUFFER_LNG);

			// Allocation check
			if (ret == NULL)
			{
				GlobalUnlock(drop);
				CloseClipboard();
				return ret;
			}

			wchar_t* curr = ret;
			memset(ret, 0, sizeof(wchar_t) * CLIPBOARD_BUFFER_LNG);

			for (int i = 0; i < nbFiles && pos < CLIPBOARD_BUFFER_LNG; i++)
			{
				int n = DragQueryFileW(drop, i, curr, CLIPBOARD_BUFFER_LNG - pos);
				if (n > 0) {
					pos += n;
					curr += n;
				}
				
				// We add a space between the files
				if ((i + 1) < nbFiles && pos < CLIPBOARD_BUFFER_LNG)
				{
					curr[0] = ' ';
					curr++;
				}
			}
		}
		GlobalUnlock(drop);
#endif
	}

	CloseClipboard();
	return ret;
}

/* --------------------------------------------------------------------------------------------- */