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

#include "CoInitialize.h"

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Objbase.h>
#include <shlobj.h>
#include <atlcomcli.h>  // for COM smart pointers
#include <system_error>
#include <algorithm>

using namespace openhere::fileexplorerdetector;

namespace {

	// Throw a std::system_error if the HRESULT indicates failure.
	template< typename T >
	void ThrowIfFailed(HRESULT hr, T&& msg) {
		if (FAILED(hr)) {
			throw std::system_error{ hr, std::system_category(), std::forward<T>(msg) };
		}
	}

}

int FileExplorerDetector::Detect()
{
	if (!coInitialize) {
		coInitialize = std::make_shared<CoInitialize>();
	}

	instances.clear();

	CComPtr<IShellWindows> pshWindows;
	ThrowIfFailed(
		pshWindows.CoCreateInstance(CLSID_ShellWindows),
		"Could not create instance of IShellWindows");

	long count = 0;
	ThrowIfFailed(
		pshWindows->get_Count(&count),
		"Could not get number of shell windows");

	instances.reserve(count);

	for (long i = 0; i < count; ++i) {
		InstanceInfo info;
		info.SetInstanceType(L"explorer.exe");

		CComVariant vi{ i };
		CComPtr<IDispatch> pDisp;
		ThrowIfFailed(
			pshWindows->Item(vi, &pDisp),
			"Could not get item from IShellWindows");
		if (!pDisp) {
			continue; // Skip - this shell window was registered with a NULL IDispatch
		}

		CComQIPtr<IWebBrowserApp> pApp{ pDisp };
		if (!pApp) {
			continue; // This window doesn't implement IWebBrowserApp
		}

		// Get the window handle.
		HWND hwnd;
		pApp->get_HWND(reinterpret_cast<SHANDLE_PTR*>(&hwnd));
		info.SetHWnd(reinterpret_cast<intptr_t>(hwnd));

		CComQIPtr<IServiceProvider> psp{ pApp };
		if (!psp) {
			continue; // This window doesn't implement IServiceProvider
		}

		CComPtr<IShellBrowser> pBrowser;
		if (FAILED(psp->QueryService(SID_STopLevelBrowser, &pBrowser))) {
			continue; // This window doesn't provide IShellBrowser
		}

		CComPtr<IShellView> pShellView;
		if (FAILED(pBrowser->QueryActiveShellView(&pShellView))) {
			continue; // For some reason there is no active shell view
		}

		CComPtr<IShellItemArray> pShellItemArray;
		if (SUCCEEDED(pShellView->GetItemObject(SVGIO_SELECTION, IID_IShellItemArray, (void**)&pShellItemArray))) {
			DWORD cnt = 0;
			if (FAILED(pShellItemArray->GetCount(&cnt))) {
				cnt = 0;
			}

			info.AccessSelectedItems().reserve(cnt);
			for (DWORD idx = 0; idx < cnt; ++idx) {
				CComPtr<IShellItem> pShellItem;
				if (SUCCEEDED(pShellItemArray->GetItemAt(idx, &pShellItem))) {
					wchar_t* path = nullptr;
					if (SUCCEEDED(pShellItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &path))) {
						info.AccessSelectedItems().push_back(path);
						CoTaskMemFree(path);
					}
				}
			}
		}

		CComQIPtr<IFolderView> pFolderView{ pShellView };
		if (!pFolderView) {
			continue; // The shell view doesn't implement IFolderView
		}

		// Get the interface from which we can finally query the PIDL of the current folder.
		CComPtr<IPersistFolder2> pFolder;
		if (FAILED(pFolderView->GetFolder(IID_IPersistFolder2, (void**)&pFolder))) {
			continue;
		}

		LPITEMIDLIST pidl = nullptr;
		if (SUCCEEDED(pFolder->GetCurFolder(&pidl)))
		{
			std::vector<wchar_t> path(32767, L'\0');
			if (::SHGetPathFromIDListEx(pidl, path.data(), static_cast<DWORD>(path.size()), 0)) {
				info.AccessOpenPaths().push_back(path.data());
			}
			::CoTaskMemFree(pidl);
		}

		instances.push_back(info);
	}

	addWindowsInfo();

	std::sort(instances.begin(), instances.end(), [](InstanceInfo const& a, InstanceInfo const& b) { return a.GetZDepth() < b.GetZDepth(); });

	return static_cast<int>(instances.size());
}

std::vector<InstanceInfo> const& FileExplorerDetector::GetInstances() const
{
	return instances;
}

void FileExplorerDetector::addWindowsInfo()
{
	HWND fgW = GetForegroundWindow();
	HWND tW = GetTopWindow(NULL);
	for (auto& info : instances) {
		HWND hwnd = reinterpret_cast<HWND>(info.GetHWnd());

		info.SetIsForegroundWindow(fgW == hwnd);
		info.SetIsTopWindow(tW == hwnd);
	}

	// Fill in window z orders
	struct context {
		size_t toFind;
		std::vector<InstanceInfo>& list;
		unsigned int z;
	} ctxt{
		instances.size(),
		instances,
		0u
	};

	EnumWindows([](HWND hwnd, LPARAM list) -> BOOL {
		auto ctxt = reinterpret_cast<context*>(list);
		for (auto& info : ctxt->list) {
			if (info.GetHWnd() == reinterpret_cast<intptr_t>(hwnd)) {
				info.SetZDepth(ctxt->z);
				ctxt->toFind--;
				if (ctxt->toFind == 0) {
					return false; // found all
				}
			}
		}

		ctxt->z++;

		return true;
		}, reinterpret_cast<LPARAM>(&ctxt));

}
