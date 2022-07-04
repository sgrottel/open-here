using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OpenHere.SettingsApp
{

    internal class IconSizeUtility
    {
        internal static float UiScaling = 1.0f;

        internal static int IconSize(int preferredSize)
        {
            float size = preferredSize * UiScaling;
            if (size > 256.0f) { }
            else if (size > 64.0f) { size = 256.0f; }
            else if (size > 48.0f) { size = 64.0f; }
            else if (size > 32.0f) { size = 48.0f; }
            else if (size > 24.0f) { size = 32.0f; }
            else if (size > 16.0f) { size = 24.0f; }
            else { size = 16.0f; }
            return (int)size;
        }
    }

}
