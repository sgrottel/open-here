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
using Microsoft.Win32;
using Microsoft.WindowsAPICodePack.Dialogs;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace OpenHere.SettingsApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window, INotifyPropertyChanged
    {
        public SettingsBase.Config Config { get; set; } = new SettingsBase.Config();
        public ObservableCollection<ToolInfoView> Tools { get; } = new ObservableCollection<ToolInfoView>();

        #region static text info

        public string VersionInfo
        {
            get
            {
                string verInfo = "";

                var ea = System.Reflection.Assembly.GetExecutingAssembly();

                object[] attrs = ea.GetCustomAttributes(typeof(AssemblyDescriptionAttribute), true);
                if (attrs != null && attrs.Length > 0)
                {
                    verInfo += ((AssemblyDescriptionAttribute)attrs[0]).Description + "\n";
                }

                attrs = ea.GetCustomAttributes(typeof(AssemblyCopyrightAttribute), true);
                if (attrs != null && attrs.Length > 0)
                {
                    verInfo += ((AssemblyCopyrightAttribute)attrs[0]).Copyright + "\n";
                }

                verInfo += "Version: ";
                attrs = ea.GetCustomAttributes(typeof(AssemblyFileVersionAttribute), true);
                if (attrs != null && attrs.Length > 0)
                {
                    verInfo += ((AssemblyFileVersionAttribute)attrs[0]).Version;
                }
                else
                {
                    verInfo += ea.GetName().Version?.ToString() ?? "?";
                }

                return verInfo;
            }
        }
        public string QuickHelp
        {
            get
            {
                try
                {
                    var assembly = Assembly.GetExecutingAssembly();
                    string resourceName = assembly.GetManifestResourceNames()
                        .Single(str => str.EndsWith("QuickHelp.txt", StringComparison.OrdinalIgnoreCase));
                    using (Stream? stream = assembly.GetManifestResourceStream(resourceName))
                    {
                        if (stream == null) throw new ArgumentNullException();
                        using (StreamReader? reader = new StreamReader(stream))
                        {
                            return reader.ReadToEnd();
                        }
                    }
                }
                catch { }
                return "";
            }
        }
        public string ProjectUrl
        {
            get
            {
                return "https://go.grottel.net/open-here";
            }
        }

        #endregion

        public event PropertyChangedEventHandler? PropertyChanged;

        public MainWindow()
        {
            InitializeComponent();
            DataContext = this;
        }

        private double GetUIScaling()
        {
            Matrix m = PresentationSource.FromVisual(Application.Current.MainWindow).CompositionTarget.TransformToDevice;
            double dx = m.M11;
            double dy = m.M22;
            return (dx + dy) * 0.5;
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            ButtonLoad_Click(null, null);
            ToolsList_SelectionChanged(null, null);
            IconSizeUtility.UiScaling = (float)GetUIScaling();
        }

        private void ButtonLoad_Click(object? sender, RoutedEventArgs? e)
        {
            var config = new SettingsBase.Config();
            try
            {
                config.Load();
                Config = config;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("Config"));
            }
            catch { }

            try
            {
                var loader = new SettingsBase.ToolInfoListLoader();
                var tools = loader.Load();

                Tools.Clear();
                foreach (var t in tools)
                {
                    ToolInfoView tiv = new ToolInfoView(Tools);
                    tiv.AssignFrom(t);
                    Tools.Add(tiv);
                }
            }
            catch { }
        }

        private void ButtonSave_Click(object sender, RoutedEventArgs e)
        {
            if (MessageBox.Show("Current configuration will be overwritten.\nOperation cannot be undone.\nProceed?",
                "Open Here Save...", MessageBoxButton.YesNo, MessageBoxImage.Question) == MessageBoxResult.Yes)
            {
                try
                {
                    Config.Save();

                    var loader = new SettingsBase.ToolInfoListLoader();
                    loader.Save(Tools.ToArray());

                    MessageBox.Show("Save successful.", "Open Here Save...", MessageBoxButton.OK, MessageBoxImage.Information);
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Save failed: " + ex.ToString(), "Open Here Save...", MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
        }

        private void HyperlinkProjectUrl_Click(object sender, RoutedEventArgs e)
        {
            Process.Start(new ProcessStartInfo(ProjectUrl) { UseShellExecute = true });
        }

        private void ButtonImport_Click(object sender, RoutedEventArgs e)
        {
            CommonOpenFileDialog ofd = new CommonOpenFileDialog();
            ofd.Title = "Open Here Import...";
            ofd.RestoreDirectory = false;
            ofd.EnsureFileExists = true;
            ofd.AllowNonFileSystemItems = false;
            ofd.Filters.Add(new CommonFileDialogFilter("Xml Files", ".xml"));
            ofd.Filters.Add(new CommonFileDialogFilter("All Files", "*.*"));

            if (ofd.ShowDialog(this) == CommonFileDialogResult.Ok)
            {
                try
                {
                    ImportExport ie = new ImportExport();
                    ie.Import(@"C:\temp\export.xml");

                    Config = ie?.Config ?? new SettingsBase.Config();
                    PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("Config"));

                    Tools.Clear();
                    if (ie?.Tools != null)
                    {
                        foreach (var t in ie.Tools)
                        {
                            ToolInfoView tiv = new ToolInfoView(Tools);
                            Tools.Add(tiv);
                            tiv.AssignFrom(t);
                        }
                    }

                    MessageBox.Show("Import successful.", ofd.Title, MessageBoxButton.OK, MessageBoxImage.Information);
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Failed to import settings: " + ex.ToString(), ofd.Title, MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
        }

        private void ButtonExport_Click(object sender, RoutedEventArgs e)
        {
            CommonSaveFileDialog sfd = new CommonSaveFileDialog();
            sfd.Title = "Open Here Export...";
            sfd.RestoreDirectory = false;
            sfd.EnsurePathExists = true;
            sfd.Filters.Add(new CommonFileDialogFilter("Xml Files", ".xml"));
            sfd.Filters.Add(new CommonFileDialogFilter("All Files", "*.*"));

            if (sfd.ShowDialog(this) == CommonFileDialogResult.Ok)
            {
                ImportExport ie = new ImportExport();
                ie.Config = Config;
                ie.Tools = new SettingsBase.ToolInfo[Tools.Count];
                for (int i = 0; i < Tools.Count; ++i)
                {
                    ie.Tools[i] = Tools[i].ToToolInfo();
                }
                ie.Export(sfd.FileName);

                MessageBox.Show("Export successful.", sfd.Title, MessageBoxButton.OK, MessageBoxImage.Information);
            }
        }

        private void ButtonOpenConfigPath_Click(object sender, RoutedEventArgs e)
        {
            Process.Start(new ProcessStartInfo(Config.Path()) { UseShellExecute = true });
        }

        private void ToolsList_SelectionChanged(object? sender, SelectionChangedEventArgs? e)
        {
            ToolInfoView? tool = ToolsList.SelectedItem as ToolInfoView;
            ButtonRemoveTool.IsEnabled = tool != null;

            if (tool != null)
            {
                if (!ToolConfigTab.IsEnabled)
                {
                    ToolConfigTab.IsEnabled = true;
                    ((TabControl)ToolConfigTab.Parent).SelectedItem = ToolConfigTab;
                }
            }
            else
            {
                if (ToolConfigTab.IsEnabled)
                {
                    ToolConfigTab.IsEnabled = false;
                    TabControl tc = (TabControl)ToolConfigTab.Parent;
                    if (tc.SelectedItem == ToolConfigTab)
                    {
                        tc.SelectedItem = AppSettingsTab;
                    }
                }
            }

            StartConfigList_SelectionChanged(null, null);
            if (tool != null)
            {
                tool.PropertyChanged += Tool_PropertyChanged;
            }
        }

        private ObservableCollection<SettingsBase.ToolStartConfig>? observingStartConfigs = null;

        private void Tool_PropertyChanged(object? sender, PropertyChangedEventArgs e)
        {
            ToolInfoView? tool = sender as ToolInfoView;
            if (tool == null) return;

            if (observingStartConfigs != tool.StartConfigs)
            {
                if (observingStartConfigs != null)
                {
                    observingStartConfigs.CollectionChanged -= ObservingStartConfigs_CollectionChanged;
                }
                observingStartConfigs = tool.StartConfigs;
                if (observingStartConfigs != null)
                {
                    observingStartConfigs.CollectionChanged += ObservingStartConfigs_CollectionChanged;
                }
            }
        }

        private void ObservingStartConfigs_CollectionChanged(object? sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            StartConfigList_SelectionChanged(null, null);
        }

        private void ButtonAddTool_Click(object sender, RoutedEventArgs e)
        {
            ToolInfoView t = new ToolInfoView(Tools)
            {
                Title = "New Tool"
            };

            if (ToolsList.SelectedIndex < 0)
            {
                Tools.Add(t);
            }
            else
            {
                Tools.Insert(ToolsList.SelectedIndex + 1, t);
            }
            ToolsList.SelectedItem = t;
        }

        private void ButtonRemoveTool_Click(object sender, RoutedEventArgs e)
        {
            ToolInfoView? tool = ToolsList.SelectedItem as ToolInfoView;
            if (tool != null)
            {
                int idx = ToolsList.SelectedIndex;
                Tools.Remove(tool);
                ToolsList.SelectedIndex = Math.Min(idx, Tools.Count - 1);
            }
        }

        private void ButtonToolToTop_Click(object sender, RoutedEventArgs e)
        {
            ToolInfoView? tool = (sender as Button)?.DataContext as ToolInfoView;
            if (tool == null) return;
            int oIdx = Tools.IndexOf(tool);
            if (oIdx > 0)
            {
                Tools.Move(oIdx, 0);
            }
        }

        private void ButtonToolUp_Click(object sender, RoutedEventArgs e)
        {
            ToolInfoView? tool = (sender as Button)?.DataContext as ToolInfoView;
            if (tool == null) return;
            int oIdx = Tools.IndexOf(tool);
            if (oIdx > 0)
            {
                Tools.Move(oIdx, oIdx - 1);
            }
        }

        private void ButtonToolDown_Click(object sender, RoutedEventArgs e)
        {
            ToolInfoView? tool = (sender as Button)?.DataContext as ToolInfoView;
            if (tool == null) return;
            int oIdx = Tools.IndexOf(tool);
            if (oIdx >= 0 && oIdx + 1 < Tools.Count)
            {
                Tools.Move(oIdx, oIdx + 1);
            }
        }

        private void ButtonToolToBottom_Click(object sender, RoutedEventArgs e)
        {
            ToolInfoView? tool = (sender as Button)?.DataContext as ToolInfoView;
            if (tool == null) return;
            int oIdx = Tools.IndexOf(tool);
            if (oIdx >= 0 && oIdx + 1 < Tools.Count)
            {
                Tools.Move(oIdx, Tools.Count - 1);
            }
        }

        // Helper to search up the VisualTree
        private static T? FindAnchestor<T>(DependencyObject? current)
            where T : DependencyObject
        {
            while (current != null)
            {
                if (current is T)
                {
                    return (T)current;
                }
                current = VisualTreeHelper.GetParent(current);
            }
            return null;
        }

        // Helper to search up the VisualTree
        private static T? FindNonControlAnchestor<T>(DependencyObject? current)
            where T : DependencyObject
        {
            while (current != null)
            {
                if (current is T)
                {
                    return (T)current;
                }
                if (current is Control) // abort if control is in path
                {
                    return null;
                }
                current = VisualTreeHelper.GetParent(current);
            }
            return null;
        }

        private void ToolsList_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            ListViewItem? lvi = FindNonControlAnchestor<ListViewItem>(e.OriginalSource as DependencyObject);
            if (lvi == null) return;
            ToolInfoView? tiv = ToolsList.ItemContainerGenerator.ItemFromContainer(lvi) as ToolInfoView;
            if (tiv == null) return;

            if (ToolsList.SelectedItem != tiv)
            {
                ToolsList.SelectedItem = tiv;
            }

            ToolsList.AllowDrop = true;
            DragDrop.DoDragDrop(ToolsList, tiv, DragDropEffects.Move);
        }

        private void ToolsList_Drop(object sender, DragEventArgs e)
        {
            ToolsList.AllowDrop = false;

            ToolInfoView? fromTiv = e.Data.GetData(typeof(ToolInfoView)) as ToolInfoView;
            if (fromTiv == null) return;
            int fromIdx = Tools.IndexOf(fromTiv);

            int toIdx = Tools.Count - 1;
            ListViewItem? lvi = FindNonControlAnchestor<ListViewItem>(e.OriginalSource as DependencyObject);
            if (lvi != null)
            {
                ToolInfoView? toTiv = ToolsList.ItemContainerGenerator.ItemFromContainer(lvi) as ToolInfoView;
                if (toTiv != null)
                {
                    toIdx = Tools.IndexOf(toTiv);
                }
            }

            if (fromIdx == toIdx) return;
            Tools.Move(fromIdx, toIdx);
        }

        private void ButtonAddStartConfig_Click(object sender, RoutedEventArgs e)
        {
            ObservableCollection<SettingsBase.ToolStartConfig>? startConfigs = (ToolsList.SelectedItem as ToolInfoView)?.StartConfigs;
            if (startConfigs == null) return;
            startConfigs.Add(new SettingsBase.ToolStartConfig());
            StartConfigList_SelectionChanged(null, null);
        }

        private void ButtonRemoveStartConfig_Click(object sender, RoutedEventArgs e)
        {
            ObservableCollection<SettingsBase.ToolStartConfig>? startConfigs = (ToolsList.SelectedItem as ToolInfoView)?.StartConfigs;
            if (startConfigs == null) return;
            SettingsBase.ToolStartConfig? ssc = StartConfigList.SelectedItem as SettingsBase.ToolStartConfig;
            if (ssc == null) return;
            startConfigs.Remove(ssc);
            StartConfigList_SelectionChanged(null, null);
        }

        private void ButtonStartConfigToTop_Click(object sender, RoutedEventArgs e)
        {
            ObservableCollection<SettingsBase.ToolStartConfig>? startConfigs = (ToolsList.SelectedItem as ToolInfoView)?.StartConfigs;
            if (startConfigs == null) return;
            SettingsBase.ToolStartConfig? ssc = StartConfigList.SelectedItem as SettingsBase.ToolStartConfig;
            if (ssc == null) return;
            int idx = startConfigs.IndexOf(ssc);
            if (idx > 0)
            {
                startConfigs.Move(idx, 0);
                StartConfigList_SelectionChanged(null, null);
            }
        }

        private void ButtonStartConfigUp_Click(object sender, RoutedEventArgs e)
        {
            ObservableCollection<SettingsBase.ToolStartConfig>? startConfigs = (ToolsList.SelectedItem as ToolInfoView)?.StartConfigs;
            if (startConfigs == null) return;
            SettingsBase.ToolStartConfig? ssc = StartConfigList.SelectedItem as SettingsBase.ToolStartConfig;
            if (ssc == null) return;
            int idx = startConfigs.IndexOf(ssc);
            if (idx > 0)
            {
                startConfigs.Move(idx, idx - 1);
                StartConfigList_SelectionChanged(null, null);
            }
        }

        private void ButtonStartConfigDown_Click(object sender, RoutedEventArgs e)
        {
            ObservableCollection<SettingsBase.ToolStartConfig>? startConfigs = (ToolsList.SelectedItem as ToolInfoView)?.StartConfigs;
            if (startConfigs == null) return;
            SettingsBase.ToolStartConfig? ssc = StartConfigList.SelectedItem as SettingsBase.ToolStartConfig;
            if (ssc == null) return;
            int idx = startConfigs.IndexOf(ssc);
            if (idx >= 0 && idx + 1 < startConfigs.Count)
            {
                startConfigs.Move(idx, idx + 1);
                StartConfigList_SelectionChanged(null, null);
            }
        }

        private void ButtonStartConfigToBottom_Click(object sender, RoutedEventArgs e)
        {
            ObservableCollection<SettingsBase.ToolStartConfig>? startConfigs = (ToolsList.SelectedItem as ToolInfoView)?.StartConfigs;
            if (startConfigs == null) return;
            SettingsBase.ToolStartConfig? ssc = StartConfigList.SelectedItem as SettingsBase.ToolStartConfig;
            if (ssc == null) return;
            int idx = startConfigs.IndexOf(ssc);
            if (idx >= 0 && idx + 1 < startConfigs.Count)
            {
                startConfigs.Move(idx, startConfigs.Count - 1);
                StartConfigList_SelectionChanged(null, null);
            }
        }

        private void StartConfigList_SelectionChanged(object? sender, SelectionChangedEventArgs? e)
        {
            ButtonRemoveStartConfig.IsEnabled = StartConfigList.SelectedItem != null;
            ButtonStartConfigToTop.IsEnabled =
                ButtonStartConfigUp.IsEnabled = StartConfigList.SelectedIndex > 0;
            ButtonStartConfigDown.IsEnabled =
                ButtonStartConfigToBottom.IsEnabled = StartConfigList.SelectedIndex >= 0 && StartConfigList.SelectedIndex < StartConfigList.Items.Count - 1;
        }

        private void StartConfigList_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            ListViewItem? lvi = FindNonControlAnchestor<ListViewItem>(e.OriginalSource as DependencyObject);
            if (lvi == null) return;
            SettingsBase.ToolStartConfig? tsc = StartConfigList.ItemContainerGenerator.ItemFromContainer(lvi) as SettingsBase.ToolStartConfig;
            if (tsc == null) return;

            if (StartConfigList.SelectedItem != tsc)
            {
                StartConfigList.SelectedItem = tsc;
            }

            StartConfigList.AllowDrop = true;
            DragDrop.DoDragDrop(StartConfigList, tsc, DragDropEffects.Move);
        }

        private void StartConfigList_Drop(object sender, DragEventArgs e)
        {
            StartConfigList.AllowDrop = false;
            ObservableCollection<SettingsBase.ToolStartConfig>? startConfigs = (ToolsList.SelectedItem as ToolInfoView)?.StartConfigs;
            if (startConfigs == null) return;

            SettingsBase.ToolStartConfig? fromTsc = e.Data.GetData(typeof(SettingsBase.ToolStartConfig)) as SettingsBase.ToolStartConfig;
            if (fromTsc == null) return;
            int fromIdx = startConfigs.IndexOf(fromTsc);

            int toIdx = startConfigs.Count - 1;
            ListViewItem? lvi = FindNonControlAnchestor<ListViewItem>(e.OriginalSource as DependencyObject);
            if (lvi != null)
            {
                SettingsBase.ToolStartConfig? toTsc = StartConfigList.ItemContainerGenerator.ItemFromContainer(lvi) as SettingsBase.ToolStartConfig;
                if (toTsc != null)
                {
                    toIdx = startConfigs.IndexOf(toTsc);
                }
            }

            if (fromIdx == toIdx) return;
            startConfigs.Move(fromIdx, toIdx);
            StartConfigList_SelectionChanged(null, null);
        }

        private void ButtonToolStartConfigExecutableBrowse_Click(object sender, RoutedEventArgs e)
        {
            ToolInfoView? tool = ToolsList.SelectedItem as ToolInfoView;
            ListViewItem? lvi = FindAnchestor<ListViewItem>(sender as DependencyObject);
            SettingsBase.ToolStartConfig? sel = (lvi != null) ? StartConfigList.ItemContainerGenerator.ItemFromContainer(lvi) as SettingsBase.ToolStartConfig : null;
            if (tool == null || sel == null) return;

            CommonOpenFileDialog ofd = new CommonOpenFileDialog();
            ofd.Title = tool.Title + " Executable...";
            ofd.DefaultFileName = sel.Executable;
            ofd.RestoreDirectory = false;
            if (!string.IsNullOrWhiteSpace(sel.Executable))
            {
                ofd.InitialDirectory = System.IO.Path.GetDirectoryName(sel.Executable);
            }

            if (ofd.ShowDialog(this) == CommonFileDialogResult.Ok)
            {
                sel.Executable = ofd.FileName;
            }
        }

        private void ButtonToolStartConfigWorkingDirectoryBrowse_Click(object sender, RoutedEventArgs e)
        {
            ToolInfoView? tool = ToolsList.SelectedItem as ToolInfoView;
            ListViewItem? lvi = FindAnchestor<ListViewItem>(sender as DependencyObject);
            SettingsBase.ToolStartConfig? sel = (lvi != null) ? StartConfigList.ItemContainerGenerator.ItemFromContainer(lvi) as SettingsBase.ToolStartConfig : null;
            if (tool == null || sel == null) return;

            CommonOpenFileDialog ofd = new CommonOpenFileDialog();
            ofd.Title = tool.Title + " Working Directory...";
            ofd.IsFolderPicker = true;
            ofd.RestoreDirectory = false;
            if (!string.IsNullOrWhiteSpace(sel.WorkingDirectory))
            {
                ofd.InitialDirectory = sel.WorkingDirectory;
            }

            if (ofd.ShowDialog(this) == CommonFileDialogResult.Ok)
            {
                sel.WorkingDirectory = ofd.FileName;
            }

        }

        private void ButtonBrowseToolIcon_Click(object sender, RoutedEventArgs e)
        {
            ToolInfoView? tool = ToolsList.SelectedItem as ToolInfoView;
            if (tool == null) return;

            CommonOpenFileDialog ofd = SelectIconDialog.CreateOpenFileDialog(tool.Title, tool.IconFile);
            if (ofd.ShowDialog(this) == CommonFileDialogResult.Ok)
            {
                try
                {
                    int size = IconSizeUtility.IconSize(48);
                    SettingsBase.IconLibrary lib = new SettingsBase.IconLibrary();
                    lib.Open(ofd.FileName, size, size);

                    if (lib.GetCount() <= 0)
                    {
                        throw new Exception("File does not contain any icons");
                    }
                    else if (lib.GetCount() == 1)
                    {
                        tool.IconFile = ofd.FileName;
                        tool.IconId = (int)lib.GetId(0);
                    }
                    else if (lib.GetCount() > 1)
                    {
                        SelectIconDialog dlg = new SelectIconDialog();
                        dlg.Title = SelectIconDialog.DialogTitle(tool.Title);
                        dlg.Filename = ofd.FileName;
                        dlg.Id = (int)lib.GetId(0);
                        for (uint i = 0; i < lib.GetCount(); ++i)
                        {
                            if (lib.GetId(i) == tool.IconId)
                            {
                                dlg.Id = tool.IconId;
                                break;
                            }
                        }
                        dlg.Owner = this;

                        if (dlg.ShowDialog() ?? false)
                        {
                            tool.IconFile = dlg.Filename;
                            tool.IconId = dlg.Id;
                        }
                    }
                }
                catch(Exception ex)
                {
                    MessageBox.Show("Failed to browse icon: " + ex, ofd.Title, MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }

        }

        private void ButtonToolPreview_Click(object sender, RoutedEventArgs e)
        {
            ToolInfoView? tool = ToolsList.SelectedItem as ToolInfoView;
            if (tool == null) return;

            try
            {
                SettingsBase.FileExplorer.Detector det = new SettingsBase.FileExplorer.Detector();
                if (det.Detect() < 1)
                {
                    throw new Exception("No file explorer detected");
                }

                var fe = det.Instances[0];
                if (fe == null) throw new Exception();
                string fileExpInfo = fe.InstanceType;
                string path = "";
                if (fe.OpenPaths != null && fe.OpenPaths.Length > 0)
                {
                    path = fe.OpenPaths[0];
                    fileExpInfo += "\n  Path: " + path;
                }
                string[] files = new string[0];
                if (fe.SelectedItems != null)
                {
                    files = fe.SelectedItems;
                    fileExpInfo += "\n  " + files.Length + " file" + (files.Length == 1 ? "" : "s") + ": "
                        + string.Join(", ", files.Select((string s) => { return System.IO.Path.GetFileName(s); }));
                }

                SettingsBase.ToolRunner runner = new SettingsBase.ToolRunner();
                var startConfig = runner.SelectStartConfig(tool, path, files, true);
                if (startConfig == null)
                {
                    throw new Exception("No Start Configuration selected");
                }
                startConfig = new SettingsBase.ToolStartConfig() {
                    PathRequirement = startConfig.PathRequirement,
                    FilesRequirement = startConfig.FilesRequirement,
                    Executable = startConfig.Executable,
                    Arguments = (string[])startConfig.Arguments.Clone(),
                    WorkingDirectory = startConfig.WorkingDirectory
                };

                runner.ApplyVariables(ref startConfig, path, files);

                tool.PreviewString = fileExpInfo
                    + "\n\nCalling:\n"
                    + "\"" + startConfig.Executable + "\" "
                    + string.Join(" ", startConfig.Arguments.Select((string f) => { return "\"" + f.Replace("\"", "\\\"") + "\""; }))
                    + "\nWorking Directory:\n"
                    + startConfig.WorkingDirectory;

            }
            catch(Exception ex)
            {
                tool.PreviewString = "Failed to preview: " + ex;
            }
            ToolPreview.IsExpanded = true;
        }
    }

}
