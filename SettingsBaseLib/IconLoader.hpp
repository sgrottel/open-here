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

#include "Toolbox/IconLoader.h"

using namespace System;
using msclr::interop::marshal_as;

namespace OpenHere
{
	namespace SettingsBase
	{

		public ref class IconLoader
		{
		public:

			System::Windows::Media::ImageSource^ LoadFromIconFile(String^ path, int id, int width)
			{
				SIZE size{ width, width };
				HBITMAP bmp = NULL;
				try
				{
					openhere::toolbox::IconLoader loader;
					bmp = loader.LoadFromIconFile(marshal_as<std::wstring>(path).c_str(), id, size);

					System::Windows::Media::Imaging::BitmapSource^ bmpSrc
						= System::Windows::Interop::Imaging::CreateBitmapSourceFromHBitmap(
							IntPtr(bmp),
							IntPtr::Zero, System::Windows::Int32Rect::Empty,
							System::Windows::Media::Imaging::BitmapSizeOptions::FromEmptyOptions());

					return bmpSrc;
				}
				finally
				{
					DeleteObject(bmp);
				}
			}

			System::Windows::Media::ImageSource^ GetIcon(IconLibrary^ lib, UInt32 id, int width)
			{
				HBITMAP bmp = NULL;
				try
				{
					openhere::toolbox::IconLoader loader;
					bmp = loader.FromIcon(lib->GetIcon(id, width, width), width, width);

					System::Windows::Media::Imaging::BitmapSource^ bmpSrc
						= System::Windows::Interop::Imaging::CreateBitmapSourceFromHBitmap(
							IntPtr(bmp),
							IntPtr::Zero, System::Windows::Int32Rect::Empty,
							System::Windows::Media::Imaging::BitmapSizeOptions::FromEmptyOptions());

					return bmpSrc;
				}
				finally
				{
					DeleteObject(bmp);
				}
			}

		};

	}
}
