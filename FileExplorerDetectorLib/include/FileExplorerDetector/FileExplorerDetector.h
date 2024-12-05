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

#include "InstanceInfo.h"

#include <vector>
#include <memory>

namespace openhere
{
namespace fileexplorerdetector
{

	class CoInitialize;

	class FileExplorerDetector
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
		int Detect();

		std::vector<InstanceInfo> const& GetInstances() const;

	private:
		std::shared_ptr<CoInitialize> coInitialize;
		std::vector<InstanceInfo> instances;
	};

}
}
