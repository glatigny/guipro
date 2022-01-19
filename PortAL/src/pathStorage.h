/*
	PortAL - GUIPro Project ( http://glatigny.github.io/guipro/ )

	Author : Glatigny Jérôme <jerome@darksage.fr>

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

#ifndef PATHSTORAGE_H
#define PATHSTORAGE_H
#ifdef USE_PATH_STORAGE

#include <vector>

typedef struct _pathSlice {
	size_t parent;
	size_t size;
	wchar_t* content;
} pathSlice;

class pathStorage
{
	public:
		std::vector<pathSlice> entries;

		pathStorage();
		~pathStorage();

		size_t add(const wchar_t*);
		bool remove(const wchar_t*);
		bool remove(size_t);
		wchar_t* get(size_t);
		size_t search(const wchar_t*);
		void clear();

	private:
		size_t wcscmpidx(const wchar_t*, const wchar_t*);
};

#endif
#endif /* PATHSTORAGE_H */
