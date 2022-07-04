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
#include "FileExplorerDetector\FileExplorerDetector.h"

#include "FileExplorerInstanceInfo.hpp"

using namespace System;
using msclr::interop::marshal_as;


namespace OpenHere
{
namespace SettingsBase
{
namespace FileExplorer
{

    public ref class Detector
    {
    public:

        /// <summary>
        /// Run the detection of file explorer applications
        /// </summary>
        /// <returns>
        ///		>0 Number of detected file explorer application instances
        ///		=0 No file explorer applications where found
        ///		<0 An error occured without throwing an exception
        /// </returns>
        int Detect()
        {
            openhere::fileexplorerdetector::FileExplorerDetector d;
            int retVal = d.Detect();
            if (retVal >= 0)
            {
                auto const& insts = d.GetInstances();

                m_instances = gcnew array<InstanceInfo^>(static_cast<int>(insts.size()));
                for (int i = 0; i < static_cast<int>(insts.size()); ++i)
                {
                    m_instances[i] = gcnew InstanceInfo(insts[i]);
                }

            }
            else
            {
                m_instances = nullptr;
            }
            return retVal;
        }

        property array<InstanceInfo^>^ Instances
        {
            array<InstanceInfo^>^ get() { return m_instances; }
        }

    private:

        array<InstanceInfo^>^ m_instances;

    };

}
}
}
