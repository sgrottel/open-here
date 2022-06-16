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

class ColorFactory
{
public:
	ColorFactory();

	inline COLORREF GetForegroundColor() const { return m_foreground; }
	inline COLORREF GetBackgroundColor() const { return m_background; }
	inline COLORREF GetRedColor() const { return m_red; }
	inline COLORREF GetDisabledForegroundColor() const { return m_disabledForeground; }

private:
	COLORREF m_foreground;
	COLORREF m_background;
	COLORREF m_red;
	COLORREF m_disabledForeground;
};
