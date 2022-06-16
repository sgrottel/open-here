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
#pragma once

#include "ColorFactory.h"

#include <functional>
#include <mutex>
#include <unordered_map>
#include <future>

class BitmapFactory
{
public:
	HBITMAP MakeDemo(RECT const& rect);

	HBITMAP MakeExitButton(RECT const& rect, ColorFactory const& colors);

	HBITMAP GetBroken(RECT const& rect);

	void Async(
		RECT const& rect,
		std::function<HBITMAP(BitmapFactory& bitmapFactory, RECT const& rect, unsigned int param, void* context)> loader,
		std::function<void(HBITMAP hBmp, unsigned int param, void* context)> setter,
		unsigned int param,
		void* context);

	HBITMAP LoadFromIcon(HINSTANCE hInst, LPWSTR name, RECT const& rect);

	HBITMAP LoadFromIconFile(LPCWSTR path, int id, RECT const& rect);

	void MultiplyColor(HBITMAP hBmp, COLORREF color);

	HBITMAP LoadExplorerIcon(std::wstring const& type, std::wstring const& path, RECT const& rect);

private:

	HBITMAP FromIcon(HICON icon, int width, int height);

	HBITMAP LoadFolderIcon(std::wstring const& path, RECT const& rect);

	void CheckTransparency(HBITMAP hBmp);

	std::mutex m_jobLock;
	unsigned int m_nextJobId{ 1 };
	std::unordered_map<unsigned int, std::shared_future<void>> m_jobs;
};
