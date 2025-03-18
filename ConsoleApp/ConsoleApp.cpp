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
#include <cwctype>

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

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

std::wstring EscapeIfNeeded(std::wstring s)
{
    bool hasQuot = std::find(s.begin(), s.end(), L'"') != s.end();
    bool hasSpace = std::find_if(s.begin(), s.end(), iswspace) != s.end();
    if (!hasQuot && !hasSpace)
    {
        return s;
    }

    std::wstring t;
    t.reserve(s.size() + 2);
    t.push_back(L'\"');
    for (wchar_t c : s)
    {
        if (c == L'\"')
        {
            t.push_back(L'\\');
        }
        t.push_back(c);
    }
    t.push_back(L'\"');

    return t;
}

void StartHere(CmdLine const& cmdLine, openhere::fileexplorerdetector::FileExplorerDetector const& detector)
{
    std::wstring cmd{ EscapeIfNeeded(cmdLine.path) };
    for (std::wstring const& a : cmdLine.addArgs)
    {
        cmd += L" ";
        cmd += EscapeIfNeeded(a);
    }

    wchar_t const* curDir = NULL;
    auto& i = detector.GetInstances();
    if (!i.empty())
    {
        auto& p = i[0].GetOpenPaths();
        if (!p.empty())
        {
            curDir = p[0].c_str();
        }
    }

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    BOOL succ = CreateProcessW(
        NULL, // app; use NULL and specify app as first arg in `cmdline` to trigger search behavior
        const_cast<wchar_t*>(cmd.c_str()), // cmdline
        NULL, // procAttr
        NULL, // threadAttr
        FALSE, // inheritHandles
        0, // creationFlags
        NULL, // env
        curDir, // curDir
        &si, // startupInfo
        &pi); // processInfo

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
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
