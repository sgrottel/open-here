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
#include "MainWindow.h"
#include "ToolboxApp.h"
#include "Toolbox/ToolRunner.h"
#include "BringHWndToFront.h"

#include <filesystem>

#include <dwmapi.h>
#include <Windowsx.h>
#include <Winuser.h>
#include <shellapi.h>

namespace
{
	constexpr uint64_t const MAGIC_PROXY_ID = 0x269bf21ab0876a01ull;
	constexpr wchar_t const* const WINDOW_CLASS_NAME = L"sgrOpenHereToolboxWnd";
	constexpr uintptr_t const EXIT_DEBOUNCE_TIMER_ID = 0x0815;
	constexpr uintptr_t const INIT_CHECK_TIMER_ID = 0x0816;
}


MainWindow::MainWindow(ToolboxApp& app)
	: m_app{ app }
{
	// Note: `app` is not fully initialized yet!
}


MainWindow:: ~MainWindow()
{
	m_proxy.magicNumber = 0;
	m_proxy.wnd = nullptr;

	DeleteObject(m_font);
}


bool MainWindow::Init(int cmdShow)
{
	if (m_proxy.magicNumber == MAGIC_PROXY_ID)
	{
		throw std::logic_error("ToolboxApp::Init must only be called one time.");
	}

	RegisterWindowClass();

	NONCLIENTMETRICSW ncMetrics;
	ZeroMemory(&ncMetrics, sizeof(NONCLIENTMETRICSW));
	ncMetrics.cbSize = sizeof(NONCLIENTMETRICSW);
	BOOL rv = SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICSW), static_cast<PVOID>(&ncMetrics), 0);
	m_font = CreateFontIndirectW(&(ncMetrics.lfMessageFont));

	m_hWnd = CreateWindowW(
		WINDOW_CLASS_NAME,
		ToolboxApp::AppName,
		WS_BORDER | WS_POPUP,
		0, 0, 400, 300, // temporary, will be finalized later
		nullptr, // no parent
		nullptr, // no menu
		m_app.GetHInstance(),
		nullptr);
	if (!m_hWnd)
	{
		throw std::runtime_error("Failed to create main window");
	}

	m_wndDpi = GetDpiForWindow(m_hWnd);
	RECT mainDisplayWorkingArea{ m_app.GetMainMonitorWorkArea() };

	SIZE wndSize = ComputeLayout(mainDisplayWorkingArea);

	unsigned int left = mainDisplayWorkingArea.left
		+ (mainDisplayWorkingArea.right - mainDisplayWorkingArea.left - wndSize.cx) / 2;
	unsigned int top = mainDisplayWorkingArea.top
		+ (mainDisplayWorkingArea.bottom - mainDisplayWorkingArea.top - wndSize.cy) / 2;
	SetWindowPos(m_hWnd, NULL, left, top, wndSize.cx, wndSize.cy, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);

	m_proxy.wnd = this;
	m_proxy.magicNumber = MAGIC_PROXY_ID;
	SetWindowLongPtr(m_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&m_proxy));

	DWM_WINDOW_CORNER_PREFERENCE cornerPreference = DWMWCP_ROUND;
	DwmSetWindowAttribute(m_hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPreference, sizeof(DWM_WINDOW_CORNER_PREFERENCE));

//#define DWMWA_SYSTEMBACKDROP_TYPE 38
//#define DWMSBT_TRANSIENTWINDOW 3
//	int Backdrop_Type = DWMSBT_TRANSIENTWINDOW;
//	DwmSetWindowAttribute(m_hWnd, DWMWA_SYSTEMBACKDROP_TYPE, &Backdrop_Type, sizeof(int));

	ShowWindow(m_hWnd, cmdShow);
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	UpdateWindow(m_hWnd);

	return true;
}


