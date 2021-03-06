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
#include "IconFallbackCache.h"

#include "Toolbox/Config.h"

#include <memory>
#include <functional>
#include <filesystem>
#include <string>
#include <sstream>
#include <unordered_map>
#include <iomanip>
#include <fstream>

using namespace openhere::toolbox;

namespace
{

	std::filesystem::path GetCachePath()
	{
		std::filesystem::path cfgPath{ Config::Path() };
		cfgPath /= "iconcache";
		std::filesystem::create_directories(cfgPath);
		return cfgPath;
	}


	std::wstring GetCacheFileName(std::filesystem::path const& path, int id, int width, int height)
	{
		std::wstringstream nameBuilder;
		std::hash<std::wstring> hasher;

		size_t pathHash = hasher(path.parent_path().wstring());

		nameBuilder << std::setw(8) << std::setfill(L'0') << std::hex << pathHash
			<< L"-" << std::setw(4) << id
			<< L"-" << std::setw(4) << width
			<< L"-" << std::setw(4) << height
			<< L"-" << path.filename().replace_extension().wstring();

		return nameBuilder.str();
	}

}

IconFallbackCache& IconFallbackCache::Instance()
{
	static IconFallbackCache inst;
	return inst;
}


HBITMAP IconFallbackCache::Load(LPCWSTR path, int id, int width, int height)
{
	std::filesystem::path filepath{ GetCachePath() };
	filepath /= GetCacheFileName(path, id, width, height);
	if (!std::filesystem::is_regular_file(filepath)) return NULL;
	
	std::vector<uint8_t> imgData;
	imgData.resize(width * height * 4);

	std::ifstream file;
	file.open(filepath, std::ios::binary | std::ios::in);
	if (!file.is_open()) return NULL;

	const int line = width * 4;
	for (int y = 0; y < height; ++y)
	{
		file.read(reinterpret_cast<char*>(imgData.data() + line * (height - y - 1)), line);
	}
	file.close();

	return CreateBitmap(width, height, 1, 32, imgData.data());
}


void IconFallbackCache::Store(LPCWSTR path, int id, HBITMAP bmp)
{
	std::vector<uint8_t> imgData;
	std::unique_ptr<std::remove_pointer_t<HDC>, std::function<void(HDC)>> memDC(CreateCompatibleDC(NULL), &DeleteDC);
	SelectObject(memDC.get(), bmp);

	BITMAPINFO bmpInfo;
	ZeroMemory(&bmpInfo, sizeof(BITMAPINFO));
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	if (GetDIBits(memDC.get(), bmp, 0, 0, NULL, &bmpInfo, DIB_RGB_COLORS) == 0)
	{
		// failed to query bitmap
		return;
	}
	if (bmpInfo.bmiHeader.biBitCount != 32)
	{
		// unexpected format
		return;
	}
	if (bmpInfo.bmiHeader.biCompression != BI_RGB && bmpInfo.bmiHeader.biCompression != BI_BITFIELDS)
	{
		// unexpected format
		return;
	}

	imgData.resize(bmpInfo.bmiHeader.biWidth * bmpInfo.bmiHeader.biHeight * 4);
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	if (GetDIBits(memDC.get(), bmp, 0, bmpInfo.bmiHeader.biHeight, imgData.data(), &bmpInfo, DIB_RGB_COLORS) == 0)
	{
		// failed to get bitmap data
		return;
	}
	memDC.reset();
	std::filesystem::path p{ path };

	std::lock_guard<std::mutex> lock{ m_asyncsLock };
	m_asyncs.push_back(
		std::async(std::launch::async,
		[](
			std::filesystem::path p,
			int id,
			int w,
			int h,
			std::vector<uint8_t>&& data)
		{
			std::wstring name{ GetCacheFileName(p, id, w, h) };
			std::filesystem::path cfgPath{ GetCachePath() };
			std::ofstream file;
			file.open(cfgPath / name, std::ios::out | std::ios::binary);
			if (file.is_open())
			{
				file.write(reinterpret_cast<const char*>(data.data()), data.size());
				file.close();
			}
		},
		std::move(p),
		id,
		bmpInfo.bmiHeader.biWidth,
		bmpInfo.bmiHeader.biHeight,
		std::move(imgData)));
}


IconFallbackCache::IconFallbackCache()
{
	std::lock_guard<std::mutex> lock{ m_asyncsLock };
	m_asyncs.push_back(
		std::async(std::launch::async,
		[]() {
			Sleep(1000);
			// Cleanup!
			// Delete all icon cache files older than 14 days.
			for (std::filesystem::directory_iterator fn{ GetCachePath() }, end; fn != end; ++fn)
			{
				if (!fn->is_regular_file()) continue;
				int ageInDays = std::chrono::duration_cast<std::chrono::days>(
					std::chrono::file_clock::now() - fn->last_write_time()).count();
				if (ageInDays >= 14)
				{
					std::filesystem::remove(fn->path());
				}
			}
		})
	);
}
