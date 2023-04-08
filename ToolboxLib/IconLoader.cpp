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
#include "Toolbox/IconLoader.h"

#include "Toolbox/IconLibrary.h"
#include "IconFallbackCache.h"
#include "Toolbox/LogFile.h"
#include "DynamicIconProviderLoader.h"

#include <vector>
#include <stdexcept>

using namespace openhere::toolbox;

namespace
{

	struct bgra {
		uint8_t b, g, r, a;
	};

}


HBITMAP IconLoader::LoadFromIconFile(LPCWSTR path, int id, SIZE const& size)
{
	int const &width = size.cx;
	int const &height = size.cy;
	HBITMAP hbmp = NULL;

	if (id < 0)
	{
		LogFile::Write() << "Trying to load as dynamic icon provider: " << path;
		DynamicIconProviderLoader dynIcon;
		if (dynIcon.Load(path))
		{
			hbmp = dynIcon.Generate(size);
			if (hbmp != NULL)
			{
				LogFile::Write() << "Icon generated";
				return hbmp;
			}
			else
			{
				LogFile::Write() << "Icon generation failed";
			}
		}
		else
		{
			LogFile::Write() << "Dynamic icon provider load failed";
		}
	}

	IconLibrary lib;
	lib.Open(path, width, height);
	HICON icon = lib.GetIcon(static_cast<uint32_t>(id), width, height);

	LogFile::Write() << "LoadFromIconFile(" << path << ", " << id << ", {" << width << ", " << height << "}) HICON = " << reinterpret_cast<uintptr_t>(icon);

	if (!icon)
	{
		hbmp = IconFallbackCache::Instance().Load(path, id, width, height);
		if (hbmp == NULL)
		{
			throw std::runtime_error("Icon not found");
		}
		return hbmp;
	}

	hbmp = FromIcon(icon, width, height);

	LogFile::Write() << "LoadFromIconFile(" << path << ", " << id << ", {" << width << ", " << height << "}) HBMP = " << reinterpret_cast<uintptr_t>(hbmp);

	DestroyIcon(icon);
	if (hbmp == NULL)
	{
		throw std::runtime_error("Icon not found");
	}
	IconFallbackCache::Instance().Store(path, id, hbmp);

	return hbmp;
}


HBITMAP IconLoader::FromIcon(HICON icon, int width, int height)
{
	std::vector<bgra> bmp;
	bmp.resize(width * height);
	for (bgra& c : bmp) c = bgra{ 0, 0, 0, 0 };
	HBITMAP hbmp = CreateBitmap(width, height, 1, 32, bmp.data());

	HDC hdcScreen = GetDC(NULL);
	HDC dc = CreateCompatibleDC(hdcScreen);
	SelectObject(dc, hbmp);

	DrawIconEx(dc, 0, 0, icon, width, height, 0, 0, DI_NORMAL);

	DeleteObject(dc);
	ReleaseDC(NULL, hdcScreen);

	CheckTransparency(hbmp);

	return hbmp;
}


void IconLoader::CheckTransparency(HBITMAP hBmp)
{
	BITMAP Bitmap;
	GetObject(hBmp, sizeof(BITMAP), (LPSTR)&Bitmap);

	BITMAPINFOHEADER  bi;
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	HDC hdcScreen = GetDC(NULL);
	HDC dc = CreateCompatibleDC(hdcScreen);
	SelectObject(dc, hBmp);

	std::vector<bgra> bmp;
	bmp.resize(Bitmap.bmWidth * Bitmap.bmHeight);

	GetDIBits(dc, hBmp, 0, Bitmap.bmHeight, bmp.data(), reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS);

	bool fixed = false;
	for (bgra& c : bmp)
	{
		uint8_t l = (std::max)(c.r, (std::max)(c.g, c.b));
		if (l > c.a)
		{
			fixed = true;
			c.a = 255;
		}
	}

	if (fixed)
	{
		SetDIBits(dc, hBmp, 0, Bitmap.bmHeight, bmp.data(), reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS);
	}

	DeleteObject(dc);
	ReleaseDC(NULL, hdcScreen);
}
