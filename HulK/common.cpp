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

#include "common.h"

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
