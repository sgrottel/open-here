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
#include "Utf8Utility.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


std::string utf8::fromWString(std::wstring const& str)
{
	int reqLen = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), nullptr, 0, nullptr, nullptr);
	if (reqLen <= 0) return "";

	std::string s;
	s.resize(reqLen);
	WideCharToMultiByte(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), s.data(), static_cast<int>(s.size()), nullptr, nullptr);

	return s;
}


std::wstring utf8::toWString(std::string const& str)
{
	int reqLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), nullptr, 0);
	if (reqLen <= 0) return L"";

	std::wstring s;
	s.resize(reqLen);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), s.data(), static_cast<int>(s.size()));

	return s;
}
