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
#include "Toolbox/VarStringSplitter.h"

#include <cassert>


std::vector<std::wstring_view> openhere::toolbox::VariableSplit(std::wstring const& str)
{
	std::vector<std::wstring_view> res;
	size_t last = 0;
	const size_t len = str.size();
	int mode = 0; // in normal string

	for (size_t i = 0; i < len; ++i)
	{
		switch (mode)
		{
		case 0:
			if (str[i] == L'$')
				mode = 1;
			break;
		case 1:
			if (str[i] == L'{')
			{
				assert(i >= 1);
				res.push_back(std::wstring_view(str.data() + last, str.data() + (i - 1)));
				last = i + 1;
				mode = 2;
			}
			else if (str[i] != L'$')
			{
				mode = 0;
			}
			break;
		case 2:
			if (str[i] == L'}')
			{
				res.push_back(std::wstring_view(str.data() + last, str.data() + i));
				last = i + 1;
				mode = 0;
			}
			break;
		}

	}
	res.push_back(std::wstring_view(str.data() + last, str.data() + len));
	if (mode == 2)
	{
		res.push_back(std::wstring_view(str.data() + len, str.data() + len));
	}

	return res;
}


std::wstring openhere::toolbox::Join(std::vector<std::wstring_view>const& strs)
{
	std::wstring r;
	size_t s = 0;
	for (std::wstring_view const& wsv : strs) s += wsv.size();
	r.reserve(s);
	for (std::wstring_view const& wsv : strs) r += wsv;
	return r;
}
