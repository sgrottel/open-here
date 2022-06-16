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
#include <CppUnitTest.h>

#include "Toolbox/ToolInfo.h"

#include <array>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using openhere::toolbox::ToolInfo;

namespace ToolboxLibUnitTest
{

	TEST_CLASS(ToolInfoTest)
	{
	public:
		typedef ToolInfo::PathRequirement PathReq;
		typedef ToolInfo::FilesRequirement FilesReq;

		template<typename T, unsigned int AS, typename TGen, typename TGet>
		void TestRequirementImpl(T dontCare, std::array<T, AS> const& oVals, TGen gen, TGet get)
		{
			{ // empty
				ToolInfo ti;
				Assert::IsTrue(ti.StartConfigs().size() == 0);
				Assert::IsTrue(get(ti) == dontCare);

				ti.StartConfigs().push_back(gen(dontCare));
				Assert::IsTrue(ti.StartConfigs().size() == 1);
				Assert::IsTrue(get(ti) == dontCare);

				ti.StartConfigs().push_back(gen(dontCare));
				Assert::IsTrue(ti.StartConfigs().size() == 2);
				Assert::IsTrue(get(ti) == dontCare);
			}

			for (T v1 : oVals)
			{
				ToolInfo ti;
				Assert::IsTrue(ti.StartConfigs().size() == 0);
				Assert::IsTrue(get(ti) == dontCare);

				for (T v2 : oVals)
				{
					ti.StartConfigs().clear();
					Assert::IsTrue(ti.StartConfigs().size() == 0);
					Assert::IsTrue(get(ti) == dontCare);

					ti.StartConfigs().push_back(gen(v1));
					Assert::IsTrue(ti.StartConfigs().size() == 1);
					Assert::IsTrue(get(ti) == v1);

					ti.StartConfigs().push_back(gen(v2));
					Assert::IsTrue(ti.StartConfigs().size() == 2);
					Assert::IsTrue(get(ti) == ((v1 == v2) ? v1 : dontCare));

					ti.StartConfigs().push_back(gen(dontCare));
					Assert::IsTrue(ti.StartConfigs().size() == 3);
					Assert::IsTrue(get(ti) == dontCare);
				}

				ti.StartConfigs().clear();
				Assert::IsTrue(get(ti) == dontCare);

				ti.StartConfigs().push_back(gen(v1));
				Assert::IsTrue(ti.StartConfigs().size() == 1);
				Assert::IsTrue(get(ti) == v1);

				ti.StartConfigs().push_back(gen(dontCare));
				Assert::IsTrue(ti.StartConfigs().size() == 2);
				Assert::IsTrue(get(ti) == dontCare);

				ti.StartConfigs().clear();
				Assert::IsTrue(get(ti) == dontCare);

				ti.StartConfigs().push_back(gen(dontCare));
				Assert::IsTrue(ti.StartConfigs().size() == 1);
				Assert::IsTrue(get(ti) == dontCare);

				ti.StartConfigs().push_back(gen(v1));
				Assert::IsTrue(ti.StartConfigs().size() == 2);
				Assert::IsTrue(get(ti) == dontCare);
			}
		}

		TEST_METHOD(TestSummaryPathRequirement)
		{
			std::array<PathReq, 2> otherValues{
				PathReq::Required,
				PathReq::RequiredNull
			};
			auto generateStartConfig = [](PathReq pr)
			{
				ToolInfo::StartConfig sc;
				sc.pathRequirement = pr;
				return sc;
			};
			auto reqGetter = [](const ToolInfo& sc)
			{
				return sc.GetPathRequirement();
			};

			TestRequirementImpl(PathReq::DontCare, otherValues, generateStartConfig, reqGetter);
		}

		TEST_METHOD(TestSummaryFilesRequirement)
		{
			std::array<FilesReq, 4> otherValues{
				FilesReq::RequireOne,
				FilesReq::RequireOneOrMore,
				FilesReq::RequireTwoOrMore,
				FilesReq::RequireNull
			};
			auto generateStartConfig = [](FilesReq fr)
			{
				ToolInfo::StartConfig sc;
				sc.filesRequirement = fr;
				return sc;
			};
			auto reqGetter = [](const ToolInfo& sc)
			{
				return sc.GetFilesRequirement();
			};

			TestRequirementImpl(FilesReq::DontCare, otherValues, generateStartConfig, reqGetter);
		}

	};
}
