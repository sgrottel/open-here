// Open Here
// Copyright 2022-2025 SGrottel (https://www.sgrottel.de)
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
#include "Toolbox/ToolRunner.h"

#include "VarStringSplitter.h"
#include "Utf8Utility.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdexcept>
#include <filesystem>
#include <regex>
#include <cassert>
#include <memory>
#include <cwctype>

using namespace openhere::toolbox;


ToolInfo::StartConfig const* ToolRunner::SelectStartConfig(ToolInfo const& tool, std::wstring const& path, std::vector<std::wstring> const& files, bool checkPaths)
{
	for (size_t i = 0; i < tool.StartConfigs().size(); ++i)
	{
		ToolInfo::StartConfig const* sc = &tool.StartConfigs()[i];

		if (sc->executable.empty())
		{
			continue;
		}

		switch (sc->pathRequirement)
		{
		case ToolInfo::PathRequirement::DontCare:
			break;
		case ToolInfo::PathRequirement::Required:
			if (path.empty()) continue;
			break;
		case ToolInfo::PathRequirement::RequiredNull:
			if (!path.empty()) continue;
			break;
		default:
			continue;
		}

		switch (sc->filesRequirement)
		{
		case ToolInfo::FilesRequirement::DontCare:
			break;
		case ToolInfo::FilesRequirement::RequireNull:
			if (!files.empty()) continue;
			break;
		case ToolInfo::FilesRequirement::RequireOne:
			if (files.size() != 1) continue;
			break;
		case ToolInfo::FilesRequirement::RequireOneOrMore:
			if (files.empty()) continue;
			break;
		case ToolInfo::FilesRequirement::RequireTwoOrMore:
			if (files.size() <= 1) continue;
			break;
		default:
			continue;
		}

		if (checkPaths)
		{
			if (!std::filesystem::is_regular_file(sc->executable))
				continue;

			if (!sc->workingDir.empty() && !std::filesystem::is_directory(sc->workingDir))
				continue;
		}

		// all checks succeeded
		return sc;
	}

	return nullptr;
}


bool ToolRunner::CanStart(ToolInfo const& tool, std::wstring const& path, std::vector<std::wstring> const& files, std::wstring& outErrMsg)
{
	if (tool.StartConfigs().size() <= 0)
	{
		outErrMsg = L"No start configuration";
		return false;
	}

	auto const* sc = SelectStartConfig(tool, path, files, false);
	if (sc == nullptr)
	{
		outErrMsg = L"No start configuration matched";
		return false;
	}

	sc = SelectStartConfig(tool, path, files, true);
	if (sc == nullptr)
	{
		outErrMsg = L"Not found";
		return false;
	}

	return true;
}


