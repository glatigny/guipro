/*
	HulK - GUIPro Project ( http://glatigny.github.io/guipro/ )

	Author : Glatigny Jérôme <jerome@darksage.fr>

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

LPSTR UnicodeToAnsi(LPCWSTR s)
{
	if (s==NULL)
		return NULL;
	
	int cw = lstrlenW(s);
	if( cw == 0 )
	{
		CHAR *psz=new CHAR[1];*psz='\0';
		return psz;
	}

	int cc = WideCharToMultiByte(CP_ACP,0,s,cw,NULL,0,NULL,NULL);
	
	if( cc == 0 )
		return NULL;

	CHAR *psz = new CHAR[cc+1];
	cc = WideCharToMultiByte(CP_ACP,0,s,cw,psz,cc,NULL,NULL);
	if( cc == 0 ) {
		delete[] psz;
		return NULL;
	}
	
	psz[cc]='\0';
	return psz;
}

/* ------------------------------------------------------------------------------------------------- */

wchar_t* AnsiToUnicode(const char* cTemp)
{
	if( (cTemp != NULL) && (cTemp[0] != '\0') )
	{
		wchar_t wcTemp[MAX_FILE_LEN];
		MultiByteToWideChar( GetACP(), 0, cTemp, -1, wcTemp, sizeof(wcTemp)/sizeof(wcTemp[0]) );
		return _wcsdup(wcTemp);
	}
	else
	{
		return NULL;
	}
}

/* ------------------------------------------------------------------------------------------------- */

typedef struct _DllVersionInfo {
	DWORD cbSize;
	DWORD dwMajorVersion;
	DWORD dwMinorVersion;
	DWORD dwBuildNumber;
	DWORD dwPlatformID;
} DLLVERSIONINFO;

#ifndef DLLGETVERSIONPROC
typedef int (FAR WINAPI *DLLGETVERSIONPROC) (DLLVERSIONINFO *);
#endif

DWORD GetDllVersion(LPCTSTR lpszDllName)
{
	/* For security purposes, LoadLibrary should be provided with a 
	   fully-qualified path to the DLL. The lpszDllName variable should be
	   tested to ensure that it is a fully qualified path before it is used. */

	DWORD dwVersion = 0;
	HINSTANCE hinstDll = LoadLibrary(lpszDllName);

	if(hinstDll)
	{
		/* Because some DLLs might not implement this function, you
		   must test for it explicitly. Depending on the particular 
		   DLL, the lack of a DllGetVersion function can be a useful
		   indicator of the version. */

		DLLGETVERSIONPROC pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hinstDll, "DllGetVersion");

		if(pDllGetVersion)
		{
			DLLVERSIONINFO dvi;

			ZeroMemory(&dvi, sizeof(dvi));
			dvi.cbSize = sizeof(dvi);

			HRESULT hr = (*pDllGetVersion)(&dvi);

			if(SUCCEEDED(hr))
			{
				dwVersion = WINVERSION(dvi.dwMajorVersion, dvi.dwMinorVersion);
			}
		}

		FreeLibrary(hinstDll);
	}
	return dwVersion;
}

/* ------------------------------------------------------------------------------------------------- */

bool fileExists(wchar_t* filename)
{
	HANDLE h = CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
	bool ret = (h != INVALID_HANDLE_VALUE);
	CloseHandle(h);
	return ret;
}

/* ------------------------------------------------------------------------------------------------- */