void MainWindow::SetFileExplorerInstanceInfo(openhere::fileexplorerdetector::InstanceInfo const* info)
{
	m_path.clear();
	m_files.clear();

	if (info != nullptr)
	{
		auto const& paths = info->GetOpenPaths();
		if (paths.empty())
		{
			m_labels[m_pathLabelIndex].txt = L"No file system path opened.";
		}
		else
		{
			m_labels[m_pathLabelIndex].txt = m_path = paths[0];
		}

		auto const& selItems = info->GetSelectedItems();
		if (selItems.empty())
		{
			m_labels[m_itemsLabelIndex].txt = L"No file system items selected.";
		}
		else
		{
			m_files.reserve(selItems.size());
			for (auto const& p : selItems)
			{
				if (!std::filesystem::is_regular_file(p)) continue;
				m_files.push_back(p);
			}

			std::wstringstream files;
			if (m_files.empty())
			{
				files << L"No files selected.";
			}
			else if (m_files.size() == 1)
			{
				files << L"1 file selected: ";
			}
			else
			{
				files << m_files.size() << L" files selected: ";
			}
			bool first = true;
			for (auto const& p : m_files)
			{
				if (first)
				{
					first = false;
				}
				else
				{
					files << L", ";
				}

				files << std::filesystem::path{ p }.filename().wstring();

			}

			m_labels[m_itemsLabelIndex].txt = files.str();
		}

		try
		{
			HBITMAP bmp = m_bitmapFactory.LoadExplorerIcon(info->GetInstanceType(), m_path, m_bitmaps[m_explorerBitmapIndex].rect);
			if (bmp)
			{
				HBITMAP old = m_bitmaps[m_explorerBitmapIndex].hBmp;
				m_bitmaps[m_explorerBitmapIndex].hBmp = bmp;
				DeleteObject(old);
			}
		}
		catch(...) {}

	}
	else
	{
		m_labels[m_pathLabelIndex].txt = L"No file explorer detected.";
		m_labels[m_itemsLabelIndex].txt = L"";
	}

	InvalidateRect(m_hWnd, NULL, TRUE);
}


void MainWindow::SetToolInfos(std::vector<openhere::toolbox::ToolInfo> const& tools)
{
	size_t len = std::min<size_t>(tools.size(), toolCount);
	for (size_t i = 0; i < len; ++i)
	{
		m_tools[i] = tools[i];

		m_labels[m_buttonTitleLabelIndex[i]].txt = m_tools[i].GetTitle();
		if (m_labels[m_buttonTitleLabelIndex[i]].txt.empty())
		{
			// empty title -> spacer
			continue;
		}
		m_labels[m_buttonTitleLabelIndex[i]].visible = true;
		m_labels[m_buttonTitleLabelIndex[i]].enabled = false;

		m_bitmaps[m_buttonBitmapIndex[i]].enabled = false;
		m_bitmapFactory.Async(
			m_bitmaps[m_buttonBitmapIndex[i]].rect,
			[i](BitmapFactory& bf, RECT const& rect, unsigned int idx, void* ctxt) -> HBITMAP {
				MainWindow* that = static_cast<MainWindow*>(ctxt);
				return bf.LoadFromIconFile(
					that->m_tools[i].GetIconFile().c_str(),
					that->m_tools[i].GetIconID(),
					rect);
			},
			&MainWindow::SetBitmapImage,
			static_cast<unsigned int>(m_buttonBitmapIndex[i]),
			static_cast<void*>(this));

	}

	InvalidateRect(m_hWnd, NULL, true);
}


void MainWindow::SetConfig(openhere::toolbox::Config const& config)
{
	m_config = config;
}


