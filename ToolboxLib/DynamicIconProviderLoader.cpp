// Open Here
// Copyright 2023 SGrottel (https://www.sgrottel.de)
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
#include "DynamicIconProviderLoader.h"

#include <vector>

using namespace openhere::toolbox;

DynamicIconProviderLoader::~DynamicIconProviderLoader()
{
	m_func = nullptr;
	if (m_lib != 0)
	{
		FreeLibrary(m_lib);
	}
}

bool DynamicIconProviderLoader::Load(std::wstring const& path)
{
	m_lib = LoadLibraryW(path.c_str());
	if (m_lib != nullptr)
	{
		m_func = reinterpret_cast<GeneratorFuncType>(GetProcAddress(m_lib, "openhere_generateicon"));
	}
	return m_func != nullptr;
}

HBITMAP DynamicIconProviderLoader::Generate(SIZE const& size)
{
	if (m_func == nullptr) return NULL;

	std::vector<uint8_t> bgraData;
	bgraData.resize(size.cx * size.cy * 4);

	if ((*m_func)(size.cx, size.cy, bgraData.data()) == 0)
	{
		// Function returned FALSE = failure
		return NULL;
	}

	BITMAPINFOHEADER bmih{ 0 };
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biWidth = size.cx;
	bmih.biHeight = size.cy;
	bmih.biPlanes = 1;
	bmih.biBitCount = 32;
	bmih.biCompression = BI_RGB;
	HDC hdc = ::GetDC(nullptr);
	HBITMAP hbmp = CreateDIBitmap(hdc, &bmih, CBM_INIT, bgraData.data(), reinterpret_cast<const BITMAPINFO*>(&bmih), DIB_RGB_COLORS);
	ReleaseDC(nullptr, hdc);
	return hbmp;
}
