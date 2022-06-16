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

#include "BitmapFactory.h"
#include "ColorFactory.h"

#include "FileExplorerDetector/InstanceInfo.h"
#include "Toolbox/ToolInfo.h"
#include "Toolbox/Config.h"

#include <vector>
#include <array>

class ToolboxApp;

class MainWindow
{
public:
	MainWindow(ToolboxApp& app);
	~MainWindow();

	bool Init(int cmdShow);

	void SetFileExplorerInstanceInfo(openhere::fileexplorerdetector::InstanceInfo const* info);
	void SetToolInfos(std::vector<openhere::toolbox::ToolInfo> const& tools);
	void SetConfig(openhere::toolbox::Config const& config);
	void FinializeSetup();

private:
	constexpr static unsigned int const toolCount{ 12u };
	constexpr static uint32_t const baseDpi{ 96u };

	SIZE ComputeLayout(RECT const& desktop);

	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static void SetBitmapImage(HBITMAP hBmp, unsigned int idx, void* ctxt);

	struct InstProxy {
		uint64_t magicNumber{ 0 };
		MainWindow* wnd{ nullptr };
	};

	void RegisterWindowClass();

	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);

	void OpenSettings();

	HWND m_hWnd { NULL };
	InstProxy m_proxy;
	ToolboxApp& m_app;

	unsigned int m_autoCloseMs{ 500 };
	BitmapFactory m_bitmapFactory;
	ColorFactory m_colors;

	uint32_t m_wndDpi{ baseDpi };

	HFONT m_font{ NULL };
	struct Label {
		std::wstring txt;
		RECT rect;
		unsigned int format;
		bool visible{ true };
		bool enabled{ true };
	};
	std::vector<Label> m_labels;
	size_t m_pathLabelIndex{ 0 };
	size_t m_itemsLabelIndex{ 0 };
	std::array<size_t, toolCount> m_buttonTitleLabelIndex;
	std::array<size_t, toolCount> m_buttonKeyLabelIndex;

	bool m_clickCursor{ false };
	struct ClickRect {
		bool enabled;
		RECT rect;
		UINT_PTR keyCode;
	};
	std::vector<ClickRect> m_clickRects;
	std::array<size_t, toolCount> m_buttonClickRectIndex;
	ClickRect const* m_clickingRect{ nullptr };

	struct Bitmap {
		RECT rect;
		HBITMAP hBmp;
		bool enabled{ true };
	};
	std::vector<Bitmap> m_bitmaps;
	size_t m_explorerBitmapIndex{ 0 };
	std::array<size_t, toolCount> m_buttonBitmapIndex;

	std::wstring m_path;
	std::vector<std::wstring> m_files;

	std::array<openhere::toolbox::ToolInfo, toolCount> m_tools;
	openhere::toolbox::Config m_config;
};
