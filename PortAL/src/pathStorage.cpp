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

#include "pathStorage.h"
#ifdef USE_PATH_STORAGE

pathStorage::pathStorage()
{
}

/* ------------------------------------------------------------------------------------------------- */

pathStorage::~pathStorage()
{
	clear();
}

/* ------------------------------------------------------------------------------------------------- */

size_t pathStorage::add(const wchar_t* content)
{
#define NEEDED_DIFF 2
	size_t len = wcslen(content);
	if (len >= USHRT_MAX)
		return 0;

	size_t pos = 0, eq = 0, cut_entry = 0, next_parent = 0, next_len = 0;
	const wchar_t* next_content = nullptr;
	for (std::vector<pathSlice>::iterator i = entries.begin(); i != entries.end(); ++i, ++pos)
	{
		// Search exact match
		if (i->parent == 0 && i->size == len && wcscmp(content, i->content) == 0)
			return (pos + 1);

		// Search similar content (when no parent)
		//-
		if (i->parent == 0 && (eq = this->wcscmpidx(content, i->content)) > 0)
		{
			// Cut or continue the search
			if (len >= eq && eq < i->size)
			{
				// Cut
				cut_entry = (pos + 1);
			}
			else
			{
				// Continue search
				next_parent = (pos + 1);
				next_content = (content + eq);
				next_len = len - eq;
			}
			break;
		}
	}

	while (next_parent > 0)
	{
		bool found = false;
		pos = 0;
		for (std::vector<pathSlice>::iterator i = entries.begin(); i != entries.end(); ++i, ++pos)
		{
			if (i->parent == next_parent && ((eq = this->wcscmpidx(next_content, i->content)) > NEEDED_DIFF))
			{
				if (next_len >= eq && eq < wcslen(i->content))
				{
					cut_entry = (pos + 1);
					next_parent = 0;
				}
				else
				{
					next_parent = (pos + 1);
					next_content += eq;
					next_len -= eq;
				}
				found = true;
				break;
			}
		}
		if (!found)
		{
			// Add entry
			break;
		}
	}

	if (cut_entry > 0)
	{
		pathSlice c = entries[cut_entry-1];

		wchar_t* cut_content = _wcsdup(c.content + eq);
		free(c.content);
		c.content = cut_content;

		const wchar_t* src_content = ((next_content != nullptr) ? next_content : content);
		size_t src_len = ((next_content != nullptr) ? next_len : len);

		cut_content = (wchar_t*)malloc(sizeof(wchar_t) * (eq+1));
		wcsncpy_s(cut_content, (eq+1), src_content, eq);

		// Add the common entry
		pathSlice entry = { c.parent, (len - src_len) + eq, cut_content };
		entries.push_back(entry);
		//
		c.parent = entries.size();

		// Add the new entry
		if (src_len > eq)
		{
			entry = { c.parent, len, _wcsdup(src_content + eq) };
			entries.push_back(entry);
		}

		entries[cut_entry - 1] = c;

		return entries.size();
	}

	if (next_parent > 0 && next_content != nullptr)
	{
		pathSlice entry = { next_parent, len, _wcsdup(next_content) };
		entries.push_back(entry);
		return entries.size();
	}

	pathSlice entry = { 0, len, _wcsdup(content) };
	entries.push_back(entry);
	return entries.size();
#undef NEEDED_DIFF
}

/* ------------------------------------------------------------------------------------------------- */

// Return the length of similar content. 0 if no match
//
size_t pathStorage::wcscmpidx(const wchar_t* first, const wchar_t* second)
{
	size_t ret = 0;
	while (*first == *second++ && ++ret)
		if (*first++ == 0)
			return (ret - 1);
	return ret;
}

/* ------------------------------------------------------------------------------------------------- */

bool pathStorage::remove(const wchar_t* content)
{
	return false;
}

/* ------------------------------------------------------------------------------------------------- */

bool pathStorage::remove(size_t id)
{
	return false;
}

/* ------------------------------------------------------------------------------------------------- */

wchar_t* pathStorage::get(size_t id)
{
	if (entries.size() < (id-1))
		return NULL;

	wchar_t* ret = NULL;
	if(!entries[id-1].size)
		return ret;

	pathSlice c = entries[id - 1];
	ret = (wchar_t*)malloc(sizeof(wchar_t) * (c.size+1));
	memset(ret, 0, sizeof(wchar_t) * (c.size + 1));

	size_t s = 0,
		pos = c.size - wcslen(c.content);
	wcscpy_s(ret + pos, c.size + 1 - pos, c.content);

	while (c.parent > 0)
	{
		c = entries[c.parent - 1];
		s = wcslen(c.content);
		pos -= s;
		wmemcpy_s(ret + pos, c.size + 1 - pos, c.content, s);
	}
	return ret;
}

/* ------------------------------------------------------------------------------------------------- */

size_t pathStorage::search(const wchar_t* content)
{
	return 0;
}

/* ------------------------------------------------------------------------------------------------- */

void pathStorage::clear()
{
	for (std::vector<pathSlice>::iterator i = entries.begin(); i != entries.end(); i++)
	{
		free(i->content);
	}
}

/* ------------------------------------------------------------------------------------------------- */

#endif