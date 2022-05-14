/*
	HulK - GUIPro Project ( http://glatigny.github.io/guipro/ )

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

#pragma once

// http://www.cyberforum.ru/blogs/105416/blog3605.html

#include <objbase.h>
#include <ObjectArray.h>
#include <windows.h>
#include <shobjidl.h>

//
const CLSID CLSID_ImmersiveShell = {
	0xC2F03A33, 0x21F5, 0x47FA, 0xB4, 0xBB, 0x15, 0x63, 0x62, 0xA2, 0xF2, 0x39
};
const CLSID CLSID_VirtualDesktopAPI_Unknown = {
	0xC5E0CDCA, 0x7B6E, 0x41B2, 0x9F, 0xC4, 0xD9, 0x39, 0x75, 0xCC, 0x46, 0x7B
};
const IID IID_IVirtualDesktopManagerInternal = {
	0xEF9F1A6C, 0xD3CC, 0x4358, 0xB7, 0x12, 0xF8, 0x4B, 0x63, 0x5B, 0xEB, 0xE7
};

//
struct IApplicationView : public IUnknown {};

//
EXTERN_C const IID IID_IVirtualDesktop;

MIDL_INTERFACE("FF72FFDD-BE7E-43FC-9C03-AD81681E88E4")
IVirtualDesktop : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE IsViewVisible(IApplicationView *pView, int *pfVisible) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetID(GUID *pGuid) = 0;
};

//
enum AdjacentDesktop { LeftDirection = 3, RightDirection = 4 };

const IID UUID_IVirtualDesktopManagerInternal_10130 {
	0xEF9F1A6C, 0xD3CC, 0x4358, 0xB7, 0x12, 0xF8, 0x4B, 0x63, 0x5B, 0xEB, 0xE7
};
const IID UUID_IVirtualDesktopManagerInternal_10240 {
	0xAF8DA486, 0x95BB, 0x4460, 0xB3, 0xB7, 0x6E, 0x7A, 0x6B, 0x29, 0x62, 0xB5
};
const IID UUID_IVirtualDesktopManagerInternal_14393 {
	0xf31574d6, 0xb682, 0x4cdc, 0xbd, 0x56, 0x18, 0x27, 0x86, 0x0a, 0xbe, 0xc6
};

//
EXTERN_C const IID IID_IVirtualDesktopManagerInternal;

MIDL_INTERFACE("AF8DA486-95BB-4460-B3B7-6E7A6B2962B5")
IVirtualDesktopManagerInternal : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE GetCount(UINT *pCount) = 0;
	virtual HRESULT STDMETHODCALLTYPE MoveViewToDesktop(IApplicationView *pView, IVirtualDesktop *pDesktop) = 0;
	virtual HRESULT STDMETHODCALLTYPE CanViewMoveDesktops(IApplicationView *pView, int *pfCanViewMoveDesktops) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetCurrentDesktop(IVirtualDesktop** desktop) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetDesktops(IObjectArray **ppDesktops) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAdjacentDesktop(IVirtualDesktop *pDesktopReference, AdjacentDesktop uDirection, IVirtualDesktop **ppAdjacentDesktop) = 0;
	virtual HRESULT STDMETHODCALLTYPE SwitchDesktop(IVirtualDesktop *pDesktop) = 0;
	virtual HRESULT STDMETHODCALLTYPE CreateDesktopW(IVirtualDesktop **ppNewDesktop) = 0;
	virtual HRESULT STDMETHODCALLTYPE RemoveDesktop(IVirtualDesktop *pRemove, IVirtualDesktop *pFallbackDesktop) = 0;
	virtual HRESULT STDMETHODCALLTYPE FindDesktop(GUID *desktopId, IVirtualDesktop **ppDesktop) = 0;
};

//
EXTERN_C const IID IID_IVirtualDesktopManager;

MIDL_INTERFACE("a5cd92ff-29be-454c-8d04-d82879fb3f1b")
IVirtualDesktopManager : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE IsWindowOnCurrentVirtualDesktop(/* [in] */ __RPC__in HWND topLevelWindow, /* [out] */ __RPC__out BOOL *onCurrentDesktop) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetWindowDesktopId(/* [in] */ __RPC__in HWND topLevelWindow,  /* [out] */ __RPC__out GUID *desktopId) = 0;
	virtual HRESULT STDMETHODCALLTYPE MoveWindowToDesktop(/* [in] */ __RPC__in HWND topLevelWindow, /* [in] */ __RPC__in REFGUID desktopId) = 0;
};

//
EXTERN_C const IID IID_IVirtualDesktopNotification;

MIDL_INTERFACE("C179334C-4295-40D3-BEA1-C654D965605A")
IVirtualDesktopNotification : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE VirtualDesktopCreated(IVirtualDesktop *pDesktop) = 0;
	virtual HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyBegin(IVirtualDesktop *pDesktopDestroyed, IVirtualDesktop *pDesktopFallback) = 0;
	virtual HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyFailed(IVirtualDesktop *pDesktopDestroyed, IVirtualDesktop *pDesktopFallback) = 0;
	virtual HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyed(IVirtualDesktop *pDesktopDestroyed, IVirtualDesktop *pDesktopFallback) = 0;
	virtual HRESULT STDMETHODCALLTYPE ViewVirtualDesktopChanged(IApplicationView *pView) = 0;
	virtual HRESULT STDMETHODCALLTYPE CurrentVirtualDesktopChanged(IVirtualDesktop *pDesktopOld, IVirtualDesktop *pDesktopNew) = 0;

};

//
EXTERN_C const IID IID_IVirtualDesktopNotificationService;

MIDL_INTERFACE("0CD45E71-D927-4F15-8B0A-8FEF525337BF")
IVirtualDesktopNotificationService : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE Register(IVirtualDesktopNotification *pNotification, DWORD *pdwCookie) = 0;
	virtual HRESULT STDMETHODCALLTYPE Unregister(DWORD dwCookie) = 0;
};