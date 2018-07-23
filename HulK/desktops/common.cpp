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
#include <VersionHelpers.h>

/* ------------------------------------------------------------------------------------------------- */

UINT16 GetWinBuildNumber()
{
	UINT16 buildNumbers[] = { 10130, 10240, 14393 };
	OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0,{ 0 }, 0, 0 };
	ULONGLONG mask = VerSetConditionMask(0, VER_BUILDNUMBER, VER_EQUAL);

	for (size_t i = 0; i < sizeof(buildNumbers) / sizeof(buildNumbers[0]); i++)
	{
		osvi.dwBuildNumber = buildNumbers[i];
		if (VerifyVersionInfoW(&osvi, VER_BUILDNUMBER, mask) != FALSE)
		{
			return buildNumbers[i];
		}
	}

	return 0;
}

/* ------------------------------------------------------------------------------------------------- */

typedef LONG NTSTATUS, *PNTSTATUS;
#define STATUS_SUCCESS (0x00000000)

typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

RTL_OSVERSIONINFOW GetRealOSVersion() {
	HMODULE hMod = GetModuleHandleW(L"ntdll.dll");
	if (hMod) {
		RtlGetVersionPtr fxPtr = (RtlGetVersionPtr)GetProcAddress(hMod, "RtlGetVersion");
		if (fxPtr != nullptr) {
			RTL_OSVERSIONINFOW rovi = { 0 };
			rovi.dwOSVersionInfoSize = sizeof(rovi);
			if (STATUS_SUCCESS == fxPtr(&rovi)) {
				return rovi;
			}
		}
	}
	RTL_OSVERSIONINFOW rovi = { 0 };
	return rovi;
}

bool isWindows10()
{
	RTL_OSVERSIONINFOW osv = GetRealOSVersion();
	return (osv.dwMajorVersion >= 10);
}

/* ------------------------------------------------------------------------------------------------- */

void FlushMemory()
{
	SetProcessWorkingSetSize(GetCurrentProcess(), 102400, 614400);
}

/* ------------------------------------------------------------------------------------------------- */