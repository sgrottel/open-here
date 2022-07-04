// Open Here
// Copyright 2022 SGrottel (https://www.sgrottel.de)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http ://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
//
#include "Toolbox/IconLibrary.h"

#include <stdexcept>

using namespace openhere::toolbox;

namespace
{

	SIZE GetIconSize(HICON icon)
	{
		ICONINFO ii;
		if (!GetIconInfo(icon, &ii)) return { 0, 0 };
		if (ii.hbmMask) DeleteObject(ii.hbmMask);
		if (!ii.hbmColor) return { 0, 0 };
		BITMAP bmp;
		ZeroMemory(&bmp, sizeof(BITMAP));
		GetObject(ii.hbmColor, sizeof(BITMAP), &bmp);
		DeleteObject(ii.hbmColor);
		return { bmp.bmWidth, bmp.bmHeight };
	}

}

IconLibrary::~IconLibrary()
{
	Close();
}

bool IconLibrary::Open(LPCWSTR filename, int width, int height)
{
	Close();

	// try load file, e.g. '.ico' file
	m_icon = (HICON)LoadImageW(NULL, filename, IMAGE_ICON, width, height, LR_LOADFROMFILE);
	if (m_icon)
	{
		AssertSize(m_icon, width, height);
		m_ids.push_back(0u);
		return true;
	}

	// try load as binary resource, e.g. '.exe' or '.dll'
	BOOL hasMod = GetModuleHandleExW(0, filename, &m_lib);
	if (!hasMod || !m_lib)
	{
		m_lib = LoadLibraryExW(filename, NULL, LOAD_LIBRARY_AS_DATAFILE);
	}
	if (!m_lib)
	{
		// throw std::runtime_error("Unable to load the specified file");
		return false;
	}

	struct IconNameSearch {
		std::vector<uint32_t>& ids;
	};
	IconNameSearch search{ m_ids };

	EnumResourceNamesW(m_lib, RT_GROUP_ICON,
		[](HMODULE hModule, LPCWSTR lpType, LPWSTR lpName, LONG_PTR lParam) -> BOOL {
			IconNameSearch* search = reinterpret_cast<IconNameSearch*>(lParam);
			if (IS_INTRESOURCE(lpName))
			{
				search->ids.push_back(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(lpName)));
			}
			else
			{
				// If name-based resources ever become a thing, then this can be refactored
				search->ids.push_back(static_cast<uint32_t>(-1));
			}
			return true;
		}, reinterpret_cast<LONG_PTR>(&search));

	return true;
}

void IconLibrary::Close()
{
	if (m_icon != NULL)
	{
		DestroyIcon(m_icon);
		m_icon = NULL;
	}
	if (m_lib != NULL)
	{
		FreeLibrary(m_lib);
		m_lib = NULL;
	}
	m_ids.clear();
}

HICON IconLibrary::GetIcon(uint32_t id, int width, int height) const
{
	if (m_icon != NULL)
	{
		return m_icon;
	}

	if (m_lib != NULL)
	{
		HICON icon = static_cast<HICON>(LoadImageW(
			m_lib,
			MAKEINTRESOURCEW(id),
			IMAGE_ICON,
			width,
			height,
			LR_SHARED));
		AssertSize(icon, width, height);
		return icon;
	}

	return NULL;
}

void IconLibrary::AssertSize(HICON& icon, int width, int height) const
{
	SIZE iconSize = GetIconSize(icon);
	if (iconSize.cx > 0 && iconSize.cy > 0 && (iconSize.cx != width || iconSize.cy != height))
	{
		HICON scaled = (HICON)CopyImage(icon, IMAGE_ICON, width, height, LR_COPYFROMRESOURCE);
		if (scaled != icon)
		{
			SIZE scaledSize = GetIconSize(scaled);
			DestroyIcon(icon);
			icon = scaled;
		}
	}
}
