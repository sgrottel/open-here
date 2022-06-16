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
#include "pch.h"
#include "ToolboxApp.h"

#include "FileExplorerDetector/FileExplorerDetector.h"
#include "Toolbox/ToolInfoListLoader.h"
#include "Toolbox/Config.h"

#include <sstream>

#include <winrt/Windows.UI.ViewManagement.h>

namespace
{
    BOOL CALLBACK GetMainMonitorCallback(HMONITOR hMon, HDC, LPRECT, LPARAM outHandleArg)
    {
        HMONITOR* outHandle = reinterpret_cast<HMONITOR*>(outHandleArg);

        MONITORINFO info{ sizeof(MONITORINFO) };
        if (!GetMonitorInfo(hMon, &info)) return TRUE;

        if ((info.dwFlags & MONITORINFOF_PRIMARY) == 0) return TRUE;

        *outHandle = hMon;
        return FALSE;
    }

}


/// <summary>
/// Application main entry point
/// </summary>
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    try
    {
        ToolboxApp app{ hInstance };

        if (!app.InitWindow(nCmdShow))
        {
            throw std::runtime_error("Failed to initialize app due to unknown reason.");
        }

        return app.RunMainLoop();
    }
    catch (std::exception const& ex)
    {
        std::wstringstream msg;
        msg << L"Unrecoverable error: " << ex.what();
        MessageBoxW(NULL, msg.str().c_str(), ToolboxApp::AppName, MB_OK | MB_ICONERROR);
    }
    catch (...)
    {
        MessageBoxW(NULL, L"Unrecoverable error: unknown error", ToolboxApp::AppName, MB_OK | MB_ICONERROR);
    }
    return -1;
}


ToolboxApp::ToolboxApp(HINSTANCE hInst)
	: m_hInst{ hInst }, m_mainWnd{ *this }, m_mainMonitor{ NULL }
{

    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    EnumDisplayMonitors(NULL, NULL, GetMainMonitorCallback, reinterpret_cast<LPARAM>(&m_mainMonitor));

    {
        winrt::Windows::UI::ViewManagement::UISettings const ui_settings{};
        m_textScale = ui_settings.TextScaleFactor();
    }

}


ToolboxApp::~ToolboxApp()
{
}


bool ToolboxApp::InitWindow(int cmdShow)
{
    bool rv = m_mainWnd.Init(cmdShow);
    if (rv)
    {

        m_explorerDetector = std::async(
            std::launch::async,
            [&]()
            {
                try
                {
                    openhere::fileexplorerdetector::FileExplorerDetector d;
                    int r = d.Detect();
                    if (r < 0)
                    {
                        throw std::runtime_error("Unspecific error running file explorer detection");
                    }
                    else if (r == 0)
                    {
                        // no file explorer found
                        m_mainWnd.SetFileExplorerInstanceInfo(nullptr);
                    }
                    else if (r > 0)
                    {
                        // found
                        m_mainWnd.SetFileExplorerInstanceInfo(&d.GetInstances()[0]);
                    }
                }
                catch (...)
                {
                    // error
                    try
                    {
                        m_mainWnd.SetFileExplorerInstanceInfo(nullptr);
                    }
                    catch(...) {}
                }
            }
        ).share();

        m_configLoader = std::async(
            std::launch::async,
            [&]()
            {
                try
                {
                    openhere::toolbox::Config config;
                    config.Load();
                    m_mainWnd.SetConfig(config);

                    openhere::toolbox::ToolInfoListLoader loader;

                    std::vector<openhere::toolbox::ToolInfo> tools{ std::move(loader.Load()) };

                    m_mainWnd.SetToolInfos(tools);
                }
                catch (std::exception& ex)
                {
                    OutputDebugStringA((std::string(ex.what()) + "\n").c_str());
                    try
                    {
                        m_mainWnd.SetToolInfos({});
                    }
                    catch (...) {}
                }
                catch (...)
                {
                    try
                    {
                        m_mainWnd.SetToolInfos({});
                    }
                    catch (...) {}
                }
            }
        ).share();

        m_startFinalizer = std::async(
            std::launch::async,
            [&]() {
                m_explorerDetector.wait();
                m_configLoader.wait();
                try
                {
                    m_mainWnd.FinializeSetup();
                }
                catch (...) { }
            }
        ).share();

    }
    return rv;
}


int ToolboxApp::RunMainLoop()
{
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}


RECT ToolboxApp::GetMainMonitorWorkArea() const
{
    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    if (!::GetWindowRect(hDesktop, &desktop))
    {
        desktop.left = 0;
        desktop.top = 0;
        desktop.right = 1280;
        desktop.bottom = 720;
    }

    if (m_mainMonitor == NULL) return desktop;

    MONITORINFO info{ sizeof(MONITORINFO) };
    if (!GetMonitorInfo(m_mainMonitor, &info)) return desktop;

    return info.rcWork;
}
