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

namespace openhere
{
namespace toolbox
{

	class IconLibrary
	{
	public:
		IconLibrary() = default;
		IconLibrary(const IconLibrary&) = delete;
		IconLibrary(IconLibrary&&) = default;
		~IconLibrary();
		IconLibrary& operator=(const IconLibrary&) = delete;
		IconLibrary& operator=(IconLibrary&&) = default;

		bool Open(LPCWSTR filename, int width, int height);
		void Close();

		inline uint32_t GetCount() const
		{
			return static_cast<uint32_t>(m_ids.size());
		}
		inline uint32_t GetId(uint32_t idx) const
		{
			return m_ids[idx];
		}

		HICON GetIcon(uint32_t id, int width, int height) const;

	protected:
	private:
		void AssertSize(HICON& icon, int width, int height) const;

		HICON m_onlyIcon{ NULL };
		HMODULE m_lib{ NULL };
		std::vector<uint32_t> m_ids;
	};

}
}