void MainWindow::FinializeSetup()
{
	if (m_config.GetPlayStartSound())
	{
		MessageBeep(MB_OK);
	}

	// Now file explorer detection and configuration loading are completed.
	using openhere::toolbox::ToolInfo;
	openhere::toolbox::ToolRunner runner;

	// Currently all are disabled. Enable if all requirements are met
	for (size_t i = 0; i < toolCount; ++i)
	{
		std::wstring err;
		if (m_tools[i].GetTitle().empty()) continue;

		switch (m_tools[i].GetPathRequirement())
		{
		case ToolInfo::PathRequirement::DontCare:
			break;
		case ToolInfo::PathRequirement::Required:
			if (m_path.empty())
			{
				err = L"Path required";
			}
			break;
		case ToolInfo::PathRequirement::RequiredNull:
			if (!m_path.empty())
			{
				err = L"Path forbidden";
			}
			break;
		default:
			throw std::logic_error("Unhandled configuration");
		}

		switch (m_tools[i].GetFilesRequirement())
		{
		case ToolInfo::FilesRequirement::DontCare:
			break;
		case ToolInfo::FilesRequirement::RequireOne:
			if (m_files.size() != 1)
			{
				err = L"Exactly one file must be selected";
			}
			break;
		case ToolInfo::FilesRequirement::RequireOneOrMore:
			if (m_files.size() < 1)
			{
				err = L"At least one file must be selected";
			}
			break;
		case ToolInfo::FilesRequirement::RequireTwoOrMore:
			if (m_files.size() < 2)
			{
				err = L"Exactly two files must be selected";
			}
			break;
		case ToolInfo::FilesRequirement::RequireNull:
			if (m_files.size() > 1)
			{
				err = L"No file must be selected";
			}
			break;
		default:
			throw std::logic_error("Unhandled configuration");
		}

		if (err.empty())
		{
			try
			{
				if (!runner.CanStart(m_tools[i], m_path, m_files, err))
				{
					if (err.empty())
					{
						err = L"Cannot be started";
					}
				}
			}
			catch (std::exception& ex)
			{
				err = (std::wstringstream{} << ex.what()).str();
			}
			catch (...)
			{
				err = L"Unknown exception";
			}
		}

		if (err.empty())
		{
			// enable button
			m_labels[m_buttonTitleLabelIndex[i]].enabled = true;
			m_labels[m_buttonKeyLabelIndex[i]].visible = true;
			m_bitmaps[m_buttonBitmapIndex[i]].enabled = true;
			m_clickRects[m_buttonClickRectIndex[i]].enabled = true;
		}
		else
		{
			// stay disabled, and add error message
			m_labels[m_buttonTitleLabelIndex[i]].txt = L"- " + err + L" -\n" + m_labels[m_buttonTitleLabelIndex[i]].txt;
		}
	}

	InvalidateRect(m_hWnd, NULL, true);

	SetForegroundWindow(m_hWnd);
	BringWindowToTop(m_hWnd);
	ShowWindow(m_hWnd, SW_SHOWNORMAL);

	SetTimer(m_hWnd, INIT_CHECK_TIMER_ID, 250, NULL);

}


