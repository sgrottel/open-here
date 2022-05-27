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

#include <string>
#include <vector>

namespace openhere
{
namespace fileexplorerdetector
{

	class InstanceInfo
	{
	public:

		intptr_t GetHWnd() const;
		void SetHWnd(intptr_t hwnd);

		unsigned int GetZDepth() const;
		void SetZDepth(unsigned int z);

		std::wstring const& GetInstanceType() const;
		void SetInstanceType(std::wstring const& type);

		std::vector<std::wstring> const& GetOpenPaths() const;
		std::vector<std::wstring>& AccessOpenPaths();

		std::vector<std::wstring> const& GetSelectedItems() const;
		std::vector<std::wstring>& AccessSelectedItems();

		bool IsTopWindow() const;
		void SetIsTopWindow(bool isTop);

		bool IsForegroundWindow() const;
		void SetIsForegroundWindow(bool isForeground);

	private:
		intptr_t hWnd = 0;
		unsigned int zDepth = 0;
		std::wstring instanceType;
		std::vector<std::wstring> openPaths;
		std::vector<std::wstring> selectedItems;
		bool isTopWindow = false;
		bool isForegroundWindow = false;
	};

}
}