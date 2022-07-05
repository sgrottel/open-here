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
#include "Toolbox/Config.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <Shlobj.h>
#include <Shlobj_core.h>

#include "nlohmann/json.hpp"

#include <fstream>
#include <filesystem>

using namespace openhere::toolbox;


std::wstring Config::Path(const wchar_t* filename)
{
	wchar_t appData[MAX_PATH];
	if (FAILED(SHGetFolderPathW(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, appData)))
	{
		if (FAILED(SHGetFolderPathW(NULL, CSIDL_PROFILE | CSIDL_FLAG_CREATE, NULL, 0, appData)))
		{
			wcscpy_s(appData, L".");
		}
	}

	std::filesystem::path path{ appData };
	path = path / L"sgrottel" / L"openhere";

	if (!std::filesystem::is_directory(path))
	{
		std::filesystem::create_directories(path);
	}

	if (filename != nullptr)
	{
		path /= filename;
	}

	return path.wstring().c_str();
}


bool Config::Load()
{
	using nlohmann::json;

	std::wstring path{ Config::Path(configFilename) };
	std::ifstream file{ path };
	if (!file.is_open()) return false;
	json j;
	file >> j;
	file.close();

	if (!j.contains("_type")) throw std::logic_error("No type info");
	if (!j["_type"].is_string()) throw std::logic_error("Type info error 1");
	if (j["_type"].get<std::string>() != fileTypeMarker) throw std::logic_error("Type info error 2");

	if (!j.contains("_version")) throw std::logic_error("No type version info");
	if (!j["_version"].is_array()) throw std::logic_error("Type version error 1");
	if (j["_version"] != json{ 1, 0 }) throw std::logic_error("Type version error 2");

	if (j.contains("startsound"))
	{
		if (!j["startsound"].is_boolean()) throw std::logic_error("`startsound` must be boolean");
		m_playStartSound = j["startsound"].get<bool>();
	}

	if (j.contains("toolstartsound"))
	{
		if (!j["toolstartsound"].is_boolean()) throw std::logic_error("`toolstartsound` must be boolean");
		m_playToolStartSound = j["toolstartsound"].get<bool>();
	}

	if (j.contains("toolstarttofront"))
	{
		if (!j["toolstarttofront"].is_boolean()) throw std::logic_error("`toolstarttofront` must be boolean");
		m_startToolToFront = j["toolstarttofront"].get<bool>();
	}

	return true;
}


bool Config::Save()
{
	using nlohmann::json;
	json tools = json::array();

	tools = json{
		{"_type", fileTypeMarker},
		{"_version", { 1, 0 }},
		{"startsound", m_playStartSound },
		{"toolstartsound", m_playToolStartSound },
		{"toolstarttofront", m_startToolToFront }
	};

	std::wstring path{ Config::Path(configFilename) };
	std::ofstream file{ path };
	if (!file.is_open()) return false;

	file << std::setw(4) << tools << std::endl;

	file.close();
	return true;
}
