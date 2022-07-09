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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <vector>
#include <future>

namespace openhere
{
namespace toolbox
{

	class IconFallbackCache
	{
	public:
		static IconFallbackCache& Instance();

		IconFallbackCache(const IconFallbackCache&) = delete;
		IconFallbackCache(IconFallbackCache&&) = delete;
		~IconFallbackCache() = default;
		IconFallbackCache& operator=(const IconFallbackCache&) = delete;
		IconFallbackCache& operator=(IconFallbackCache&&) = delete;

		HBITMAP Load(LPCWSTR path, int id, int width, int height);
		void Store(LPCWSTR path, int id, HBITMAP bmp);

	private:
		IconFallbackCache();

		// store all asyncs (1 + 1xtools) to defer the joins to the end of the program run
		std::vector<std::future<void>> m_asyncs;
	};

}
}