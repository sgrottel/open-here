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
#include "Toolbox/ToolInfo.h"


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections::Generic;
using namespace System::Collections::ObjectModel;
using msclr::interop::marshal_as;


namespace OpenHere
{
namespace SettingsBase
{

    public enum class ToolPathRequirement
    {
        DontCare,
        Required,
        RequiredNull
    };


    ToolPathRequirement Convert(openhere::toolbox::ToolInfo::PathRequirement src)
    {
        switch (src)
        {
        case openhere::toolbox::ToolInfo::PathRequirement::DontCare:
            return ToolPathRequirement::DontCare;
        case openhere::toolbox::ToolInfo::PathRequirement::Required:
            return ToolPathRequirement::Required;
        case openhere::toolbox::ToolInfo::PathRequirement::RequiredNull:
            return ToolPathRequirement::RequiredNull;
        }
        return ToolPathRequirement::DontCare;
    }


    openhere::toolbox::ToolInfo::PathRequirement Convert(ToolPathRequirement src)
    {
        switch (src)
        {
        case ToolPathRequirement::DontCare:
            return openhere::toolbox::ToolInfo::PathRequirement::DontCare;
        case ToolPathRequirement::Required:
            return openhere::toolbox::ToolInfo::PathRequirement::Required;
        case ToolPathRequirement::RequiredNull:
            return openhere::toolbox::ToolInfo::PathRequirement::RequiredNull;
        }
        return openhere::toolbox::ToolInfo::PathRequirement::DontCare;
    }


    public enum class ToolFilesRequirement
    {
        DontCare,
        RequireOne,
        RequireOneOrMore,
        RequireTwoOrMore,
        RequireNull
    };


    ToolFilesRequirement Convert(openhere::toolbox::ToolInfo::FilesRequirement src)
    {
        switch (src)
        {
        case openhere::toolbox::ToolInfo::FilesRequirement::DontCare:
            return ToolFilesRequirement::DontCare;
        case openhere::toolbox::ToolInfo::FilesRequirement::RequireOne:
            return ToolFilesRequirement::RequireOne;
        case openhere::toolbox::ToolInfo::FilesRequirement::RequireOneOrMore:
            return ToolFilesRequirement::RequireOneOrMore;
        case openhere::toolbox::ToolInfo::FilesRequirement::RequireTwoOrMore:
            return ToolFilesRequirement::RequireTwoOrMore;
        case openhere::toolbox::ToolInfo::FilesRequirement::RequireNull:
            return ToolFilesRequirement::RequireNull;
        }
        return ToolFilesRequirement::DontCare;
    }


    openhere::toolbox::ToolInfo::FilesRequirement Convert(ToolFilesRequirement src)
    {
        switch (src)
        {
        case ToolFilesRequirement::DontCare:
            return openhere::toolbox::ToolInfo::FilesRequirement::DontCare;
        case ToolFilesRequirement::RequireOne:
            return openhere::toolbox::ToolInfo::FilesRequirement::RequireOne;
        case ToolFilesRequirement::RequireOneOrMore:
            return openhere::toolbox::ToolInfo::FilesRequirement::RequireOneOrMore;
        case ToolFilesRequirement::RequireTwoOrMore:
            return openhere::toolbox::ToolInfo::FilesRequirement::RequireTwoOrMore;
        case ToolFilesRequirement::RequireNull:
            return openhere::toolbox::ToolInfo::FilesRequirement::RequireNull;
        }
        return openhere::toolbox::ToolInfo::FilesRequirement::DontCare;
    }


    public ref class ToolStartConfig : public INotifyPropertyChanged
    {
    public:

        virtual event PropertyChangedEventHandler^ PropertyChanged;

        property ToolPathRequirement PathRequirement
        {
            ToolPathRequirement get() { return m_pathReq; }
            void set(ToolPathRequirement v)
            {
                if (m_pathReq != v)
                {
                    m_pathReq = v;
                    OnPropertyChanged("PathRequirement");
                }
            }
        }

        property ToolFilesRequirement FilesRequirement
        {
            ToolFilesRequirement get() { return m_filesReq; }
            void set(ToolFilesRequirement v)
            {
                if (m_filesReq != v)
                {
                    m_filesReq = v;
                    OnPropertyChanged("FilesRequirement");
                }
            }
        }

        property String^ Executable
        {
            String^ get() { return m_executable; }
            void set(String^ v)
            {
                if (!String::Equals(m_executable, v))
                {
                    m_executable = v;
                    OnPropertyChanged("Executable");
                }
            }
        }

        property String^ WorkingDirectory
        {
            String^ get() { return m_workingDir; }
            void set(String^ v)
            {
                if (!String::Equals(m_workingDir, v))
                {
                    m_workingDir = v;
                    OnPropertyChanged("WorkingDirectory");
                }
            }
        }

        property array<String^>^ Arguments
        {
            array<String^>^ get() { return m_arguments; }
            void set(array<String^>^ v)
            {
                if (m_arguments != v)
                {
                    m_arguments = v;
                    OnPropertyChanged("Arguments");
                }
            }
        }

    private:

        void OnPropertyChanged(const char* name)
        {
            PropertyChanged(this, gcnew PropertyChangedEventArgs(marshal_as<String^>(name)));
        }

        ToolPathRequirement m_pathReq;
        ToolFilesRequirement m_filesReq;
        String^ m_executable;
        String^ m_workingDir;
        array<String^>^ m_arguments;
    };


