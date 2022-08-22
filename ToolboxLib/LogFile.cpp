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
#include "Toolbox/LogFile.h"

#include "Toolbox/Config.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <fstream>
#include <filesystem>
#include <chrono>
#include <format>
#include <cassert>

using namespace openhere::toolbox;

namespace
{

	std::filesystem::path MakeLogFileName()
	{
		std::filesystem::path cfg{ Config::Path() };
		cfg /= "logs";
		std::filesystem::create_directories(cfg);

		auto now = std::chrono::system_clock::now();

		// fixing time zone
		const auto& timeZoneDatabase = std::chrono::get_tzdb();
		const auto& currentZone = timeZoneDatabase.current_zone();
		const auto& otherNow = currentZone->to_local(now);
		now = std::chrono::system_clock::time_point{
			otherNow.time_since_epoch()
		};

		cfg /= (std::format("{0:%Y%m%d-}", now)
			+ std::format("{0:%H%M%S}", now).substr(0, 6)
			+ ".txt");

		return cfg;
	}

	std::string ToUtf8(std::wstring const& s, bool addNewLine = false)
	{
		std::string rv;
		int tarSize = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), static_cast<int>(s.size()), nullptr, 0, nullptr, nullptr);
		if (tarSize > 0)
		{
			rv.resize(tarSize + (addNewLine ? 1 : 0));
			int confSize = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), static_cast<int>(s.size()), rv.data(), static_cast<int>(rv.size()), nullptr, nullptr);
			if (confSize != tarSize)
			{
				rv.resize(confSize + (addNewLine ? 1 : 0)); // something is strange.
			}
			if (addNewLine)
			{
				assert(rv.size() == tarSize + 1);
				rv[tarSize] = '\n';
			}
		}
		return std::move(rv);
	}

	void LogFilesCleanup()
	{
		// remove old log files. Only keep the newest
		constexpr size_t keepLogsCount = 5;

		std::filesystem::path cfg{ Config::Path() };
		cfg /= "logs";

		std::vector<std::tuple<std::filesystem::path, std::filesystem::file_time_type>> files;

		for (const auto fileInfo : std::filesystem::directory_iterator{ cfg })
		{
			if (!fileInfo.is_regular_file()) continue;
			files.push_back(std::make_tuple(fileInfo.path(), fileInfo.last_write_time()));
		}

		if (files.size() > keepLogsCount)
		{
			std::sort(files.begin(), files.end(),
				[](auto const& a, auto const& b) {
					return std::get<1>(a) > std::get<1>(b);
				});
			auto end = files.end();
			auto it = files.begin();
			std::advance(it, std::min<size_t>(keepLogsCount, files.size()));
			for (; it != end; ++it)
			{
				std::filesystem::remove(std::get<0>(*it));
			}
		}

	}

	std::filesystem::path const& LogFileNameUnderLock()
	{
		static std::filesystem::path fileName;
		if (fileName.empty())
		{
			fileName = MakeLogFileName();
			LogFilesCleanup();
		}
		return fileName;
	}

}

bool LogFile::m_enabled{ false };

std::vector<std::wstring> LogFile::m_lineBuf{};

std::mutex LogFile::m_writeLock{};


LogFile::Line::Line()
{
	m_stream << "[" << static_cast<int>(GetCurrentThreadId()) << "] ";
}


LogFile::Line::~Line()
{
	LogFile::Write(m_stream);
}


void LogFile::Write(const std::wstringstream& stream)
{
	{
		std::lock_guard<std::mutex> lock{ m_writeLock };

		std::wstring str{ stream.str() };
		bool emitNewLine = (str.back() != L'\n');

		if (!m_enabled)
		{
			if (emitNewLine)
			{
				str += L'\n';
			}
			m_lineBuf.push_back(str);
			return;
		}

		std::ofstream file(LogFileNameUnderLock(), std::ios::out | std::ios::binary | std::ios::app);
		if (!file.is_open())
		{
			return;
		}
		if (!m_lineBuf.empty())
		{
			for (std::wstring const& l : m_lineBuf)
			{
				std::string s{ ToUtf8(l) };
				file.write(s.c_str(), s.size());
			}
			m_lineBuf.clear();
		}
		{
			std::string s = ToUtf8(str, emitNewLine);
			file.write(s.c_str(), s.size());
		}

		file.close();

	}

}


void LogFile::Enable(bool enable)
{
	if (m_enabled == enable) return;
	m_enabled = enable;
	if (!m_enabled) return;
	{
		std::lock_guard<std::mutex> lock{ m_writeLock };
		if (m_lineBuf.empty()) return;
		std::ofstream file(LogFileNameUnderLock(), std::ios::out | std::ios::binary | std::ios::app);
		if (file.is_open())
		{
			for (std::wstring const& l : m_lineBuf)
			{
				std::string s{ ToUtf8(l) };
				file.write(s.c_str(), s.size());
			}
			file.close();
		}
		m_lineBuf.clear();
	}
}
