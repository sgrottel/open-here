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

#include "ToolInfo.h"

#include <string>
#include <vector>
#include <functional>

namespace openhere
{
namespace toolbox
{

	class ToolRunner
	{
	public:

		ToolInfo::StartConfig const* SelectStartConfig(ToolInfo const& tool, std::wstring const& path, std::vector<std::wstring> const& files, bool checkPaths);

		bool CanStart(ToolInfo const& tool, std::wstring const& path, std::vector<std::wstring> const& files, std::wstring& outErrMsg);

		void ApplyVariables(ToolInfo::StartConfig& inOutToolStartConfig, std::wstring const& path, std::vector<std::wstring> const& files);

		bool Start(ToolInfo::StartConfig const& toolStartConfig, unsigned int* outProcId);

	};

}
}
