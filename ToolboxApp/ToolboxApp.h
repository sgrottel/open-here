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

#include "pch.h"
#include "MainWindow.h"

#include <future>

class ToolboxApp {
public:
	static constexpr const wchar_t* AppName = L"Open Here";

	ToolboxApp(HINSTANCE hInst);
	~ToolboxApp();

	inline HINSTANCE GetHInstance() const { return m_hInst; }

	bool InitWindow(int cmdShow);

	int RunMainLoop();

	inline HMONITOR GetMainMonitor() const { return m_mainMonitor; }
	RECT GetMainMonitorWorkArea() const;

	inline double GetTextScale() const { return m_textScale; }

private:
	HINSTANCE m_hInst;
	MainWindow m_mainWnd;
	HMONITOR m_mainMonitor;
	double m_textScale{ 1.0 };
	std::shared_future<void> m_explorerDetector;
	std::shared_future<void> m_configLoader;
	std::shared_future<void> m_startFinalizer;
};
