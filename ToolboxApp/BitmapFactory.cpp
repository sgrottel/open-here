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
#include "pch.h"
#include "BitmapFactory.h"

#include <shellapi.h>

#include <vector>
#include <cassert>

namespace
{

	struct bgra {
		uint8_t b, g, r, a;
	};

	bgra ToBGRA(COLORREF const& color)
	{
		return {
			GetBValue(color),
			GetGValue(color),
			GetRValue(color),
			255
		};
	}

	bgra PreMultAlpha(bgra const& color, uint8_t a)
	{
		assert(color.a == 255);
		return {
			static_cast<uint8_t>(static_cast<unsigned int>(color.b) * a / 255u),
			static_cast<uint8_t>(static_cast<unsigned int>(color.g) * a / 255u),
			static_cast<uint8_t>(static_cast<unsigned int>(color.r) * a / 255u),
			a
		};
	}

	bgra MidMix(bgra const& c1, bgra const& c2)
	{
		assert(c1.a == 255);
		assert(c2.a == 255);
		return {
			static_cast<uint8_t>((static_cast<unsigned int>(c1.b) + static_cast<unsigned int>(c2.b)) / 2),
			static_cast<uint8_t>((static_cast<unsigned int>(c1.g) + static_cast<unsigned int>(c2.g)) / 2),
			static_cast<uint8_t>((static_cast<unsigned int>(c1.r) + static_cast<unsigned int>(c2.r)) / 2),
			255
		};
	}

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


HBITMAP BitmapFactory::MakeDemo(RECT const& rect)
{
	const int width = rect.right - rect.left;
	const int height = rect.bottom - rect.top;

	std::vector<bgra> bmp;
	bmp.resize(width * height);

	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			size_t pos = y * width + x;

			bmp[pos] = bgra{ 8, 0, 0, 16 };
		}
	}

	HBITMAP hbmp = CreateBitmap(width, height, 1, 32, bmp.data());
	return hbmp;
}


HBITMAP BitmapFactory::MakeExitButton(RECT const& rect, ColorFactory const& colors)
{
	const int width = rect.right - rect.left;
	const int height = rect.bottom - rect.top;

	std::vector<bgra> bmp;
	bmp.resize(width * height);
	
	auto pos = [&width](int x, int y) { return x + y * width; };

	bgra const redBase{ ToBGRA(colors.GetRedColor()) };
	bgra const red{ PreMultAlpha(redBase, 160) };
	bgra const border{ 0, 0, 0, 64 };

	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			bmp[pos(x, y)] =
				(x == 0 || y == 0 || x == width - 1 || y == height - 1)
				? border
				: red;
		}
	}

	const int midX = width / 2;
	const int midY = height / 2;
	const int rad = (std::min)(midX * 2 / 5, midY * 2 / 5);

	bgra const foreground{ ToBGRA(colors.GetForegroundColor()) };
	bgra const foregroundBorder{ PreMultAlpha(MidMix(foreground, redBase), 191) };

	for (int r = -rad; r <= rad; ++r)
	{
		int const x1 = midX - std::abs(r) - 1;
		int const x2 = midX + std::abs(r) + 2;

		for (int x = x1; x <= x2; ++x)
		{
			int d = (std::min)(x - x1, x2 - x);
			if (d >= 4) continue;

			bmp[pos(x, midY + r)] = 
				(d == 0 || d == 3)
				? foregroundBorder
				: foreground;
		}
	}

	HBITMAP hbmp = CreateBitmap(width, height, 1, 32, bmp.data());
	return hbmp;
}


HBITMAP BitmapFactory::GetBroken(RECT const& rect)
{
	const int width = rect.right - rect.left;
	const int height = rect.bottom - rect.top;
	std::vector<bgra> bmp;
	bmp.resize(width * height);
	auto pos = [&width](int x, int y) { return x + y * width; };

	bgra const bkgnd{ 0, 0, 0, 0 };
	bgra const w{ 255, 255, 255, 255 };
	bgra const b{ 0, 0, 0, 255 };

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			bmp[pos(x, y)] = bkgnd;

			if (x == 0 || y == 0 || x == width - 1 || y == height - 1) continue; // border

			if (x < 2 || y < 2 || x > width - 3 || y > height - 3 || x == y || x == (height - y))
			{
				bmp[pos(x, y)] = b;
				continue;
			}
			if (x < 3 || y < 3 || x > width - 4 || y > height - 4 || x + 1 == y || x + 1 == (height - y))
			{
				bmp[pos(x, y)] = w;
				continue;
			}

		}
	}

	HBITMAP hbmp = CreateBitmap(width, height, 1, 32, bmp.data());
	return hbmp;
}