SIZE MainWindow::ComputeLayout(RECT const& desktop)
{
	const wchar_t escLabel[] = L"Esc";
	unsigned int escLabelWidth = 0;
	const wchar_t settingsLabel[] = L"Settings (Home)";
	unsigned int settingsLabelWidth = 0;

	unsigned int grid = 16;
	{
		HDC dc = CreateCompatibleDC(NULL);
		SelectObject(dc, m_font);

		TEXTMETRICW tm;
		BOOL rv = GetTextMetricsW(dc, &tm);
		if (rv)
		{
			grid = tm.tmHeight;
		}

		SIZE s;
		const int escLen = (sizeof(escLabel) / sizeof(wchar_t)) - 1;
		rv = GetTextExtentPoint32W(dc, escLabel, escLen, &s);
		if (rv)
		{
			escLabelWidth = s.cx / grid;
			if (s.cx % grid) escLabelWidth++;
			escLabelWidth *= grid;
		}

		const int settingsLen = (sizeof(settingsLabel) / sizeof(wchar_t)) - 1;
		rv = GetTextExtentPoint32W(dc, settingsLabel, settingsLen, &s);
		if (rv)
		{
			settingsLabelWidth = s.cx / grid;
			if (s.cx % grid) settingsLabelWidth++;
			settingsLabelWidth *= grid;
		}

		DeleteDC(dc);
	}

	unsigned int halfGrid = grid / 2;

	unsigned int iconSize = 4 * grid;
	unsigned int tileHeight = iconSize + 2 * grid;
	unsigned int tileWidth = tileHeight * 2;

	unsigned int width = 4 * tileWidth + 7 * grid;
	unsigned int height = 3 * tileHeight + (4 + 4 + 3) * grid;

	// path
	m_labels.push_back({
		L"... Initializing ...",
		RECT(3 * grid, grid, width - 6 * grid, 2 * grid),
		DT_LEFT | DT_PATH_ELLIPSIS | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX
		});
	m_pathLabelIndex = m_labels.size() - 1;
	// files
	m_labels.push_back({
		L"",
		RECT(3 * grid, 2 * grid + halfGrid, width - grid, 3 * grid + halfGrid),
		DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS | DT_NOPREFIX
		});
	m_itemsLabelIndex = m_labels.size() - 1;

	// explorer icon
	m_bitmaps.push_back({
		RECT(halfGrid, halfGrid, 2 * grid + halfGrid, 2 * grid + halfGrid),
		NULL
		});
	m_explorerBitmapIndex = m_bitmaps.size() - 1;

	// exit button
	m_clickRects.push_back(ClickRect{ 
		true,
		RECT(width - (3 * grid + escLabelWidth), 0, width, grid + halfGrid),
		VK_ESCAPE
		});
	m_bitmaps.push_back({
		RECT(width - 3 * grid, 0, width, grid + halfGrid),
		NULL
		});
	m_bitmaps.back().hBmp = m_bitmapFactory.MakeExitButton(m_bitmaps.back().rect, m_colors);
	m_labels.push_back({
		escLabel,
		RECT(width - (3 * grid + escLabelWidth), 0, width - 3 * grid, grid + halfGrid),
		DT_CENTER | DT_SINGLELINE | DT_VCENTER
		});

	for (int x = 0; x < 4; ++x)
	{
		int tx = (x + 2) * grid + x * tileWidth;
		for (int y = 0; y < 3; ++y)
		{
			int ty = (5 + y) * grid + y * tileHeight;

			m_clickRects.push_back(ClickRect{
				false,
				RECT(tx, ty, tx + tileWidth, ty + tileHeight),
				static_cast<UINT_PTR>(VK_F1 + x + y * 4)
				}); // bounds
			m_buttonClickRectIndex[x + y * 4] = m_clickRects.size() - 1;

			// icon
			m_bitmaps.push_back({
				RECT(tx + (tileWidth - iconSize) / 2, ty, tx + (tileWidth + iconSize) / 2, ty + iconSize),
				NULL
				});
			m_buttonBitmapIndex[x + y * 4] = m_bitmaps.size() - 1;

			// title
			m_labels.push_back({
				L"",
				RECT(tx, ty + iconSize + (tileHeight - iconSize - 2 * grid) / 2, tx + tileWidth, ty + iconSize + (tileHeight - iconSize + 2 * grid) / 2),
				DT_CENTER | DT_HIDEPREFIX | DT_WORDBREAK,
				false
			});
			m_buttonTitleLabelIndex[x + y * 4] = m_labels.size() - 1;

			// F-key label
			m_labels.push_back({
				L"F" + std::to_wstring(1 + x + y * 4),
				RECT(tx, ty, tx + (tileWidth - iconSize) / 2, ty + grid),
				DT_CENTER | DT_SINGLELINE | DT_VCENTER,
				false
				});
			m_buttonKeyLabelIndex[x + y * 4] = m_labels.size() - 1;
		}
	}

	// settings button
	m_clickRects.push_back(ClickRect{
		true,
		RECT(grid,
			4 * grid + 3 * tileHeight + 3 * grid + halfGrid + 1 * grid,
			2 * grid + halfGrid + settingsLabelWidth,
			4 * grid + 3 * tileHeight + 3 * grid + halfGrid + 2 * grid + halfGrid),
		VK_HOME
		});
	m_bitmaps.push_back({
		RECT(grid,
		4 * grid + 3 * tileHeight + 3 * grid + halfGrid + 1 * grid + halfGrid/2,
		2 * grid,
		4 * grid + 3 * tileHeight + 3 * grid + halfGrid + 2 * grid + halfGrid / 2),
		NULL
		});
	size_t settingsImageIndex = m_bitmaps.size() - 1;
	m_labels.push_back({
		settingsLabel,
		RECT(2 * grid,
			4 * grid + 3 * tileHeight + 3 * grid + halfGrid + 1 * grid,
			2 * grid + halfGrid + settingsLabelWidth,
			4 * grid + 3 * tileHeight + 3 * grid + halfGrid + 2 * grid + halfGrid),
		DT_CENTER | DT_SINGLELINE | DT_VCENTER
		});

	m_bitmapFactory.Async(
		m_bitmaps[m_explorerBitmapIndex].rect,
		[](BitmapFactory& bf, RECT const& rect, unsigned int idx, void* ctxt) -> HBITMAP {
			MainWindow* that = static_cast<MainWindow*>(ctxt);
			return bf.LoadFromIcon(that->m_app.GetHInstance(), MAKEINTRESOURCEW(100), rect);
		},
		& MainWindow::SetBitmapImage,
		static_cast<unsigned int>(m_explorerBitmapIndex),
		static_cast<void*>(this));

	m_bitmapFactory.Async(
		m_bitmaps[settingsImageIndex].rect,
		[](BitmapFactory& bf, RECT const& rect, unsigned int idx, void* ctxt) -> HBITMAP {
			MainWindow* that = static_cast<MainWindow*>(ctxt);
			HBITMAP hBmp = bf.LoadFromIcon(that->m_app.GetHInstance(), MAKEINTRESOURCEW(1001), rect);
			bf.MultiplyColor(hBmp, that->m_colors.GetForegroundColor());
			return hBmp;
		},
		&MainWindow::SetBitmapImage,
		static_cast<unsigned int>(settingsImageIndex),
		static_cast<void*>(this));

	return SIZE{ static_cast<long>(width), static_cast<long>(height) };
}


