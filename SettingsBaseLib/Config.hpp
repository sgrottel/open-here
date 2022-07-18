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
#include "Toolbox/Config.h"


using namespace System;
using namespace System::ComponentModel;
using msclr::interop::marshal_as;


namespace OpenHere
{
namespace SettingsBase
{

    public ref class Config : public INotifyPropertyChanged
    {
    public:

        virtual event PropertyChangedEventHandler^ PropertyChanged;

        Config()
        {
            m_config = new openhere::toolbox::Config();
        }

        ~Config()
        { // dtor
            if (m_config != nullptr)
            {
                delete m_config;
                m_config = nullptr;
            }
        }

        String^ Path()
        {
            return marshal_as<String^>(openhere::toolbox::Config::Path());
        }

        String^ Path(String^ filename)
        {
            return marshal_as<String^>(openhere::toolbox::Config::Path(marshal_as<std::wstring>(filename).c_str()));
        }

        bool Load()
        {
            return m_config->Load();
        }

        bool Save()
        {
            return m_config->Save();
        }

        property bool PlayStartSound
        {
            bool get() { return m_config->GetPlayStartSound(); }
            void set(bool v) {
                if (v != m_config->GetPlayStartSound())
                {
                    m_config->SetPlayStartSound(v);
                    OnPropertyChanged("PlayStartSound");
                }
            }
        }

        property bool PlayToolStartSound
        {
            bool get() { return m_config->GetPlayToolStartSound(); }
            void set(bool v) {
                if (v != m_config->GetPlayToolStartSound())
                {
                    m_config->SetPlayToolStartSound(v);
                    OnPropertyChanged("PlayToolStartSound");
                }
            }
        }

        property bool StartToolToFront
        {
            bool get() { return m_config->GetStartToolToFront(); }
            void set(bool v) {
                if (v != m_config->GetStartToolToFront())
                {
                    m_config->SetStartToolToFront(v);
                    OnPropertyChanged("StartToolToFront");
                }
            }
        }

        property bool DoLog
        {
            bool get() { return m_config->GetDoLog(); }
            void set(bool v) {
                if (v != m_config->GetDoLog())
                {
                    m_config->SetDoLog(v);
                    OnPropertyChanged("DoLog");
                }
            }
        }

    protected:

        !Config()
        { // finalizer
            if (m_config != nullptr)
            {
                delete m_config;
                m_config = nullptr;
            }
        }

    private:

        void OnPropertyChanged(const char* name)
        {
            PropertyChanged(this, gcnew PropertyChangedEventArgs(marshal_as<String^>(name)));
        }

        openhere::toolbox::Config *m_config;

    };

}
}
