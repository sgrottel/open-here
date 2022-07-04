using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Xml.Serialization;

namespace OpenHere.SettingsApp
{

    public class ImportExport
    {
        const string CorrectFormatVersion = "OpenHere.Settings.V1.0";

        public string? FormatVersion { get; set; } = null;

        public SettingsBase.Config? Config { get; set; } = null;

        public SettingsBase.ToolInfo[]? Tools { get; set; } = null;

        internal void Export(string filename)
        {
            FormatVersion = CorrectFormatVersion;

            XmlWriterSettings wrtrSettings = new XmlWriterSettings();
            wrtrSettings.Indent = true;
            wrtrSettings.Encoding = new UTF8Encoding(false);

            using (XmlWriter wrtr = XmlWriter.Create(filename, wrtrSettings))
            {
                XmlSerializer xmlSerializer = new XmlSerializer(typeof(ImportExport));
                xmlSerializer.Serialize(wrtr, this);
            }
        }

        internal void Import(string filename)
        {
            using (XmlReader rdr = XmlReader.Create(filename))
            {
                XmlSerializer xmlSerializer = new XmlSerializer(typeof(ImportExport));
                ImportExport? o = xmlSerializer.Deserialize(rdr) as ImportExport;
                if (!string.Equals(o?.FormatVersion,CorrectFormatVersion))
                {
                    throw new Exception("Settings file format version is not compatible");
                }
                Config = o?.Config;
                Tools = o?.Tools;
            }
        }
    }
}