void BitmapFactory::Async(
	RECT const& rect,
	std::function<HBITMAP(BitmapFactory& bitmapFactory, RECT const& rect, unsigned int param, void* context)> loader,
	std::function<void(HBITMAP hBmp, unsigned int param, void* context)> setter,
	unsigned int param,
	void* context)
{
	std::lock_guard<std::mutex> lock(m_jobLock);

	unsigned int jobId = m_nextJobId++;

	std::function<void(void)> job = [this, jobId, rect, loader, setter, param, context]() {
		try
		{
			HBITMAP bmp = loader(*this, rect, param, context);
			setter(bmp, param, context);
		}
		catch (...)
		{
			setter(GetBroken(rect), param, context);
		}
		std::shared_future<void> sf;
		{
			std::lock_guard<std::mutex> lock(m_jobLock);
			sf = m_jobs[jobId]; // keep job alive even after we remove it from the management list
			m_jobs.erase(jobId);
		}
	};

	m_jobs[jobId] = std::async(std::launch::async, job).share();
}


HBITMAP BitmapFactory::LoadFromIcon(HINSTANCE hInst, LPWSTR name, RECT const& rect)
{
	int const reqWidth = rect.right - rect.left;
	int const reqHeight = rect.bottom - rect.top;

	HICON icon = (HICON)LoadImageW(hInst, name, IMAGE_ICON, reqWidth, reqHeight, 0);
	if (icon == NULL)
	{
		throw std::runtime_error("Icon not found");
	}

	HBITMAP hbmp = FromIcon(icon, reqWidth, reqHeight);

	DestroyIcon(icon);

	return hbmp;
}


HBITMAP BitmapFactory::LoadFromIconFile(LPCWSTR path, int id, RECT const& rect)
{
	int const width = rect.right - rect.left;
	int const height = rect.bottom - rect.top;

	// try load file, e.g. '.ico' file
	HICON icon = (HICON)LoadImageW(NULL, path, IMAGE_ICON, width, height, LR_LOADFROMFILE | LR_SHARED);
	if (icon)
	{
		HBITMAP hbmp = FromIcon(icon, width, height);
		DestroyIcon(icon);
		return hbmp;
	}

	// try load as binary resource, e.g. '.exe' or '.dll'
	HMODULE mod{ NULL };
	BOOL hasMod = GetModuleHandleExW(0, path, &mod);
	if (!hasMod || !mod)
	{
		mod = LoadLibraryExW(path, NULL, LOAD_LIBRARY_AS_DATAFILE);
	}
	if (!mod)
	{
		throw std::runtime_error("Unable to load the specified file");
	}

	icon = static_cast<HICON>(LoadImageW(
		mod,
		MAKEINTRESOURCEW(id),
		IMAGE_ICON,
		width,
		height,
		LR_SHARED));
	FreeLibrary(mod);
	if (!icon)
	{
		throw std::runtime_error("Icon not found");
	}

	SIZE size = GetIconSize(icon);
	if (size.cx > 0 && size.cy > 0 && (size.cx != width || size.cy != height))
	{
		HICON scaled = (HICON)CopyImage(icon, IMAGE_ICON, width, height, LR_COPYFROMRESOURCE);
		if (scaled != icon)
		{
			SIZE scaledSize = GetIconSize(scaled);
			DestroyIcon(icon);
			icon = scaled;
		}
	}

	HBITMAP hbmp = FromIcon(icon, width, height);

	DestroyIcon(icon);

	return hbmp;
}


void BitmapFactory::MultiplyColor(HBITMAP hBmp, COLORREF color)
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

	unsigned r = GetRValue(color);
	unsigned g = GetGValue(color);
	unsigned b = GetBValue(color);

	for (bgra& c : bmp)
	{
		c.r = static_cast<uint8_t>(c.r * r / 255);
		c.g = static_cast<uint8_t>(c.g * g / 255);
		c.b = static_cast<uint8_t>(c.b * b / 255);
	}

	SetDIBits(dc, hBmp, 0, Bitmap.bmHeight, bmp.data(), reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS);

	DeleteObject(dc);
	ReleaseDC(NULL, hdcScreen);
}


