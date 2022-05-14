/*
	HulK - GUIPro Project ( http://glatigny.github.io/guipro/ )

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
#include "win10.h"
#include "desktopmanager.h"
#include "desktops.h"

#include <thread>

// We need to keep the DesktopManagers but we do not need to keep the ImmersiveShell
IVirtualDesktopManagerInternal* g_DesktopManager = nullptr;
IVirtualDesktopManager* g_DesktopWindowManager = nullptr;

/* ------------------------------------------------------------------------------------------------- */

HRESULT GetVirtualDesktopManager()
{
	if (g_DesktopManager != NULL)
		return NOERROR;

	HRESULT hr = E_FAIL;
	
	// In our case we need to call "CoInitialize" but not "CoInitializeEx" (specially not with COINIT_MULTITHREADED)
	hr = CoInitialize(NULL);
	if (FAILED(hr))
		return hr;

	//
	//
	IServiceProvider* g_immersiveShell = nullptr;
	hr = CoCreateInstance(CLSID_ImmersiveShell, NULL, CLSCTX_LOCAL_SERVER, __uuidof(IServiceProvider), (LPVOID*)&g_immersiveShell);
	
	if (FAILED(hr))
		return hr;

	//
	//
	hr = g_immersiveShell->QueryService(__uuidof(IVirtualDesktopManager), &g_DesktopWindowManager);
	if (FAILED(hr))
	{
		g_immersiveShell->Release();
		g_immersiveShell = NULL;

		g_DesktopManager = NULL;
		return hr;
	}

	//
	//
	UINT16 buildNumber = GetWinBuildNumber();
	switch (buildNumber)
	{
		case 10130:
			hr = g_immersiveShell->QueryService(CLSID_VirtualDesktopAPI_Unknown, UUID_IVirtualDesktopManagerInternal_10130, (void**)&g_DesktopManager);
			break;
		case 10240:
			hr = g_immersiveShell->QueryService(CLSID_VirtualDesktopAPI_Unknown, UUID_IVirtualDesktopManagerInternal_10240, (void**)&g_DesktopManager);
			break;
		case 14393:
		default:
			hr = g_immersiveShell->QueryService(CLSID_VirtualDesktopAPI_Unknown, UUID_IVirtualDesktopManagerInternal_14393, (void**)&g_DesktopManager);
			break;
	}
	/*
	hr = immersiveShell->QueryService(CLSID_VirtualDesktopAPI_Unknown, &g_DesktopManager);
	*/

	if (FAILED(hr))
	{
		g_DesktopManager->Release();
		g_DesktopManager = NULL;

		g_DesktopWindowManager = NULL;
	}

	g_immersiveShell->Release();
	g_immersiveShell = NULL;

	return hr;
}

/* ------------------------------------------------------------------------------------------------- */

HRESULT ReleaseVirtualDesktopManager()
{
	if (g_DesktopManager == NULL)
		return NOERROR;

	g_DesktopWindowManager->Release();
	g_DesktopWindowManager = NULL;

	g_DesktopManager->Release();
	g_DesktopManager = NULL;

	CoUninitialize();
	FlushMemory();

	return NOERROR;
}

/* ------------------------------------------------------------------------------------------------- */

HRESULT getAdjacentVirtualDesktop(GUID *guid, deskDirection dir)
{
	HRESULT hr = GetVirtualDesktopManager();
	if (FAILED(hr))
		return hr;

	IVirtualDesktop *pCurrentDesktop = NULL;
	hr = g_DesktopManager->GetCurrentDesktop(&pCurrentDesktop);

	if (FAILED(hr))
		return hr;

	IVirtualDesktop *pNextDesktop = NULL;
	AdjacentDesktop direction = (dir == deskDirection::left) ? AdjacentDesktop::LeftDirection : AdjacentDesktop::RightDirection;
	hr = g_DesktopManager->GetAdjacentDesktop(pCurrentDesktop, direction, &pNextDesktop);

	pCurrentDesktop->Release();

	if (SUCCEEDED(hr))
	{
		hr = pNextDesktop->GetID(guid);
		pNextDesktop->Release();
		return hr;
	}

	IObjectArray *pObjectArray = NULL;
	hr = g_DesktopManager->GetDesktops(&pObjectArray);

	if (FAILED(hr))
		return hr;

	int nextIndex = 0;
	if (dir == deskDirection::left)
	{
		UINT count = 0;
		hr = pObjectArray->GetCount(&count);
		if (SUCCEEDED(hr))
			nextIndex = count - 1;
	}

	hr = pObjectArray->GetAt(nextIndex, __uuidof(IVirtualDesktop), (void**)&pNextDesktop);
	if (SUCCEEDED(hr))
	{
		hr = pNextDesktop->GetID(guid);
		pNextDesktop->Release();
	}
	pObjectArray->Release();

	return hr;
}

/* ------------------------------------------------------------------------------------------------- */

