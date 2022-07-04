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
#include "Toolbox/ToolInfoListLoader.h"


using namespace System;
using msclr::interop::marshal_as;


namespace OpenHere
{
namespace SettingsBase
{

    public ref class ToolInfoListLoader
    {
    public:

        array<ToolInfo^>^ Load()
        {
            openhere::toolbox::ToolInfoListLoader loader;
            std::vector<openhere::toolbox::ToolInfo> tools{ loader.Load() };

            array<ToolInfo^>^ l = gcnew array<ToolInfo^>(static_cast<int>(tools.size()));
            for (size_t i = 0; i < tools.size(); ++i)
            {
                CopyTo(l[static_cast<int>(i)], tools[i]);
            }
            return l;
        }

        bool Save(array<ToolInfo^>^ list)
        {
            std::vector<openhere::toolbox::ToolInfo> tools;
            tools.resize(list->Length);
            for (size_t i = 0; i < tools.size(); ++i)
            {
                CopyTo(tools[i], list[static_cast<int>(i)]);
            }

            openhere::toolbox::ToolInfoListLoader loader;
            return loader.Save(tools);
        }

    };

}
}
