// Open Here
// Copyright 2023 SGrottel (https://www.sgrottel.de)
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
#include "DynamicIconProviderLoader.h"

#include <vector>

using namespace openhere::toolbox;

bool DynamicIconProviderLoader::Load(std::wstring const& path)
{
	// TODO: Implement
	return true;
}

HBITMAP DynamicIconProviderLoader::Generate(SIZE const& size)
{
	std::vector<uint8_t> bgraData;
	bgraData.resize(size.cx * size.cy * 4);


	// TODO: Implement
	for (int i = 0; i < bgraData.size(); i += 4)
	{
		uint8_t* c = bgraData.data() + i;
		c[0] = 255;
		c[1] = 0;
		c[2] = 0;
		c[3] = 255;
	}


	BITMAPINFOHEADER bmih{ 0 };
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biWidth = size.cx;
	bmih.biHeight = size.cy;
	bmih.biPlanes = 1;
	bmih.biBitCount = 32;
	bmih.biCompression = BI_RGB;
	HDC hdc = ::GetDC(nullptr);
	HBITMAP hbmp = CreateDIBitmap(hdc, &bmih, CBM_INIT, bgraData.data(), reinterpret_cast<const BITMAPINFO*>(&bmih), DIB_RGB_COLORS);
	ReleaseDC(nullptr, hdc);
	return hbmp;
}
