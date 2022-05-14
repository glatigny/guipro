/*
	VulCan - GUIPro Project ( http://glatigny.github.io/guipro/ )

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

/* ------------------------------------------------------------------------------------------------- */

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);

DWORD GetWindowsVersion()
{
	OSVERSIONINFOEX osvi;
	SYSTEM_INFO si;
	PGNSI pGNSI;
	BOOL bOsVersionInfoEx;

	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

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
			return WINVER_7;
		}
		else if( osvi.dwMajorVersion == 5 )
		{
			if( osvi.dwMinorVersion == 0 )
				return WINVER_2000;
			return WINVER_XP;
		}
	}
	
	return WINVER_unknow;
}

/* ------------------------------------------------------------------------------------------------- */

LPSTR UnicodeToAnsi(LPCWSTR s)
{
	if (s == NULL)
		return NULL;

	int cw = lstrlenW(s);
	if (cw == 0)
	{
		CHAR *psz = new CHAR[1]; *psz = '\0';
		return psz;
	}

	int cc = WideCharToMultiByte(CP_ACP, 0, s, cw, NULL, 0, NULL, NULL);

	if (cc == 0)
		return NULL;

	CHAR *psz = new CHAR[cc + 1];
	cc = WideCharToMultiByte(CP_ACP, 0, s, cw, psz, cc, NULL, NULL);
	if (cc == 0) {
		delete[] psz;
		return NULL;
	}

	psz[cc] = '\0';
	return psz;
}

/* ------------------------------------------------------------------------------------------------- */

wchar_t* AnsiToUnicode(const char* cTemp)
{
	if ((cTemp != NULL) && (cTemp[0] != '\0'))
	{
		wchar_t wcTemp[MAX_FILE_LEN];
		MultiByteToWideChar(GetACP(), 0, cTemp, -1, wcTemp, sizeof(wcTemp) / sizeof(wcTemp[0]));
		return _wcsdup(wcTemp);
	}
	return NULL;
}

/* ------------------------------------------------------------------------------------------------- */