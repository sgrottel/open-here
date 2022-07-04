using Microsoft.WindowsAPICodePack.Dialogs;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace OpenHere.SettingsApp
{
    /// <summary>
    /// Interaction logic for SelectIconDialog.xaml
    /// </summary>
    public partial class SelectIconDialog : Window, INotifyPropertyChanged
    {
        private string filename = "";
        private SettingsBase.IconLibrary library = new SettingsBase.IconLibrary();

        public struct IconInfo
        {
            public uint Id { get; set; }
            public ImageSource Icon { get; set; }
        }

        public string Filename
        {
            get { return filename; }
            set
            {
                if (filename != value)
                {
                    filename = value;
                    PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(Filename)));

                    library.Close();
                    Icons.Clear();

                    int width = IconSizeUtility.IconSize(48);
                    try
                    {
                        SettingsBase.IconLoader loader = new SettingsBase.IconLoader();
                        library.Open(Filename, width, width);
                        uint cnt = library.GetCount();
                        for (uint i = 0; i < cnt; ++i)
                        {
                            IconInfo ii = new IconInfo();
                            ii.Id = library.GetId(i);
                            ii.Icon = loader.GetIcon(library, ii.Id, width);
                            Icons.Add(ii);
                        }

                    }
                    catch { }
                }
            }
        }

        public int Id
        {
            get { return ((int?)(IconsList.SelectedItem as IconInfo?)?.Id) ?? -1; }
            set
            {
                IconInfo? sel = IconsList.SelectedItem as IconInfo?;
                if (value < 0)
                {
                    if (sel == null) return; // ok
                    IconsList.SelectedItem = null;
                }
                else
                {
                    sel = null;
                    foreach (IconInfo ii in Icons)
                    {
                        if (ii.Id == value)
                        {
                            sel = ii;
                            break;
                        }
                    }
                    IconsList.SelectedItem = sel;
                }
            }
        }

        public ObservableCollection<IconInfo> Icons {
            get;
        } = new ObservableCollection<IconInfo>();

        public static string DialogTitle(string toolTitle)
        {
            return toolTitle + " Select Icon...";
        }

        public static CommonOpenFileDialog CreateOpenFileDialog(string toolTitle, string filename)
        {
            CommonOpenFileDialog ofd = new CommonOpenFileDialog();
            ofd.Title = DialogTitle(toolTitle);
            ofd.DefaultFileName = filename;
            if (!string.IsNullOrWhiteSpace(filename))
            {
                ofd.InitialDirectory = System.IO.Path.GetDirectoryName(filename);
            }
            ofd.RestoreDirectory = true;
            ofd.EnsureFileExists = true;
            ofd.AllowNonFileSystemItems = false;
            ofd.Filters.Add(new CommonFileDialogFilter("Supported Files", ".ico;.exe;.dll"));
            ofd.Filters.Add(new CommonFileDialogFilter("Icon Files", ".ico"));
            ofd.Filters.Add(new CommonFileDialogFilter("Executables Files", ".exe"));
            ofd.Filters.Add(new CommonFileDialogFilter("Dynamic Library Files", ".dll"));
            ofd.Filters.Add(new CommonFileDialogFilter("All Files", "*.*"));

            return ofd;
        }

        public SelectIconDialog()
        {
            InitializeComponent();
            DataContext = this;
        }

        public event PropertyChangedEventHandler? PropertyChanged;

        private void ButtonBrowse_Click(object sender, RoutedEventArgs e)
        {
            CommonOpenFileDialog ofd = CreateOpenFileDialog("", Filename);
            ofd.Title = Title;
            if (ofd.ShowDialog(this) == CommonFileDialogResult.Ok)
            {
                Filename = ofd.FileName;
            }
        }

        private void ButtonCancel_Click(object sender, RoutedEventArgs e)
        {
            IconsList.SelectedItem = null;
            DialogResult = false;
            Close();
        }

        private void ButtonSelect_Click(object sender, RoutedEventArgs e)
        {
            DialogResult = Id >= 0;
            Close();
        }

    }
}
