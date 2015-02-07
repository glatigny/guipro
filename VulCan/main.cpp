/*  VulCan - GUIPro Project ( http://guipro.sourceforge.net/ )

    Author : DarkSage  aka  Glatigny Jérôme <darksage@darksage.fr>

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
#include "main.h"
#include "resource.h"
#include "hotkey.h"
#include <crtdbg.h>

/* ------------------------------------------------------------------------------------------------- */

const WCHAR g_szMainWnd[]		= L"_VL90_VulCan_MainWnd_";
const WCHAR g_szWindowName[]	= L"VulCan";

HINSTANCE g_hInst	= NULL;
HWND g_hwndMain		= NULL;
HICON g_hIconTray	= NULL;
DWORD windows_version = 0;

GUID g_guidMyContext = GUID_NULL;
IAudioEndpointVolume *g_pEndptVol = NULL;
IMMDeviceEnumerator *pEnumerator = NULL;
IMMDevice *pDevice = NULL;

/* ------------------------------------------------------------------------------------------------- */

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
#ifndef NDEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
flag |= _CRTDBG_LEAK_CHECK_DF;
_CrtSetDbgFlag(flag);
#endif 
	// Get the Version of Windows
	windows_version = GetWindowsVersion();

	if( windows_version < WINVER_VISTA )
	{
		MessageBox(NULL, TEXT("his program requires Windows Vista or newer."), TEXT("VulCan"), NULL);
		return 1;
	}

	// Keep the instance handle of the application
	g_hInst = hInstance;

	// Creation and registration of the application class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= 0;
	wcex.lpfnWndProc	= (WNDPROC) MainWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= g_hInst;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= g_szMainWnd;
	wcex.hIconSm		= NULL;

	if( !RegisterClassEx(&wcex) )
		return 1;

	if( !CreateWindowEx(WS_EX_TOOLWINDOW, g_szMainWnd, g_szWindowName, 0, 0, 0, 0, 0, NULL, NULL, g_hInst, NULL) )
		return 1;

	// Main loop.
	// Get and send messages for the application.
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

/* ------------------------------------------------------------------------------------------------- */

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static UINT s_uTaskBarCreated = 0;

	switch(uMsg)
	{
	case WM_CREATE:
		s_uTaskBarCreated = RegisterWindowMessage(L"TaskbarCreated");
		g_hwndMain = hwnd;

		// Set icons
		SendMessage(hwnd, WM_SETICON, ICON_BIG, 
			(LONG)(LONG_PTR)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_MAIN_ICON), IMAGE_ICON, 
			GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR));

		SendMessage(hwnd, WM_SETICON, ICON_SMALL, 
			(LONG)(LONG_PTR)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_TRAY_ICON), IMAGE_ICON, 
			GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR));

		myRegisterHotKey(VK_ADD, MOD_CONTROL | MOD_WIN, IDH_HOTKEY_VOL_UP);
		myRegisterHotKey(VK_SUBTRACT, MOD_CONTROL | MOD_WIN, IDH_HOTKEY_VOL_DOWN);
		myRegisterHotKey(VK_NUMPAD0, MOD_CONTROL | MOD_WIN, IDH_HOTKEY_VOL_MUTE);

		// Set the Tray Icon
		ReloadExTrayIcon();

		// Clean the memory in order to have a light application
		FlushMemory();
		break;

	case WM_DESTROY:
			GUIProQuit();
		break;

	case WM_HOTKEY:
		if( wParam == IDH_HOTKEY_VOL_UP )
		{
			changeVolumeUp();
		}
		
		if( wParam == IDH_HOTKEY_VOL_DOWN )
		{
			changeVolumeDown();
		}

		if( wParam == IDH_HOTKEY_VOL_MUTE )
		{
			changeVolumeMute();
		}

		FlushMemory();
		break;

	case WM_MYTRAYMSG:
		// Tray Icon event
		if (wParam == IDI_MAIN_ICON)
		{
			if( (UINT)lParam == WM_RBUTTONUP ) 
			{
				GUIProQuit();
			}
		}
		FlushMemory();
		break;

	default:
		if( uMsg == s_uTaskBarCreated )
		{
			ReloadExTrayIcon();
			FlushMemory();
		}
		else
		{
			return(DefWindowProc(hwnd, uMsg, wParam, lParam));
		}
		break;
	}
	return 0;
}

