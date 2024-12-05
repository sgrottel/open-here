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

#include "FileExplorerDetector/FileExplorerDetector.h"

#include <stdexcept>
#include <iostream>

int main()
{
    using openhere::fileexplorerdetector::FileExplorerDetector;
    using openhere::fileexplorerdetector::InstanceInfo;

    FileExplorerDetector detector;

    try
    {
        int found = detector.Detect();
        if (found < 0)
        {
            throw std::runtime_error("Unknown error");
        }
    }
    catch (std::exception const& ex)
    {
        std::cerr << "FAILED to detect file explorer applications: " << ex.what() << std::endl;
        return -1;
    }
    catch (...)
    {
        std::cerr << "FAILED to detect file explorer applications: Unknown error" << std::endl;
        return -1;
    }

    for (InstanceInfo const& inst : detector.GetInstances())
    {
        std::wcout << L"i " << inst.GetHWnd() << L" " << inst.GetInstanceType() << L"\n"
            << L"w " << (inst.IsTopWindow() ? L'T' : L'-') << (inst.IsForegroundWindow() ? L'F' : L'-')
            << L" " << inst.GetZDepth()
            << L" " << inst.GetSubOrder()
            << L"\n";
        for (std::wstring const& s : inst.GetOpenPaths())
        {
            std::wcout << L"p " << s << L"\n";
        }
        for (std::wstring const& s : inst.GetSelectedItems())
        {
            std::wcout << L"s " << s << L"\n";
        }
    }

    std::wcout << L"e " << std::endl;

    return 0;
}