LRESULT CALLBACK MainWindow::StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MainWindow::InstProxy* proxy = reinterpret_cast<MainWindow::InstProxy*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if ((proxy != nullptr)
		&& (proxy->magicNumber == MAGIC_PROXY_ID)
		&& (proxy->wnd->m_hWnd == hWnd))
	{
		return proxy->wnd->WndProc(message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}


void MainWindow::SetBitmapImage(HBITMAP hBmp, unsigned int idx, void* ctxt)
{
	MainWindow* that = static_cast<MainWindow*>(ctxt);
	that->m_bitmaps[idx].hBmp = hBmp;
	InvalidateRect(that->m_hWnd, NULL, TRUE);
}


void MainWindow::RegisterWindowClass()
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = MainWindow::StaticWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_app.GetHInstance();
	wcex.hIcon = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(100));
	wcex.hCursor = NULL;
	wcex.hbrBackground = CreateSolidBrush(m_colors.GetBackgroundColor());
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = WINDOW_CLASS_NAME;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(100));

	// no need for error checking here. If this fails, CreateWindow will fail subsequently.
	RegisterClassExW(&wcex);
}


LRESULT MainWindow::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ACTIVATEAPP:
		// Automatically close the window, when it looses the activation status for some time.
		if (wParam)
		{
			// activated, stop kill timer
			KillTimer(m_hWnd, EXIT_DEBOUNCE_TIMER_ID);
		}
		else
		{
			// deactivated, start debounce kill timer.
			SetTimer(m_hWnd, EXIT_DEBOUNCE_TIMER_ID, m_autoCloseMs, NULL);
		}
		break;
	case WM_TIMER:
		if (wParam == INIT_CHECK_TIMER_ID)
		{
			KillTimer(m_hWnd, INIT_CHECK_TIMER_ID);
			BringHWndToFront(m_hWnd, FALSE);
		}
		else if (wParam == EXIT_DEBOUNCE_TIMER_ID)
		{
			KillTimer(m_hWnd, EXIT_DEBOUNCE_TIMER_ID);
			// close main window & app after timeout
			PostQuitMessage(0);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;

		case VK_HOME:
			OpenSettings();
			PostQuitMessage(0);
			break;

		case VK_F1: // no break;
		case VK_F2: // no break;
		case VK_F3: // no break;
		case VK_F4: // no break;
		case VK_F5: // no break;
		case VK_F6: // no break;
		case VK_F7: // no break;
		case VK_F8: // no break;
		case VK_F9: // no break;
		case VK_F10: // no break;
		case VK_F11: // no break;
		case VK_F12:
		{
			openhere::toolbox::ToolRunner runner;
			openhere::toolbox::ToolInfo const& tool = m_tools[wParam - VK_F1];

			if (tool.GetTitle().empty())
			{
				// spacer -> no action
				break;
			}

			try
			{
				openhere::toolbox::ToolInfo::StartConfig const* selCfg = runner.SelectStartConfig(tool, m_path, m_files, true);
				if (selCfg == nullptr) throw std::runtime_error("No start configuration selected");
				openhere::toolbox::ToolInfo::StartConfig cfg{ *selCfg };
				runner.ApplyVariables(cfg, m_path, m_files);
				if (!runner.Start(cfg)) throw std::runtime_error("generic start call failure");
				PostQuitMessage(0);
			}
			catch(std::exception const& ex)
			{
				std::wstring msg{ (std::wstringstream{} << L"Failed to started \"" << tool.GetTitle() << L"\": " << ex.what()).str() };
				MessageBoxW(m_hWnd, msg.c_str(), ToolboxApp::AppName, MB_ICONERROR | MB_OK);
			}
			catch (...)
			{
				std::wstring msg{ (std::wstringstream{} << L"Failed to started \"" << tool.GetTitle() << L"\": unknown error").str() };
				MessageBoxW(m_hWnd, msg.c_str(), ToolboxApp::AppName, MB_ICONERROR | MB_OK);
			}
		}
			break;
		}
		break;
		
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(m_hWnd, &ps);
		BLENDFUNCTION bf;
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.AlphaFormat = AC_SRC_ALPHA;
		bf.SourceConstantAlpha = 255;

		SetBkColor(ps.hdc, m_colors.GetBackgroundColor());
		SetBkMode(ps.hdc, TRANSPARENT);
		SelectObject(ps.hdc, m_font);

		for (auto const& b : m_bitmaps)
		{
			if (!b.hBmp) continue;

			bf.SourceConstantAlpha = b.enabled ? 255 : 63;

			HDC hDCBits = CreateCompatibleDC(ps.hdc);
			BITMAP Bitmap;
			GetObject(b.hBmp, sizeof(BITMAP), (LPSTR)&Bitmap);
			SelectObject(hDCBits, b.hBmp);
			AlphaBlend(ps.hdc, b.rect.left, b.rect.top, Bitmap.bmWidth, Bitmap.bmHeight, hDCBits, 0, 0, Bitmap.bmWidth, Bitmap.bmHeight, bf);
			DeleteDC(hDCBits);
		}

		for (auto const& l : m_labels)
		{
			if (!l.visible)
			{
				continue;
			}
			RECT r = l.rect;
			SetTextColor(ps.hdc, 
				l.enabled
				? m_colors.GetForegroundColor()
				: m_colors.GetDisabledForegroundColor()
			);
			DrawTextW(ps.hdc, l.txt.c_str(), static_cast<int>(l.txt.size()), &r, l.format);
		}

		EndPaint(m_hWnd, &ps);
	}
	break;

	case WM_NCMOUSEMOVE: // no break;
	case WM_MOUSEMOVE:
	{
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		bool clickCursor = false;
		if (m_clickingRect != nullptr)
		{
			RECT const& r = m_clickingRect->rect;
			if ((r.left < xPos) && (xPos < r.right)
				&& (r.top < yPos) && (yPos < r.bottom))
			{
				clickCursor = true;
			}
		}
		else
		{
			for (auto const& cr : m_clickRects)
			{
				if (!cr.enabled) continue;
				RECT const& r = cr.rect;
				if ((r.left < xPos) && (xPos < r.right)
					&& (r.top < yPos) && (yPos < r.bottom))
				{
					clickCursor = true;
					break;
				}
			}
		}

		if (clickCursor != m_clickCursor)
		{
			m_clickCursor = clickCursor;
			SetCursor(LoadCursor(NULL, m_clickCursor ? IDC_HAND : IDC_ARROW));
		}

	}
	break;

	case WM_LBUTTONDOWN: // no break;
	case WM_RBUTTONDOWN: // no break;
	case WM_MBUTTONDOWN:
	{
		SetCapture(m_hWnd);

		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		for (auto const& cr : m_clickRects)
		{
			if (!cr.enabled) continue;
			RECT const& r = cr.rect;
			if ((r.left < xPos) && (xPos < r.right)
				&& (r.top < yPos) && (yPos < r.bottom))
			{
				m_clickingRect = &cr;
				break;
			}
		}
	}
	break;

	case WM_LBUTTONUP: // no break;
	case WM_RBUTTONUP: // no break;
	case WM_MBUTTONUP:
	{
		ReleaseCapture();

		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		if (m_clickingRect != nullptr)
		{
			RECT const& r = m_clickingRect->rect;
			if ((r.left < xPos) && (xPos < r.right)
				&& (r.top < yPos) && (yPos < r.bottom))
			{
				PostMessage(m_hWnd, WM_KEYDOWN, m_clickingRect->keyCode, NULL);
			}
			m_clickingRect = nullptr;
		}
	}
	break;

	default:
		return DefWindowProc(m_hWnd, message, wParam, lParam);
	}
	return 0;
}


void MainWindow::OpenSettings()
{

	// TODO: Implement calling the settings app

	// open settings folder as fallback
	ShellExecuteW(NULL, NULL, openhere::toolbox::Config::Path().c_str(), NULL, NULL, SW_SHOWNORMAL);
}
