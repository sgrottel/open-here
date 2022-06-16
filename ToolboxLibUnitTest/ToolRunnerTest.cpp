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

#include "Toolbox/ToolRunner.h"

#include <string>
#include <array>
#include <algorithm>
#include <random>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using openhere::toolbox::ToolRunner;
using openhere::toolbox::ToolInfo;

namespace ToolboxLibUnitTest
{
	TEST_CLASS(ToolRunnerTest)
	{
	public:
		typedef ToolInfo::PathRequirement PathReq;
		typedef ToolInfo::FilesRequirement FilesReq;

		struct Input {
			std::wstring path;
			std::vector<std::wstring> files;
		};

		struct Results {
			PathReq pr;
			FilesReq fr;
		};

		bool Equals(PathReq r1, PathReq r2)
		{
			if (r1 == PathReq::DontCare) return true;
			if (r2 == PathReq::DontCare) return true;
			return r1 == r2;
		}

		bool Equals(FilesReq r1, FilesReq r2)
		{
			if (r1 == FilesReq::DontCare) return true;
			if (r2 == FilesReq::DontCare) return true;
			return r1 == r2;
		}

		TEST_METHOD(TestSelectStartConfig)
		{
			const std::array<Input, 10> inputs{ {
				{L"", {}},
				{L"", { L"file1" }},
				{L"", { L"file1", L"file2" }},
				{L"", { L"file1", L"file2", L"file3" }},
				{L"", { L"file1", L"file2", L"file3", L"file4" }},
				{L"path", {}},
				{L"path", { L"file1" }},
				{L"path", { L"file1", L"file2" }},
				{L"path", { L"file1", L"file2", L"file3" }},
				{L"path", { L"file1", L"file2", L"file3", L"file4" }},
			} };

			const std::array<Results, 15> results{ {
				{ PathReq::DontCare, FilesReq::DontCare },
				{ PathReq::DontCare, FilesReq::RequireOne },
				{ PathReq::DontCare, FilesReq::RequireOneOrMore },
				{ PathReq::DontCare, FilesReq::RequireTwoOrMore },
				{ PathReq::DontCare, FilesReq::RequireNull },
				{ PathReq::Required, FilesReq::DontCare },
				{ PathReq::Required, FilesReq::RequireOne },
				{ PathReq::Required, FilesReq::RequireOneOrMore },
				{ PathReq::Required, FilesReq::RequireTwoOrMore },
				{ PathReq::Required, FilesReq::RequireNull },
				{ PathReq::RequiredNull, FilesReq::DontCare },
				{ PathReq::RequiredNull, FilesReq::RequireOne },
				{ PathReq::RequiredNull, FilesReq::RequireOneOrMore },
				{ PathReq::RequiredNull, FilesReq::RequireTwoOrMore },
				{ PathReq::RequiredNull, FilesReq::RequireNull },
			} };

			const std::array<std::array<bool, 10>, 15> expectedMatch{ {
				{ true, true, true, true, true, true, true, true, true, true },
				{ false, true, false, false, false, false, true, false, false, false },
				{ false, true, true, true, true, false, true, true, true, true },
				{ false, false, true, true, true, false, false, true, true, true },
				{ true, false, false, false, false, true, false, false, false, false },
				{ false, false, false, false, false, true, true, true, true, true },
				{ false, false, false, false, false, false, true, false, false, false },
				{ false, false, false, false, false, false, true, true, true, true },
				{ false, false, false, false, false, false, false, true, true, true },
				{ false, false, false, false, false, true, false, false, false, false },
				{ true, true, true, true, true, false, false, false, false, false },
				{ false, true, false, false, false, false, false, false, false, false },
				{ false, true, true, true, true, false, false, false, false, false },
				{ false, false, true, true, true, false, false, false, false, false },
				{ true, false, false, false, false, false, false, false, false, false },
			} };

			for (size_t ii = 0; ii < inputs.size(); ++ii)
			{
				Input const& i{ inputs[ii] };

				std::vector<Results> pos;
				std::vector<Results> neg;
				for (size_t ri = 0; ri < results.size(); ++ri)
				{
					(expectedMatch[ri][ii] ? pos : neg).push_back(results[ri]);
				}

				auto rng = std::default_random_engine{};
				std::shuffle(std::begin(neg), std::end(neg), rng);

				for (Results const& r : pos)
				{
					for (size_t nrc = 0; nrc < neg.size(); ++nrc)
					{
						ToolRunner runner;
						ToolInfo ti;
						std::wstring msg{ L"I(" + std::to_wstring(ii) + L")" };

						ToolInfo::StartConfig const *rsc = runner.SelectStartConfig(ti, i.path, i.files, false);
						Assert::IsNull(rsc, msg.c_str());

						msg += L"-[";

						for (size_t nri = 0; nri < nrc; ++nri)
						{
							ToolInfo::StartConfig sc;
							sc.executable = L"dummy";
							sc.pathRequirement = neg[nri].pr;
							sc.filesRequirement = neg[nri].fr;
							ti.StartConfigs().push_back(sc);

							rsc = runner.SelectStartConfig(ti, i.path, i.files, false);
							msg += (nri == 0 ? L"" : L",") + std::to_wstring(static_cast<int>(neg[nri].pr)) + L"/" + std::to_wstring(static_cast<int>(neg[nri].fr));
							Assert::IsNull(rsc, msg.c_str());
						}

						msg += L"]";

						ToolInfo::StartConfig sc;
						sc.executable = L"dummy";
						sc.pathRequirement = r.pr;
						sc.filesRequirement = r.fr;
						ti.StartConfigs().push_back(sc);

						rsc = runner.SelectStartConfig(ti, i.path, i.files, false);
						Assert::IsNotNull(rsc, msg.c_str());
						Assert::IsTrue(Equals(rsc->pathRequirement, r.pr), msg.c_str());
						Assert::IsTrue(Equals(rsc->filesRequirement, r.fr), msg.c_str());
					}
				}
			}

		}
	};
}
