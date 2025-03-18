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

#include "CmdLine.h"

#include "FileExplorerDetector/FileExplorerDetector.h"

#include <stdexcept>
#include <iostream>

void PrintDetectorReport(openhere::fileexplorerdetector::FileExplorerDetector const& detector)
{
    using openhere::fileexplorerdetector::InstanceInfo;

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
}

void StartHere(CmdLine const& cmdLine, openhere::fileexplorerdetector::FileExplorerDetector const& detector)
{

    std::cerr << "Not implemented!" << std::endl;

}

int wmain(int argc, wchar_t* argv[])
{
    using openhere::fileexplorerdetector::FileExplorerDetector;

    CmdLine cmdLine;
    if (!cmdLine.Parse(argc, argv))
    {
        return 0;
    }
    if (cmdLine.command == Command::Error)
    {
        return -1;
    }

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

    switch (cmdLine.command)
    {
    case Command::Default:
        PrintDetectorReport(detector);
        break;

    case Command::StartHere:
        StartHere(cmdLine, detector);
        break;

    default:
        std::cerr << "Command selected from command line arguments not implemented: " << static_cast<int>(cmdLine.command) << std::endl;
        return -1;
    }

    return 0;
}
