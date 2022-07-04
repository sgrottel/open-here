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

#include "Toolbox/../../VarStringSplitter.h"

#include <string>
#include <array>
#include <algorithm>
#include <random>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using openhere::toolbox::VariableSplit;

namespace ToolboxLibUnitTest
{
	TEST_CLASS(VarStringSplitterTest)
	{
	public:

		TEST_METHOD(TestSplit)
		{
			std::wstring input{ L"a${b}c${path}d$${}{e}f" };
			auto split = VariableSplit(input);

			Assert::AreEqual(static_cast<size_t>(7), split.size());
			Assert::AreEqual(std::wstring_view{ L"a" }, split[0]);
			Assert::AreEqual(std::wstring_view{ L"b" }, split[1]);
			Assert::AreEqual(std::wstring_view{ L"c" }, split[2]);
			Assert::AreEqual(std::wstring_view{ L"path" }, split[3]);
			Assert::AreEqual(std::wstring_view{ L"d$" }, split[4]);
			Assert::AreEqual(std::wstring_view{ L"" }, split[5]);
			Assert::AreEqual(std::wstring_view{ L"{e}f" }, split[6]);
		}

		TEST_METHOD(TestSplitEmpty)
		{
			std::wstring input{ L"" };
			auto split = VariableSplit(input);

			Assert::AreEqual(static_cast<size_t>(1), split.size());
			Assert::AreEqual(std::wstring_view{ L"" }, split[0]);
		}

		TEST_METHOD(TestSplitNoVar)
		{
			std::wstring input{ L"a{b}c" };
			auto split = VariableSplit(input);

			Assert::AreEqual(static_cast<size_t>(1), split.size());
			Assert::AreEqual(std::wstring_view{ L"a{b}c" }, split[0]);
		}

		TEST_METHOD(TestSplitEmptyStart)
		{
			std::wstring input{ L"${1}2" };
			auto split = VariableSplit(input);

			Assert::AreEqual(static_cast<size_t>(3), split.size());
			Assert::AreEqual(std::wstring_view{ L"" }, split[0]);
			Assert::AreEqual(std::wstring_view{ L"1" }, split[1]);
			Assert::AreEqual(std::wstring_view{ L"2" }, split[2]);
		}

		TEST_METHOD(TestSplitEmptyEnd)
		{
			std::wstring input{ L"x${y}" };
			auto split = VariableSplit(input);

			Assert::AreEqual(static_cast<size_t>(3), split.size());
			Assert::AreEqual(std::wstring_view{ L"x" }, split[0]);
			Assert::AreEqual(std::wstring_view{ L"y" }, split[1]);
			Assert::AreEqual(std::wstring_view{ L"" }, split[2]);
		}

		TEST_METHOD(TestSplitVarOpenEnd)
		{
			std::wstring input{ L"x${y" };
			auto split = VariableSplit(input);

			Assert::AreEqual(static_cast<size_t>(3), split.size());
			Assert::AreEqual(std::wstring_view{ L"x" }, split[0]);
			Assert::AreEqual(std::wstring_view{ L"y" }, split[1]);
			Assert::AreEqual(std::wstring_view{ L"" }, split[2]);
		}

		TEST_METHOD(TestSplitOnlyVar)
		{
			std::wstring input{ L"${a}" };
			auto split = VariableSplit(input);

			Assert::AreEqual(static_cast<size_t>(3), split.size());
			Assert::AreEqual(std::wstring_view{ L"" }, split[0]);
			Assert::AreEqual(std::wstring_view{ L"a" }, split[1]);
			Assert::AreEqual(std::wstring_view{ L"" }, split[2]);
		}

		TEST_METHOD(TestSplitOnlyTwoVar)
		{
			std::wstring input{ L"${x}${y}" };
			auto split = VariableSplit(input);

			Assert::AreEqual(static_cast<size_t>(5), split.size());
			Assert::AreEqual(std::wstring_view{ L"" }, split[0]);
			Assert::AreEqual(std::wstring_view{ L"x" }, split[1]);
			Assert::AreEqual(std::wstring_view{ L"" }, split[2]);
			Assert::AreEqual(std::wstring_view{ L"y" }, split[3]);
			Assert::AreEqual(std::wstring_view{ L"" }, split[4]);
		}

	};
}