/* ------------------------------------------------------------------------------------------------- */

void GUIProQuit()
{
	myUnregisterHotKey(IDH_HOTKEY_VOL_UP);
	myUnregisterHotKey(IDH_HOTKEY_VOL_DOWN);

	unloadVolumeInterface();

	DeleteTrayIcon(g_hwndMain, IDI_MAIN_ICON);
	PostQuitMessage(0);
}

/* ------------------------------------------------------------------------------------------------- */

/** Flush Memory
 * Just a little function to call in order to reduce application memory usage.
 * The operating system (microsoft windows) will remove non use memory allocated for the application. 
 */
void FlushMemory()
{
	SetProcessWorkingSetSize(GetCurrentProcess(), 102400, 409600);
}

/* ------------------------------------------------------------------------------------------------- */

#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

BOOL initVolumeInterface()
{
	HRESULT hr = S_OK;
	CoInitialize(NULL);
	hr = CoCreateGuid(&g_guidMyContext);
	if (FAILED(hr)) return 1;

	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
			NULL, CLSCTX_INPROC_SERVER,
			__uuidof(IMMDeviceEnumerator),
			(void**)&pEnumerator);
	if (FAILED(hr)) return 1;

	hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	if (FAILED(hr)) return 1;

    hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&g_pEndptVol);
	if (FAILED(hr)) return 1;

	return 0;
}

/* ------------------------------------------------------------------------------------------------- */

void unloadVolumeInterface()
{
	SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pDevice)
    SAFE_RELEASE(g_pEndptVol)
    CoUninitialize();

	pEnumerator = NULL;
	pDevice = NULL;
	g_pEndptVol = NULL;
}

#undef SAFE_RELEASE

/* ------------------------------------------------------------------------------------------------- */

BOOL changeVolumeUp()
{
	if( pDevice == NULL )
		initVolumeInterface();

	float fVolume;
	HRESULT hr;
	hr = g_pEndptVol->GetMasterVolumeLevelScalar(&fVolume);
	if( FAILED(hr) )
		return FALSE;

	fVolume += (float)0.05;
	if( fVolume > 1 )
		fVolume = 1;
	hr = g_pEndptVol->SetMasterVolumeLevelScalar(fVolume, &g_guidMyContext);
	if( FAILED(hr) )
		return FALSE;

	return TRUE;
}

/* ------------------------------------------------------------------------------------------------- */

BOOL changeVolumeDown()
{
	if( pDevice == NULL )
		initVolumeInterface();

	float fVolume;
	HRESULT hr;
	hr = g_pEndptVol->GetMasterVolumeLevelScalar(&fVolume);
	if( FAILED(hr) )
		return FALSE;

	fVolume -= (float)0.05;
	if( fVolume < 0 )
		fVolume = 0;
	hr = g_pEndptVol->SetMasterVolumeLevelScalar(fVolume, &g_guidMyContext);
	if( FAILED(hr) )
		return FALSE;

	return TRUE;
}

/* ------------------------------------------------------------------------------------------------- */

BOOL changeVolumeMute()
{
	if( pDevice == NULL )
		initVolumeInterface();

	BOOL bMute;
	HRESULT hr;
	
	hr = g_pEndptVol->GetMute( &bMute );
	if( FAILED(hr) )
		return FALSE;

	hr = g_pEndptVol->SetMute( !bMute, &g_guidMyContext);
	if( FAILED(hr) )
		return FALSE;

	return TRUE;
}

/* ------------------------------------------------------------------------------------------------- */
