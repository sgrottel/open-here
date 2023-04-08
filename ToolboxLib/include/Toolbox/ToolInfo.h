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
#include <vector>

namespace openhere
{
namespace toolbox
{

	class ToolInfo
	{
	public:

		enum class PathRequirement {
			DontCare,
			Required,
			RequiredNull
		};

		enum class FilesRequirement {
			DontCare,
			RequireOne,
			RequireOneOrMore,
			RequireTwoOrMore,
			RequireNull
		};

		struct StartConfig
		{
			PathRequirement pathRequirement{ PathRequirement::DontCare };
			FilesRequirement filesRequirement{ FilesRequirement::DontCare };
			std::wstring executable;
			std::wstring workingDir;
			std::vector<std::wstring> arguments;
		};

		inline std::wstring const& GetTitle() const { return m_title; }
		inline void SetTitle(std::wstring const& title) { m_title = title; }

		inline std::wstring const& GetIconFile() const { return m_iconFile; }
		inline void SetIconFile(std::wstring const& file) { m_iconFile = file; }

		// A negative ID will trigger the dynmamic-icon code path
		inline int GetIconID() const { return m_iconId; }
		inline void SetIconID(int id) { m_iconId = id; }

		inline std::vector<StartConfig>& StartConfigs() { return m_startConfigs; }
		inline std::vector<StartConfig> const& StartConfigs() const { return m_startConfigs; }

		PathRequirement GetPathRequirement() const;
		FilesRequirement GetFilesRequirement() const;

	private:

		std::wstring m_title;
		std::wstring m_iconFile;
		int m_iconId{ 0 };
		std::vector<StartConfig> m_startConfigs;
	};

}
}
