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
#include "Toolbox/ToolInfoListLoader.h"
#include "Toolbox/Config.h"
#include "Utf8Utility.h"

#include "nlohmann/json.hpp"

#include <stdexcept>
#include <filesystem>
#include <fstream>

using namespace openhere::toolbox;


namespace
{

	int toInt(ToolInfo::FilesRequirement r)
	{
		switch (r)
		{
		case ToolInfo::FilesRequirement::DontCare:
			return 0;
		case ToolInfo::FilesRequirement::RequireOne:
			return 1;
		case ToolInfo::FilesRequirement::RequireOneOrMore:
			return 2;
		case ToolInfo::FilesRequirement::RequireTwoOrMore:
			return 3;
		case ToolInfo::FilesRequirement::RequireNull:
			return 4;
		}
		return 0;
	}

	ToolInfo::FilesRequirement toFilesRequirement(int i)
	{
		switch (i)
		{
		case 0:
			return ToolInfo::FilesRequirement::DontCare;
		case 1:
			return ToolInfo::FilesRequirement::RequireOne;
		case 2:
			return ToolInfo::FilesRequirement::RequireOneOrMore;
		case 3:
			return ToolInfo::FilesRequirement::RequireTwoOrMore;
		case 4:
			return ToolInfo::FilesRequirement::RequireNull;
		}
		return ToolInfo::FilesRequirement::DontCare;
	}

	int toInt(ToolInfo::PathRequirement r)
	{
		switch (r)
		{
		case ToolInfo::PathRequirement::DontCare:
			return 0;
		case ToolInfo::PathRequirement::Required:
			return 1;
		case ToolInfo::PathRequirement::RequiredNull:
			return 2;
		}
		return 0;
	}

	ToolInfo::PathRequirement toPathRequirement(int i)
	{
		switch (i)
		{
		case 0:
			return ToolInfo::PathRequirement::DontCare;
		case 1:
			return ToolInfo::PathRequirement::Required;
		case 2:
			return ToolInfo::PathRequirement::RequiredNull;
		}
		return ToolInfo::PathRequirement::DontCare;
	}

}


std::vector<ToolInfo> ToolInfoListLoader::Load()
{
	using nlohmann::json;

	std::vector<ToolInfo> tools;

	std::wstring path{ Config::Path(toolsFilename) };
	std::ifstream file{ path };
	if (!file.is_open()) return tools;
	json j;
	file >> j;
	file.close();

	if (!j.contains("_type")) throw std::logic_error("No type info");
	if (!j["_type"].is_string()) throw std::logic_error("Type info error 1");
	if (j["_type"].get<std::string>() != fileTypeMarker) throw std::logic_error("Type info error 2");

	if (!j.contains("_version")) throw std::logic_error("No type version info");
	if (!j["_version"].is_array()) throw std::logic_error("Type version error 1");
	if (j["_version"] != json{ 1, 0 }) throw std::logic_error("Type version error 2");

	if (!j.contains("tools")) return tools;
	auto const& jTools{ j["tools"] };
	if (!jTools.is_array()) throw std::logic_error("Tools data error 1");

	for (json const& jTool : jTools)
	{
		ToolInfo tool;

		if (!jTool.contains("title")) continue;
		tool.SetTitle(utf8::toWString(jTool["title"].get<std::string>()));

		if (jTool.contains("icon"))
		{
			auto const& jtIcon{ jTool["icon"] };
			if (jtIcon.contains("file"))
			{
				tool.SetIconFile(utf8::toWString(jtIcon["file"].get<std::string>()));
			}
			if (jtIcon.contains("id"))
			{
				tool.SetIconID(jtIcon["id"].get<int>());
			}
		}

		if (jTool.contains("start"))
		{
			if (!jTool["start"].is_array()) throw std::logic_error("Tool `" + utf8::fromWString(tool.GetTitle()) + "` start config must be an array");
			for (auto const& jStart : jTool["start"])
			{
				ToolInfo::StartConfig sc;
				if (!jStart.contains("file")) continue;
				sc.executable = utf8::toWString(jStart["file"].get<std::string>());

				if (jStart.contains("path"))
				{
					sc.workingDir = utf8::toWString(jStart["path"].get<std::string>());
				}

				if (jStart.contains("args"))
				{
					for (auto const& jArg : jStart["args"])
					{
						sc.arguments.push_back(utf8::toWString(jArg.get<std::string>()));
					}
				}

				if (jStart.contains("req_path"))
				{
					sc.pathRequirement = toPathRequirement(jStart["req_path"].get<int>());
				}

				if (jStart.contains("req_file"))
				{
					sc.filesRequirement = toFilesRequirement(jStart["req_file"].get<int>());
				}

				tool.StartConfigs().push_back(std::move(sc));
			}
		}

		tools.push_back(std::move(tool));
	}

	return tools;
}


bool ToolInfoListLoader::Save(std::vector<ToolInfo> const& list)
{
	using nlohmann::json;
	json tools = json::array();

	for (ToolInfo const& ti : list)
	{
		json tool{
			{"title", utf8::fromWString(ti.GetTitle()) },
			{"icon", {
				{"file", utf8::fromWString(ti.GetIconFile()) },
				{"id", ti.GetIconID()}
			}}
		};

		auto const& starts = ti.StartConfigs();
		if (!starts.empty())
		{
			json startConfigs = json::array();
			for (auto const& sc : starts)
			{
				json jsc{
					{"file", utf8::fromWString(sc.executable)},
					{"path", utf8::fromWString(sc.workingDir)},
					{"req_path", toInt(sc.pathRequirement)},
					{"req_file", toInt(sc.filesRequirement)}
				};
				json args = json::array();
				for(auto const& arg: sc.arguments)
				{
					args.push_back(utf8::fromWString(arg));
				}
				jsc["args"] = args;
				startConfigs.push_back(jsc);
			}
			tool["start"] = startConfigs;
		}

		tools.push_back(tool);
	}

	tools = json{
		{"_type", fileTypeMarker},
		{"_version", { 1, 0 }},
		{"tools", tools }
	};

	std::wstring path{ Config::Path(toolsFilename) };
	std::ofstream file{ path };
	if (!file.is_open()) return false;

	file << std::setw(4) << tools << std::endl;

	file.close();
	return true;
}
