/*
	VulCan - GUIPro Project ( http://glatigny.github.io/guipro/ )

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

#ifndef VOLUME_H
#define VOLUME_H

#include <windows.h>

extern float g_volume_step;

BOOL initVolumeInterface();
void unloadVolumeInterface();
int changeVolumeUp();
int changeVolumeDown();
int changeVolumeMute();
int getVolume();

#endif /* VOLUME_H */
