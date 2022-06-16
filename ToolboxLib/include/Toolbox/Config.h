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

#include <string>

namespace openhere
{
namespace toolbox
{

	class Config
	{
	public:

		static std::wstring Path(const wchar_t* filename = nullptr);

		bool Load();
		bool Save();

		inline bool GetPlayStartSound() const { return m_playStartSound; }
		inline void SetPlayStartSound(bool playStartSound) { m_playStartSound = playStartSound; }

	private:
		constexpr static const wchar_t* configFilename = L"config.json";
		constexpr static const char* fileTypeMarker = "open-here-config";

		bool m_playStartSound{ false };

	};

}
}