HRESULT switchVirtualDesktop(deskDirection dir)
{
//*
	GUID guid;
	HRESULT hr = getAdjacentVirtualDesktop(&guid, dir);
	if (FAILED(hr))
		return hr;
	IVirtualDesktop *pNextDesktop = nullptr;
	hr = g_DesktopManager->FindDesktop(&guid, &pNextDesktop);
	if (FAILED(hr))
		return hr;
	hr = g_DesktopManager->SwitchDesktop(pNextDesktop);
	pNextDesktop->Release();
	return hr;
/*/
	HRESULT hr = GetVirtualDesktopManager();
	if (FAILED(hr))
		return hr;

	IVirtualDesktop *pCurrentDesktop = NULL;
	hr = g_DesktopManager->GetCurrentDesktop(&pCurrentDesktop);

	if (FAILED(hr))
		return hr;

	IVirtualDesktop *pNextDesktop = NULL;

	AdjacentDesktop direction = reverse ? AdjacentDesktop::LeftDirection : AdjacentDesktop::RightDirection;
	hr = g_DesktopManager->GetAdjacentDesktop(pCurrentDesktop, direction, &pNextDesktop);

	if (SUCCEEDED(hr))
	{
		hr = g_DesktopManager->SwitchDesktop(pNextDesktop);
		pNextDesktop->Release();
		pCurrentDesktop->Release();

		return hr;
	}

	IObjectArray *pObjectArray = NULL;
	hr = g_DesktopManager->GetDesktops(&pObjectArray);

	if (FAILED(hr))
		return hr;

	int nextIndex = 0;
	if (reverse)
	{
		UINT count;
		hr = pObjectArray->GetCount(&count);

		if (SUCCEEDED(hr))
			nextIndex = count - 1;
	}

	hr = pObjectArray->GetAt(nextIndex, __uuidof(IVirtualDesktop), (void**)&pNextDesktop);
	if (SUCCEEDED(hr))
	{
		hr = g_DesktopManager->SwitchDesktop(pNextDesktop);
		pNextDesktop->Release();
	}
	pObjectArray->Release();

	return hr;
//*/
}

/* ------------------------------------------------------------------------------------------------- */

HRESULT createVirtualDesktop()
{
	HRESULT hr = GetVirtualDesktopManager();
	if (FAILED(hr))
		return hr;

	IVirtualDesktop* newDesktop = NULL;
	hr = g_DesktopManager->CreateDesktopW(&newDesktop);

	return hr;
}

/* ------------------------------------------------------------------------------------------------- */

UINT getVirtualDesktopCount()
{
	HRESULT hr = GetVirtualDesktopManager();
	if (FAILED(hr))
		return 0;

	IObjectArray* desktopList = NULL;
	hr = g_DesktopManager->GetDesktops(&desktopList);

	if (FAILED(hr))
		return 0;

	UINT ret = 0;
	hr = desktopList->GetCount(&ret);
	desktopList->Release();

	if (FAILED(hr))
		return 0;

	return ret;
}

/* ------------------------------------------------------------------------------------------------- */

HRESULT moveWindowToAdjacentDesktop(HWND hwnd, deskDirection dir)
{
	return moveWindowToAdjacentDesktop(hwnd, dir, false);
}

HRESULT moveWindowToAdjacentDesktop(HWND hwnd, deskDirection dir, bool create)
{
	UNREFERENCED_PARAMETER(create);

	GUID guid;
	HRESULT hr = getAdjacentVirtualDesktop(&guid, dir);
	if (SUCCEEDED(hr))
	{
		hr = g_DesktopWindowManager->MoveWindowToDesktop(hwnd, guid);
	}
	return hr;
}

/* ------------------------------------------------------------------------------------------------- */

HRESULT moveWindowToPreviousDesktop(HWND hwnd)
{
	return moveWindowToAdjacentDesktop(hwnd, deskDirection::left, false);
}

/* ------------------------------------------------------------------------------------------------- */

HRESULT moveWindowToNextDesktop(HWND hwnd)
{
	return moveWindowToAdjacentDesktop(hwnd, deskDirection::right, false);
}

/* ------------------------------------------------------------------------------------------------- */

HRESULT moveWindowToDesktopId(HWND hwnd, UINT id)
{
	HRESULT hr = GetVirtualDesktopManager();
	if (FAILED(hr))
		return hr;

	IObjectArray* desktopList = NULL;
	hr = g_DesktopManager->GetDesktops(&desktopList);

	if (FAILED(hr))
		return hr;

	IVirtualDesktop* desktop = NULL;
	hr = desktopList->GetAt(id, __uuidof(IVirtualDesktop), (void**)&desktop);

	if (SUCCEEDED(hr))
	{
		GUID guid;
		hr = desktop->GetID(&guid);
		if (SUCCEEDED(hr))
		{
			hr = g_DesktopWindowManager->MoveWindowToDesktop(hwnd, guid);
		}
		desktop->Release();
	}
	desktopList->Release();

	return hr;
}

/* ------------------------------------------------------------------------------------------------- */