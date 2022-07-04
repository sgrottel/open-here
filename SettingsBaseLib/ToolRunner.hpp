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
#include "Toolbox/ToolRunner.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;
using msclr::interop::marshal_as;


namespace OpenHere
{
namespace SettingsBase
{

    public ref class ToolRunner
    {
    public:

        ToolRunner()
        {
            m_runner = new openhere::toolbox::ToolRunner();
        }

        ~ToolRunner()
        {
            if (m_runner != nullptr)
            {
                delete m_runner;
                m_runner = nullptr;
            }
        }

        ToolStartConfig^ SelectStartConfig(ToolInfo^ tool, String^ path, IEnumerable<String^>^ files, bool checkPaths)
        {
            openhere::toolbox::ToolInfo t;
            CopyTo(t, tool);

            std::vector<std::wstring> nFiles;
            CopyFiles(nFiles, files);

            openhere::toolbox::ToolInfo::StartConfig const* sc = m_runner->SelectStartConfig(t, marshal_as<std::wstring>(path), nFiles, checkPaths);

            if (sc == nullptr) return nullptr;

            ToolStartConfig^ tsc = gcnew ToolStartConfig();
            CopyTo(tsc, *sc);

            return tsc;
        }

        bool CanStart(ToolInfo^ tool, String^ path, IEnumerable<String^>^ files, [Out] String^% errorMsg)
        {
            openhere::toolbox::ToolInfo t;
            CopyTo(t, tool);

            std::vector<std::wstring> nFiles;
            CopyFiles(nFiles, files);

            std::wstring errMsg;

            bool rv = m_runner->CanStart(t, marshal_as<std::wstring>(path), nFiles, errMsg);

            errorMsg = marshal_as<String^>(errMsg);

            return rv;
        }

        void ApplyVariables(ToolStartConfig^% config, String^ path, IEnumerable<String^>^ files)
        {
            if (config == nullptr) return;

            openhere::toolbox::ToolInfo::StartConfig sc;
            CopyTo(sc, config);

            std::vector<std::wstring> nFiles;
            CopyFiles(nFiles, files);

            m_runner->ApplyVariables(sc, marshal_as<std::wstring>(path), nFiles);

            CopyTo(config, sc);
        }

        bool Start(ToolStartConfig^ config)
        {
            if (config == nullptr) return false;

            openhere::toolbox::ToolInfo::StartConfig sc;
            CopyTo(sc, config);

            return m_runner->Start(sc);
        }

    protected:

        !ToolRunner()
        {
            if (m_runner != nullptr)
            {
                delete m_runner;
                m_runner = nullptr;
            }
        }

    private:

        void CopyFiles(std::vector<std::wstring>& dest, IEnumerable<String^>^ src)
        {
            dest.clear();
            IEnumerator<String^>^ e = src->GetEnumerator();
            while (e->MoveNext())
            {
                dest.push_back(marshal_as<std::wstring>(e->Current));
            }
        }

        openhere::toolbox::ToolRunner *m_runner{ nullptr };

    };

}
}
