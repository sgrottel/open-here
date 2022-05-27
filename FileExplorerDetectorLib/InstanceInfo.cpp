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

#include "FileExplorerDetector/InstanceInfo.h"

using namespace openhere::fileexplorerdetector;

intptr_t InstanceInfo::GetHWnd() const
{
	return hWnd;
}

void InstanceInfo::SetHWnd(intptr_t hwnd)
{
	hWnd = hwnd;
}

unsigned int InstanceInfo::GetZDepth() const
{
	return zDepth;
}

void InstanceInfo::SetZDepth(unsigned int z)
{
	zDepth = z;
}

std::wstring const& InstanceInfo::GetInstanceType() const
{
	return instanceType;
}

void InstanceInfo::SetInstanceType(std::wstring const& type)
{
	instanceType = type;
}

std::vector<std::wstring> const& InstanceInfo::GetOpenPaths() const
{
	return openPaths;
}

std::vector<std::wstring>& InstanceInfo::AccessOpenPaths()
{
	return openPaths;
}

std::vector<std::wstring> const& InstanceInfo::GetSelectedItems() const
{
	return selectedItems;
}

std::vector<std::wstring>& InstanceInfo::AccessSelectedItems()
{
	return selectedItems;
}

bool InstanceInfo::IsTopWindow() const
{
	return isTopWindow;
}

void InstanceInfo::SetIsTopWindow(bool isTop)
{
	isTopWindow = isTop;
}

bool InstanceInfo::IsForegroundWindow() const
{
	return isForegroundWindow;
}

void InstanceInfo::SetIsForegroundWindow(bool isForeground)
{
	isForegroundWindow = isForeground;
}
