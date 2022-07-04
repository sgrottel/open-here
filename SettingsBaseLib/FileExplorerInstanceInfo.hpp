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
#include "FileExplorerDetector\InstanceInfo.h"

using namespace System;
using msclr::interop::marshal_as;


namespace OpenHere
{
namespace SettingsBase
{
namespace FileExplorer
{

    public ref class InstanceInfo
    {
    public:

        InstanceInfo()
        {
            m_info = new openhere::fileexplorerdetector::InstanceInfo();
        }

        InstanceInfo(openhere::fileexplorerdetector::InstanceInfo const& src)
        {
            m_info = new openhere::fileexplorerdetector::InstanceInfo();
            *m_info = src;
        }

        ~InstanceInfo()
        { // dtor
            if (m_info != nullptr)
            {
                delete m_info;
                m_info = nullptr;
            }
        }

        property IntPtr HWnd
        {
            IntPtr get() { return IntPtr(m_info->GetHWnd()); }
        }

        property unsigned int ZDepth
        {
            unsigned int get() { return m_info->GetZDepth(); }
        }

        property String^ InstanceType
        {
            String^ get() { return marshal_as<String^>(m_info->GetInstanceType()); }
        }

        property array<String^>^ OpenPaths
        {
            array<String^>^ get() {
                if (m_paths == nullptr)
                {
                    const int size = (m_info ? static_cast<int>(m_info->GetOpenPaths().size()) : 0);
                    m_paths = gcnew array<String^>(size);
                    for (int i = 0; i < size; ++i)
                    {
                        m_paths[i] = marshal_as<String^>(m_info->GetOpenPaths()[i]);
                    }
                }
                return m_paths;
            }
        }

        property array<String^>^ SelectedItems
        {
            array<String^>^ get() {
                if (m_selItems == nullptr)
                {
                    const int size = (m_info ? static_cast<int>(m_info->GetSelectedItems().size()) : 0);
                    m_selItems = gcnew array<String^>(size);
                    for (int i = 0; i < size; ++i)
                    {
                        m_selItems[i] = marshal_as<String^>(m_info->GetSelectedItems()[i]);
                    }
                }
                return m_selItems;
            }
        }

        property bool IsTopWindow
        {
            bool get() { return m_info->IsTopWindow(); }
        }

        property bool IsForegroundWindow
        {
            bool get() { return m_info->IsForegroundWindow(); }
        }

    protected:

        !InstanceInfo()
        { // finalizer
            if (m_info != nullptr)
            {
                delete m_info;
                m_info = nullptr;
            }
        }

    private:

        openhere::fileexplorerdetector::InstanceInfo* m_info;

        array<String^>^ m_paths;
        array<String^>^ m_selItems;

    };

}
}
}
