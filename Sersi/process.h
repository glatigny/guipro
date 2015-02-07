/*  Sersi - GUIPro Project ( http://guipro.sourceforge.net/ )

    Author : DarkSage  aka  Glatigny Jérôme <darksage@darksage.info>

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
#include "sersiProg.h"

void process(const wchar_t* name);
struct SersiProg* execute(const wchar_t* name, const wchar_t* progname, const wchar_t* progparam, const wchar_t* dirparam, bool autoRestart, bool dailyRestart);
void endAllChildren();