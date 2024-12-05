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

	void addWindowsInfo(std::vector<InstanceInfo>& newInstances)
	{
		HWND fgW = GetForegroundWindow();
		HWND tW = GetTopWindow(NULL);
		for (auto& info : newInstances) {
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
			newInstances.size(),
			newInstances,
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

	std::vector<InstanceInfo> newInstances;
	newInstances.reserve(count);
	std::vector<std::vector<int>> childPaths;
	childPaths.reserve(count);

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
		if (SUCCEEDED(pApp->get_HWND(reinterpret_cast<SHANDLE_PTR*>(&hwnd)))) {
			info.SetHWnd(reinterpret_cast<intptr_t>(hwnd));
		} else {
			hwnd = 0;
			info.SetHWnd(0);
		}

		CComQIPtr<IServiceProvider> psp{ pApp };
		if (!psp) {
			continue; // This window doesn't implement IServiceProvider
		}

		CComPtr<IShellBrowser> pBrowser;
		if (FAILED(psp->QueryService(SID_STopLevelBrowser, &pBrowser))) {
			continue; // This window doesn't provide IShellBrowser
		}

		// check the child-sibling path length to determine which of the multiple tabs in an explorer window is currently selected.
		std::vector<int> childPath;
		if (hwnd != 0) {
			HWND hCWnd;
			if (SUCCEEDED(pBrowser->GetControlWindow(FCW_STATUS, &hCWnd))) {
				HWND h = hCWnd;
				while (h != NULL) {
					int cnt = 0;
					HWND s = h;
					while (s != NULL) {
						s = GetWindow(s, GW_HWNDPREV);
						if (s == NULL) break;
						cnt++;
					}
					childPath.push_back(cnt);
					h = GetParent(h);
					if (h == hwnd) break;
				};
			}
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
		if (SUCCEEDED(pFolder->GetCurFolder(&pidl))) {
			std::vector<wchar_t> path(32767, L'\0');
			if (::SHGetPathFromIDListEx(pidl, path.data(), static_cast<DWORD>(path.size()), 0)) {
				info.AccessOpenPaths().push_back(path.data());
			}
			::CoTaskMemFree(pidl);
		}

		newInstances.push_back(info);
		childPaths.push_back(childPath);
	}

	addWindowsInfo(newInstances);

	std::vector<int> order;
	order.resize(newInstances.size());
	for (int i = 0; i < order.size(); ++i) order[i] = i;

	std::sort(order.begin(), order.end(), [&](int a, int b) {
		unsigned int aZ = newInstances[a].GetZDepth();
		unsigned int bZ = newInstances[b].GetZDepth();
		if (aZ == bZ) {
			std::vector<int> aP = childPaths[a];
			std::vector<int> bP = childPaths[b];
			while (!aP.empty()) {
				if (bP.empty()) return false;
				int aD = aP.back();
				int bD = bP.back();
				aP.pop_back();
				bP.pop_back();
				if (aD != bD) {
					return aD < bD;
				}
			}
			return !bP.empty();
		}
		return aZ < bZ;
		});


	unsigned int lastZ = 0;
	unsigned int nextSubOrder = 0;
	for (int i : order) {
		instances.push_back(newInstances[i]);
		unsigned int z = instances.back().GetZDepth();
		if (lastZ != z) {
			lastZ = z;
			nextSubOrder = 0;
		} else {
			instances.back().SetSubOrder(++nextSubOrder);
		}
	}

	return static_cast<int>(instances.size());
}

std::vector<InstanceInfo> const& FileExplorerDetector::GetInstances() const
{
	return instances;
}
