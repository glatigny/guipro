/*
	VulCan - GUIPro Project ( http://obsidev.github.io/guipro/ )

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
#include "volume.h"
#include "main.h"
#include "resource.h"

GUID g_guidMyContext = GUID_NULL;
IAudioEndpointVolume *g_pEndptVol = NULL;
IMMDeviceEnumerator *pEnumerator = NULL;
IMMDevice *pDevice = NULL;

float g_volume_step = (float)0.05;

/* ------------------------------------------------------------------------------------------------- */

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
#define SAFE_RELEASE(punk)  if ((punk) != NULL) { (punk)->Release(); (punk) = NULL; }

	SAFE_RELEASE(pEnumerator)
	SAFE_RELEASE(pDevice)
	SAFE_RELEASE(g_pEndptVol)
	CoUninitialize();

#undef SAFE_RELEASE
}

#undef SAFE_RELEASE

/* ------------------------------------------------------------------------------------------------- */

int changeVolumeUp()
{
	if (pDevice == NULL)
		initVolumeInterface();

	float fVolume;
	HRESULT hr;
	hr = g_pEndptVol->GetMasterVolumeLevelScalar(&fVolume);
	if (FAILED(hr))
		return -1;

	fVolume += (float)g_volume_step;
	if (fVolume > 1)
		fVolume = 1;
	hr = g_pEndptVol->SetMasterVolumeLevelScalar(fVolume, &g_guidMyContext);
	if (FAILED(hr))
		return -1;

	return (int)(fVolume * 100);
}

/* ------------------------------------------------------------------------------------------------- */

int changeVolumeDown()
{
	if (pDevice == NULL)
		initVolumeInterface();

	float fVolume;
	HRESULT hr;
	hr = g_pEndptVol->GetMasterVolumeLevelScalar(&fVolume);
	if (FAILED(hr))
		return -1;

	fVolume -= (float)g_volume_step;
	if (fVolume < 0)
		fVolume = 0;
	hr = g_pEndptVol->SetMasterVolumeLevelScalar(fVolume, &g_guidMyContext);
	if (FAILED(hr))
		return -1;

	return (int)(fVolume * 100);
}

/* ------------------------------------------------------------------------------------------------- */

int changeVolumeMute()
{
	if (pDevice == NULL)
		initVolumeInterface();

	BOOL bMute;
	HRESULT hr;

	hr = g_pEndptVol->GetMute(&bMute);
	if (FAILED(hr))
		return -1;

	hr = g_pEndptVol->SetMute(!bMute, &g_guidMyContext);
	if (FAILED(hr))
		return -1;

	return (int)(bMute ? 1 : 0);
}

/* ------------------------------------------------------------------------------------------------- */

int getVolume()
{
	if (pDevice == NULL)
		initVolumeInterface();

	float fVolume;
	BOOL bMute;
	HRESULT hr;

	hr = g_pEndptVol->GetMute(&bMute);
	if (FAILED(hr))
		return -1;

	if (bMute == TRUE)
		return 0;

	hr = g_pEndptVol->GetMasterVolumeLevelScalar(&fVolume);
	if (FAILED(hr))
		return -1;
	return (int)(fVolume * 100);
}

/* ------------------------------------------------------------------------------------------------- */