    void CopyTo(ToolStartConfig^% dest, openhere::toolbox::ToolInfo::StartConfig const& src)
    {
        if (dest == nullptr)
        {
            dest = gcnew ToolStartConfig();
        }

        dest->PathRequirement = Convert(src.pathRequirement);
        dest->FilesRequirement = Convert(src.filesRequirement);
        dest->Executable = marshal_as<String^>(src.executable);
        dest->WorkingDirectory = marshal_as<String^>(src.workingDir);

        dest->Arguments = gcnew array<String^>(static_cast<int>(src.arguments.size()));
        for (size_t i = 0; i < src.arguments.size(); ++i)
        {
            dest->Arguments[i] = marshal_as<String^>(src.arguments[i]);
        }
    }


    void CopyTo(openhere::toolbox::ToolInfo::StartConfig& dest, ToolStartConfig^ src)
    {
        dest.pathRequirement = Convert(src->PathRequirement);
        dest.filesRequirement = Convert(src->FilesRequirement);
        dest.executable = (src->Executable != nullptr) ? marshal_as<std::wstring>(src->Executable) : L"";
        dest.workingDir = (src->WorkingDirectory != nullptr) ? marshal_as<std::wstring>(src->WorkingDirectory) : L"";

        dest.arguments.resize((src->Arguments != nullptr) ? src->Arguments->Length : 0);
        for (size_t i = 0; i < dest.arguments.size(); ++i)
        {
            String^ arg = src->Arguments[i];
            dest.arguments[i] = (arg != nullptr) ? marshal_as<std::wstring>(arg) : L"";
        }
    }


    public ref class ToolInfo : public INotifyPropertyChanged
    {
    public:

        virtual event PropertyChangedEventHandler^ PropertyChanged;

        property String^ Title
        {
            String^ get() { return m_title; }
            void set(String^ v)
            {
                if (!String::Equals(m_title, v))
                {
                    m_title = v;
                    OnPropertyChanged("Title");
                }
            }
        }

        property String^ IconFile
        {
            String^ get() { return m_iconFile; }
            void set(String^ v)
            {
                if (!String::Equals(m_iconFile, v))
                {
                    m_iconFile = v;
                    OnPropertyChanged("IconFile");
                }
            }
        }

        property int IconId
        {
            int get() { return m_iconId; }
            void set(int v)
            {
                if (m_iconId != v)
                {
                    m_iconId = v;
                    OnPropertyChanged("IconId");
                }
            }
        }

        property ObservableCollection<ToolStartConfig^>^ StartConfigs
        {
            ObservableCollection<ToolStartConfig^>^ get() { return m_startConfigs; }
            void set(ObservableCollection<ToolStartConfig^>^ v)
            {
                if (m_startConfigs != v)
                {
                    PreStartConfigsChange();
                    m_startConfigs = v;
                    PostStartConfigsChange();
                    OnPropertyChanged("StartConfigs");
                }
            }
        }

    protected:

        void OnPropertyChanged(const char* name)
        {
            OnPropertyChanged(marshal_as<String^>(name));
        }

        void OnPropertyChanged(String^ name)
        {
            PropertyChanged(this, gcnew PropertyChangedEventArgs(name));
        }

        virtual void PreStartConfigsChange()
        {
        }

        virtual void PostStartConfigsChange()
        {
        }

        ToolPathRequirement CombinedPathRequirement();
        ToolFilesRequirement CombinedFilesRequirement();

    private:

        String^ m_title;
        String^ m_iconFile;
        int m_iconId;
        ObservableCollection<ToolStartConfig^>^ m_startConfigs = gcnew ObservableCollection<ToolStartConfig^>();
    };


    void CopyTo(ToolInfo^% dest, openhere::toolbox::ToolInfo const& src)
    {
        if (dest == nullptr)
        {
            dest = gcnew ToolInfo();
        }
        dest->Title = marshal_as<String^>(src.GetTitle());
        dest->IconFile = marshal_as<String^>(src.GetIconFile());
        dest->IconId = src.GetIconID();

        ObservableCollection<ToolStartConfig^>^ cfgs = gcnew ObservableCollection<ToolStartConfig^>();
        for (auto const& cfg : src.StartConfigs())
        {
            ToolStartConfig^ tsc = gcnew ToolStartConfig();
            CopyTo(tsc, cfg);
            cfgs->Add(tsc);
        }

        dest->StartConfigs = cfgs;
    }


    void CopyTo(openhere::toolbox::ToolInfo& dest, ToolInfo^ src)
    {
        dest.SetTitle((src->Title != nullptr) ? marshal_as<std::wstring>(src->Title) : L"");
        dest.SetIconFile((src->IconFile != nullptr) ? marshal_as<std::wstring>(src->IconFile) : L"");
        dest.SetIconID(src->IconId);

        auto& cfgs = dest.StartConfigs();
        cfgs.resize(src->StartConfigs->Count);

        IEnumerator<ToolStartConfig^>^ e = src->StartConfigs->GetEnumerator();
        e->MoveNext();
        for (size_t i = 0; i < cfgs.size(); ++i, e->MoveNext())
        {
            CopyTo(cfgs[i], e->Current);
        }
    }

    ToolPathRequirement ToolInfo::CombinedPathRequirement()
    {
        openhere::toolbox::ToolInfo t;
        CopyTo(t, this);
        return Convert(t.GetPathRequirement());
    }

    ToolFilesRequirement ToolInfo::CombinedFilesRequirement()
    {
        openhere::toolbox::ToolInfo t;
        CopyTo(t, this);
        return Convert(t.GetFilesRequirement());
    }


}
}
