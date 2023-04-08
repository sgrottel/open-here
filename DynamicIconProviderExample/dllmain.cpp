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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

extern "C" int __declspec(dllexport) openhere_generateicon(int width, int height, unsigned char* bgradata)
{
    for (int y = 0; y < height; ++y)
    {
        float fy = static_cast<float>(y) / static_cast<float>(height - 1);
        for (int x = 0; x < width; ++x)
        {
            float fx = static_cast<float>(x) / static_cast<float>(width - 1);

            unsigned char* c = bgradata + (x + y * width) * 4;

            float dx = (fx - 0.5f) * 2.0f;
            float dy = (fy - 0.5f) * 2.0f;
            float dr = dx * dx + dy * dy;

            float a = (1.0f - dr) * 5.0f;
            if (a < 0.0f) a = 0.0;
            if (a > 1.0f) a = 1.0f;

            c[0] = 0;
            c[1] = static_cast<unsigned char>(a * fy * 255.0f);
            c[2] = static_cast<unsigned char>(a * fx * 255.0f);
            c[3] = static_cast<unsigned char>(a * 255.0f);

        }
    }

    return TRUE;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