void ToolRunner::ApplyVariables(ToolInfo::StartConfig& inOutToolStartConfig, std::wstring const& path, std::vector<std::wstring> const& files)
{
	// variables:
	//  ${path}
	//  ${file}			(first file)
	//  ${file\d+}		(\d+th file; starting at zero, no leading zeros allowed)
	//  ${file\d+dir}	(directory path of the \d+th file; starting at zero, no leading zeros allowed)

	// array variables:
	//  ${files}			(array of all files *; this one is special, as it multiplies argument variables)
	//  ${filesafter\d+}	(array of all files after the \d+th file; this one is special, as it multiplies argument variables)
	// You can only use one array valuable per argument.

	// Note, if you need to "escape" the string "${" for a very strange file name, write it like this:
	//   "$${}{"
	// The empty variable in the middle will protect the surrounding characters from being detected as one sequence.
	// Ugly, I know, but I consider this an edge case.

	std::vector<std::wstring_view> splits = VariableSplit(inOutToolStartConfig.workingDir);
	if (splits.size() > 1)
	{
		assert(splits.size() % 2 == 1);
		std::vector<std::wstring> values;
		values.resize(splits.size() / 2);
		assert(splits.size() - 1 == values.size() * 2);

		// need var replacements
		for (size_t i = 1; i < splits.size(); i += 2)
		{
			auto const& nv = splits[i];
			std::wstring n;
			n.resize(nv.size());
			std::transform(nv.begin(), nv.end(), n.begin(), towlower);

			std::wstring& newVal = values[i / 2];

			if (n == L"path")
			{
				newVal = path;
			}
			else if (n == L"file")
			{
				if (!files.empty())
				{
					newVal = files[0];
				}
			}
			else
			{
				for (int idx = 0; idx < static_cast<int>(files.size()); ++idx)
				{
					std::wstring tn = L"file" + std::to_wstring(idx);
					if (n == tn)
					{
						newVal = files[idx];
						break;
					}
					tn = L"file" + std::to_wstring(idx) + L"dir";
					if (n == tn)
					{
						newVal = std::filesystem::path{ files[idx] }.parent_path().wstring();
						break;
					}
				}
			}

			splits[i] = newVal;
		}

		inOutToolStartConfig.workingDir = Join(splits);
	}

	for (int argI = 0; argI < static_cast<int>(inOutToolStartConfig.arguments.size()); ++argI)
	{
		splits = VariableSplit(inOutToolStartConfig.arguments[argI]);
		if (splits.size() <= 1) continue;

		assert(splits.size() % 2 == 1);
		std::vector<std::wstring> values;
		values.resize(splits.size() / 2);
		assert(splits.size() - 1 == values.size() * 2);

		int arrayArgIdx = -1;

		// need var replacements
		for (size_t i = 1; i < splits.size(); i += 2)
		{
			auto const& nv = splits[i];
			std::wstring n;
			n.resize(nv.size());
			std::transform(nv.begin(), nv.end(), n.begin(), towlower);

			std::wstring& newVal = values[i / 2];

			if (n == L"path")
			{
				newVal = path;
			}
			else if (n == L"file")
			{
				if (!files.empty())
				{
					newVal = files[0];
				}
			}
			else if (n == L"files")
			{
				if (arrayArgIdx != -1)
				{
					throw std::logic_error("You may only use one array variable per argument");
				}
				arrayArgIdx = static_cast<int>(i);
				newVal = n;
			}
			else
			{
				for (int idx = 0; idx < static_cast<int>(files.size()); ++idx)
				{
					std::wstring tn = L"file" + std::to_wstring(idx);
					if (n == tn)
					{
						newVal = files[idx];
						break;
					}
					tn = L"file" + std::to_wstring(idx) + L"dir";
					if (n == tn)
					{
						newVal = std::filesystem::path{ files[idx] }.parent_path().wstring();
						break;
					}
					tn = L"filesafter" + std::to_wstring(idx);
					if (n == tn)
					{
						if (arrayArgIdx != -1)
						{
							throw std::logic_error("You may only use one array variable per argument");
						}
						arrayArgIdx = static_cast<int>(i);
						newVal = n;
					}
				}
			}

			splits[i] = newVal;
		}

		if (arrayArgIdx < 0)
		{
			inOutToolStartConfig.arguments[argI] = Join(splits);
		}
		else
		{
			// multiply argument per array-variable entry
			typedef std::vector<std::wstring>::const_iterator iterator;
			iterator begin = files.cend(), end = files.cend();

			if (splits[arrayArgIdx] == L"files")
			{
				begin = files.cbegin();
				end = files.cend();
			}
			else
			{
				for (int idx = 0; idx < static_cast<int>(files.size()); ++idx)
				{
					std::wstring tn = L"filesafter" + std::to_wstring(idx);
					if (splits[arrayArgIdx] == tn)
					{
						begin = files.cbegin() + std::min<size_t>(idx, files.size());
						end = files.cend();
						break;
					}
				}
			}

			std::vector<std::wstring> newArgs;
			for (iterator v = begin; v != end; ++v)
			{
				splits[arrayArgIdx] = *v;
				newArgs.push_back(Join(splits));
			}

			inOutToolStartConfig.arguments.erase(inOutToolStartConfig.arguments.begin() + argI);
			for (std::wstring& s : newArgs)
			{
				inOutToolStartConfig.arguments.insert(inOutToolStartConfig.arguments.begin() + argI, std::move(s));
				argI++;
			}
			argI--;

		}
	}
}


bool ToolRunner::Start(ToolInfo::StartConfig const& toolStartConfig, unsigned int* outProcId)
{
	std::wstringstream cmdLine;
	cmdLine << std::filesystem::path{ toolStartConfig.executable }.filename().wstring();

	for (size_t i = 0; i < toolStartConfig.arguments.size(); ++i)
	{
		cmdLine << L" ";
		std::wstring const& arg = toolStartConfig.arguments[i];

		if (std::find_if(arg.cbegin(), arg.cend(), [](wchar_t const& c) { return std::iswspace(c) != 0; }) != arg.cend())
		{
			cmdLine << L"\"";
			if (std::find(arg.cbegin(), arg.cend(), L'"') != arg.cend())
			{
				std::wstring a{ arg };
				for (size_t i = 0; i < a.size(); ++i)
				{
					if (a[i] == L'"') {
						a = a.substr(0, i) + L'\\' + a.substr(i);
						++i;
					}
				}
				cmdLine << a;
			}
			else
			{
				cmdLine << arg;
			}
			cmdLine << L"\"";
		}
		else
		{
			cmdLine << arg;
		}
	}

	std::wstring cmd{ cmdLine.str() };
	
	STARTUPINFOW si;
	ZeroMemory(&si, sizeof(STARTUPINFOW));
	si.cb = sizeof(STARTUPINFOW);

	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	BOOL cpr = CreateProcessW(
		toolStartConfig.executable.c_str(),
		cmd.data(),
		nullptr, nullptr, FALSE,
		CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP,
		nullptr,
		(!toolStartConfig.workingDir.empty())
			? toolStartConfig.workingDir.c_str()
			: nullptr,
		&si, &pi);
	if (cpr == FALSE)
	{
		DWORD le = GetLastError();
		std::string msg{ "Failed to create Process: " };
		msg += std::to_string(le);
		throw std::runtime_error(msg.c_str());
	}

	if (outProcId != nullptr)
	{
		*outProcId = pi.dwProcessId;
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return true;
}