HBITMAP BitmapFactory::LoadExplorerIcon(std::wstring const& type, std::wstring const& path, RECT const& rect)
{
	if (type == L"explorer.exe")
	{
		return LoadFolderIcon(path, rect);
	}

	return LoadFolderIcon(path, rect);
}


HBITMAP BitmapFactory::FromIcon(HICON icon, int width, int height)
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


HBITMAP BitmapFactory::LoadFolderIcon(std::wstring const& path, RECT const& rect)
{
	int const width = rect.right - rect.left;
	int const height = rect.bottom - rect.top;

	auto fallback = [=]() {
		wchar_t pathBuf[MAX_PATH];
		ZeroMemory(pathBuf, sizeof(pathBuf));
		::memcpy(pathBuf, path.c_str(), sizeof(wchar_t) * path.size());

		WORD idx = 0;
		HICON icon = ExtractAssociatedIconW(nullptr, pathBuf, &idx);

		HBITMAP hbmp = FromIcon(icon, width, height);

		DestroyIcon(icon);

		return hbmp;
	};

	SHFILEINFOW fileInfo;
	ZeroMemory(&fileInfo, sizeof(SHFILEINFOW));

	if (!SHGetFileInfoW(path.c_str(), 0, &fileInfo, sizeof(SHFILEINFOW), SHGFI_ICONLOCATION))
	{
		return fallback();
	}

	// try load file, e.g. '.ico' file
	HICON icon = (HICON)LoadImageW(NULL, fileInfo.szDisplayName, IMAGE_ICON, width, height, LR_LOADFROMFILE | LR_SHARED);
	if (icon)
	{
		HBITMAP hbmp = FromIcon(icon, width, height);
		DestroyIcon(icon);
		return hbmp;
	}

	// try load as binary resource, e.g. '.exe' or '.dll'
	HMODULE mod{ NULL };
	BOOL hasMod = GetModuleHandleExW(0, fileInfo.szDisplayName, &mod);
	if (!hasMod || !mod)
	{
		mod = LoadLibraryExW(fileInfo.szDisplayName, NULL, LOAD_LIBRARY_AS_DATAFILE);
	}
	if (!mod) return fallback();

	struct IconNameSearch {
		int index;
		bool found;
		int id;
		std::wstring name;
	};
	IconNameSearch search{ std::abs(fileInfo.iIcon) };

	EnumResourceNamesW(mod, RT_GROUP_ICON, 
		[](HMODULE hModule, LPCWSTR lpType, LPWSTR lpName, LONG_PTR lParam) -> BOOL {
			IconNameSearch* search = reinterpret_cast<IconNameSearch*>(lParam);
			if (search->index > 0)
			{
				search->index--;
				return true;
			}

			search->found = true;

			if (IS_INTRESOURCE(lpName))
			{
				search->id = static_cast<int>(reinterpret_cast<uintptr_t>(lpName));
				search->name.clear();
			}
			else
			{
				search->id = -1;
				search->name = lpName;
			}

			return false;
		}, reinterpret_cast<LONG_PTR>(&search));

	if (search.found)
	{
		icon = static_cast<HICON>(LoadImageW(
			mod,
			(search.id >= 0) ? MAKEINTRESOURCEW(search.id) : search.name.c_str(),
			IMAGE_ICON,
			width,
			height,
			LR_SHARED));
	}
	FreeLibrary(mod);
	if (!icon)
	{
		return fallback();
	}

	SIZE size = GetIconSize(icon);
	if (size.cx > 0 && size.cy > 0 && (size.cx != width || size.cy != height))
	{
		HICON scaled = (HICON)CopyImage(icon, IMAGE_ICON, width, height, LR_COPYFROMRESOURCE);
		if (scaled != icon)
		{
			SIZE scaledSize = GetIconSize(scaled);
			DestroyIcon(icon);
			icon = scaled;
		}
	}

	HBITMAP hbmp = FromIcon(icon, width, height);

	DestroyIcon(icon);

	return hbmp;
}


void BitmapFactory::CheckTransparency(HBITMAP hBmp)
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
