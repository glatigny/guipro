/*
	PortAL - GUIPro Project ( http://glatigny.github.io/guipro/ )

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

#ifndef __LAUNCH_H__
#define __LAUNCH_H__

#include <windows.h>
#include "portalConfig.h"

#ifndef _USING_V110_SDK71_ // WIN8_SUPPORT

#include <atlbase.h>
//#include <atlstr.h>
#include <Shobjidl.h>

#else

typedef long HRESULT;

#endif /* _USING_V110_SDK71_ / WIN8_SUPPORT */

void launch(PortalProg* prog);
void fireEvent(UINT evt);
HRESULT launchRTApp(wchar_t* app);

#endif /* __LAUNCH_H__ */
