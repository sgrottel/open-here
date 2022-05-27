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

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Objbase.h>

namespace openhere
{
namespace fileexplorerdetector
{

	class CoInitialize {
	public:
		CoInitialize(DWORD dwCoInit = COINIT_MULTITHREADED);
		~CoInitialize();

		CoInitialize(const CoInitialize& rhs) = delete;
		CoInitialize(CoInitialize&& rhs) = delete;
		CoInitialize& operator=(const CoInitialize& rhs) = delete;
		CoInitialize& operator=(CoInitialize&& rhs) = delete;
	};

}
}