// Dup Or Null with special directories replacement
wchar_t* specialDirs(const wchar_t* cTemp, int mode)
{
	if (cTemp == NULL)
	{
		return NULL;
	}

	wchar_t* cur = wcschr((wchar_t*)cTemp, L'%');
	if (cur == NULL)
		return _wcsdup(cTemp);

	bool special = false;
	wchar_t lTemp[MAX_FILE_LEN];
	wchar_t realPath[MAX_FILE_LEN];
	wchar_t *lcur = NULL;
	size_t pos = (size_t)(cur - cTemp);

	memset(lTemp, 0, MAX_FILE_LEN);
	wcsncat_s(lTemp, MAX_FILE_LEN, cTemp, pos);

	while (cur != NULL)
	{
		int lenght = 0;
		int CSIDL = 0;

#define D(a,b,c) if( !wcsncmp(cur, L ## a, b) ) { \
	lenght = b; CSIDL = c; }
#define D64(a,b,c,d) if( !wcsncmp(cur, L ## a, b) ) { \
	lenght = b; \
	if( mode == 86 ) CSIDL = c; else { CSIDL = d; special = true; } }

		/*
		http://msdn2.microsoft.com/en-us/library/bb762181(VS.85).aspx

		CSIDL_SYSTEM
		CSIDL_WINDOWS
		CSIDL_SYSTEMX86
		CSIDL_MYMUSIC
		CSIDL_DESKTOPDIRECTORY
		CSIDL_PROGRAM_FILES
		CSIDL_MYVIDEO
		CSIDL_PERSONAL =~ CSIDL_MYDOCUMENTS
		CSIDL_LOCAL_APPDATA

		CSIDL_ADMINTOOLS
		CSIDL_APPDATA
		CSIDL_COMMON_ADMINTOOLS
		CSIDL_COMMON_APPDATA
		CSIDL_COMMON_DOCUMENTS
		CSIDL_COOKIES
		CSIDL_HISTORY
		CSIDL_INTERNET_CACHE
		CSIDL_MYPICTURES
		CSIDL_PROGRAM_FILES_COMMON

		CSIDL_COMMON_DESKTOPDIRECTORY
		CSIDL_COMMON_PICTURES
		CSIDL_COMMON_PROGRAMS
		CSIDL_COMMON_VIDEO
		CSIDL_DRIVES
		CSIDL_DESKTOP
		CSIDL_NETHOOD (user nethood)
		CSIDL_NETWORK
		CSIDL_PRINTERS
		CSIDL_RECENT

		http://msdn2.microsoft.com/en-us/library/bb762494(VS.85).aspx
		*/

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

		if ((lenght > 0) && (CSIDL != 0))
		{
			wchar_t buff[MAX_PATH];
			DWORD dwSize = sizeof(buff);
			HRESULT result = SHGetFolderPathW(NULL, CSIDL, NULL, SHGFP_TYPE_CURRENT, buff);

			if (result == S_OK)
			{
				wcscat_s(lTemp, MAX_FILE_LEN, (wchar_t*)buff);
			}

			cur += lenght;
		}
		else if (!wcsncmp(cur, L"%hulk%", 6))
		{
			// portaldir
			wchar_t buff[MAX_PATH];
			int n = GetModuleFileNameW(NULL, buff, MAX_PATH);
			if (n != 0 && n < MAX_PATH) {
				while (n >= 0 && buff[n] != L'\\') n--;
				wcsncat_s(lTemp, MAX_FILE_LEN, buff, n);
				cur += 6;
			}
		}
		else if (!wcsncmp(cur, L"%config%", 8))
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
				wcsncat_s(lTemp, MAX_FILE_LEN, cur, lcur - cur + 1);
				cur = lcur + 1;
			}
		}

		lcur = wcschr(cur + 1, '%');
		if (lcur != NULL) {
			wcsncat_s(lTemp, MAX_FILE_LEN, cur, lcur - cur);
			cur = lcur;
		}
		else
		{
			wcscat_s(lTemp, MAX_FILE_LEN, cur);
			cur = NULL;
		}
	}

	if (lTemp[1] == L':' && (wcsstr(lTemp, L"..\\") || wcsstr(lTemp, L"../")))
	{
		memset(realPath, 0, MAX_FILE_LEN);
		if (GetFullPathNameW(lTemp, MAX_FILE_LEN, realPath, NULL) > 0)
		{
			wcscpy_s(lTemp, realPath);
		}
	}

	// TODO support "," after extension to test if file exists
	//
	HANDLE h = CreateFileW(lTemp, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
	if (h == INVALID_HANDLE_VALUE && mode == 0 && special)
	{
		CloseHandle(h);
		return specialDirs(cTemp, 86);
	}
	CloseHandle(h);
	return _wcsdup(lTemp);
}

/* ------------------------------------------------------------------------------------------------- */
