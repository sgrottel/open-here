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
#include "ColorFactory.h"

#include <winrt/Windows.UI.ViewManagement.h>


namespace
{

    inline bool IsColorLight(winrt::Windows::UI::Color const& color)
    {
        return (((5 * color.G) + (2 * color.R) + color.B) > (8 * 128));
    }

    inline COLORREF ToRGB(winrt::Windows::UI::Color const& color)
    {
        return RGB(color.R, color.G, color.B);
    }

    inline COLORREF Invert(COLORREF const& color)
    {
        BYTE r = GetRValue(color);
        BYTE g = GetGValue(color);
        BYTE b = GetBValue(color);
        return RGB(255 - r, 255 - g, 255 - b);
    }

}


ColorFactory::ColorFactory()
    : m_foreground{ RGB(0, 0, 0) }, m_background{ RGB(255, 255, 255) }, m_red{ RGB(0xC4, 0x2B, 0x1C) }
{
    using namespace winrt;
    using namespace Windows::UI::ViewManagement;

    UISettings const ui_settings{};

    auto const accent_color{ ui_settings.GetColorValue(UIColorType::Accent) };
    bool const lightAccent = IsColorLight(accent_color);
    m_background = ToRGB(accent_color);

    auto const foreground_color{ ui_settings.GetColorValue(UIColorType::Foreground) };
    bool const lightForeground = IsColorLight(foreground_color);
    m_foreground = ToRGB(foreground_color);

    if (lightAccent == lightForeground)
    {
        m_foreground = Invert(m_foreground);
    }

    m_disabledForeground = RGB(
        static_cast<uint8_t>((static_cast<int>(GetRValue(m_foreground)) + 3 * static_cast<int>(GetRValue(m_background))) / 4),
        static_cast<uint8_t>((static_cast<int>(GetGValue(m_foreground)) + 3 * static_cast<int>(GetGValue(m_background))) / 4),
        static_cast<uint8_t>((static_cast<int>(GetBValue(m_foreground)) + 3 * static_cast<int>(GetBValue(m_background))) / 4)
    );
}
