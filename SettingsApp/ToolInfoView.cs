using Microsoft.VisualBasic;
using OpenHere.SettingsBase;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Threading;

namespace OpenHere.SettingsApp
{

    /// <summary>
    /// ViewModel version of ToolInfo
    /// </summary>
    public class ToolInfoView : SettingsBase.ToolInfo
    {
        private WeakReference<ObservableCollection<ToolInfoView>> collection;

        public int Index { get; private set; } = -1;

        public int Position { get { return Index + 1; } }

        public int Size { get; private set; } = -1;

        internal ToolInfoView(ObservableCollection<ToolInfoView> col)
        {
            collection = new WeakReference<ObservableCollection<ToolInfoView>>(col);
            col.CollectionChanged += Col_CollectionChanged;
            IconLoadDebouncer.Enabled = false;
            IconLoadDebouncer.Interval = 250;
            IconLoadDebouncer.Elapsed += IconLoadDebouncer_Elapsed;
        }

        private void Col_CollectionChanged(object? sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            int index = -1, size = -1;

            ObservableCollection<ToolInfoView>? col;
            if (collection.TryGetTarget(out col))
            {
                size = col.Count;
                index = col.IndexOf(this);
            }

            bool update = false;
            if (index != Index)
            {
                update = true;
                Index = index;
                OnPropertyChanged(nameof(Index));
                OnPropertyChanged(nameof(Position));
                OnPropertyChanged(nameof(Pos0Brush));
                OnPropertyChanged(nameof(Pos1Brush));
                OnPropertyChanged(nameof(Pos2Brush));
                OnPropertyChanged(nameof(Pos3Brush));
                OnPropertyChanged(nameof(Pos4Brush));
                OnPropertyChanged(nameof(Pos5Brush));
                OnPropertyChanged(nameof(Pos6Brush));
                OnPropertyChanged(nameof(Pos7Brush));
                OnPropertyChanged(nameof(Pos8Brush));
                OnPropertyChanged(nameof(Pos9Brush));
                OnPropertyChanged(nameof(Pos10Brush));
                OnPropertyChanged(nameof(Pos11Brush));
            }
            if (size != Size)
            {
                update = true;
                Size = size;
                OnPropertyChanged(nameof(Size));
            }

            if (update)
            {
                OnPropertyChanged(nameof(CanGoUp));
                OnPropertyChanged(nameof(CanGoDown));
            }
        }

        public string Description
        {
            get
            {
                if (string.IsNullOrEmpty(Title))
                {
                    return "Spacer";
                }

                int cnt = StartConfigs?.Count ?? 0;
                if (cnt <= 0)
                {
                    return "No Start Configurations";
                }

                string d = string.Format("{0} Start Configuration{1}", cnt, (cnt != 1) ? "s" : "");

                var pr = base.CombinedPathRequirement();
                switch (pr)
                {
                    case SettingsBase.ToolPathRequirement.Required:
                        d += ", Path must be selected";
                        break;
                    case SettingsBase.ToolPathRequirement.RequiredNull:
                        d += ", No path must be selected";
                        break;
                }

                var fr = base.CombinedFilesRequirement();
                switch (fr)
                {
                    case SettingsBase.ToolFilesRequirement.RequireOne:
                        d += ", One file must be selected";
                        break;
                    case SettingsBase.ToolFilesRequirement.RequireOneOrMore:
                        d += ", One or more files must be selected";
                        break;
                    case SettingsBase.ToolFilesRequirement.RequireTwoOrMore:
                        d += ", More than one file must be selected";
                        break;
                    case SettingsBase.ToolFilesRequirement.RequireNull:
                        d += ", No files must be selected";
                        break;
                }

                return d;
            }
        }

        public string CombinedIcon
        {
            get
            {
                string s = IconFile;
                if (IconId != 0)
                {
                    s += "|" + IconId;
                }
                return s;
            }
            set
            {
                string[] frags = value.Split(new char[] { '|' }, 2);
                int id = 0;
                if (frags.Length == 2)
                {
                    if (!int.TryParse(frags[1], out id)) return;
                }
                IconFile = frags[0];
                IconId = id;
            }
        }

        public bool CanGoUp
        {
            get { return Index > 0; }
        }

        public bool CanGoDown
        {
            get { return Index + 1 < Size; }
        }

        #region Placement Icon Utility

        private Brush highlightBrush = System.Windows.SystemColors.HighlightBrush;
        private Brush controlBrush = System.Windows.SystemColors.ControlBrush;

        public Brush Pos0Brush
        {
            get { return (Index % 12 == 0) ? highlightBrush : controlBrush; }
        }
        public Brush Pos1Brush
        {
            get { return (Index % 12 == 1) ? highlightBrush : controlBrush; }
        }
        public Brush Pos2Brush
        {
            get { return (Index % 12 == 2) ? highlightBrush : controlBrush; }
        }
        public Brush Pos3Brush
        {
            get { return (Index % 12 == 3) ? highlightBrush : controlBrush; }
        }
        public Brush Pos4Brush
        {
            get { return (Index % 12 == 4) ? highlightBrush : controlBrush; }
        }
        public Brush Pos5Brush
        {
            get { return (Index % 12 == 5) ? highlightBrush : controlBrush; }
        }
        public Brush Pos6Brush
        {
            get { return (Index % 12 == 6) ? highlightBrush : controlBrush; }
        }
        public Brush Pos7Brush
        {
            get { return (Index % 12 == 7) ? highlightBrush : controlBrush; }
        }
        public Brush Pos8Brush
        {
            get { return (Index % 12 == 8) ? highlightBrush : controlBrush; }
        }
        public Brush Pos9Brush
        {
            get { return (Index % 12 == 9) ? highlightBrush : controlBrush; }
        }
        public Brush Pos10Brush
        {
            get { return (Index % 12 == 10) ? highlightBrush : controlBrush; }
        }
        public Brush Pos11Brush
        {
            get { return (Index % 12 == 11) ? highlightBrush : controlBrush; }
        }

        #endregion

        protected override void raise_PropertyChanged(object sender, PropertyChangedEventArgs arg)
        {
            base.raise_PropertyChanged(sender, arg);

            if (arg == null
                || arg.PropertyName == null
                || arg.PropertyName == nameof(Title)
                || arg.PropertyName == nameof(StartConfigs))
            {
                base.raise_PropertyChanged(sender, new PropertyChangedEventArgs(nameof(Description)));
            }

            if (arg != null && arg.PropertyName != null
                && (arg.PropertyName == nameof(IconFile) || arg.PropertyName == nameof(IconId)))
            {
                IconLoadDebouncer.Stop();
                IconLoadDebouncer.Start();
                OnPropertyChanged(nameof(CombinedIcon));
            }
        }

        #region Observe StartConfigs to Trigger Description Update

        protected override void PreStartConfigsChange()
        {
            base.PreStartConfigsChange();
            if (StartConfigs != null)
            {
                foreach (var sc in StartConfigs)
                {
                    sc.PropertyChanged -= ToolStartConfig_PropertyChanged;
                }
                StartConfigs.CollectionChanged -= StartConfigs_CollectionChanged;
            }
        }

        protected override void PostStartConfigsChange()
        {
            base.PostStartConfigsChange();
            if (StartConfigs != null)
            {
                StartConfigs.CollectionChanged += StartConfigs_CollectionChanged;
                foreach (var sc in StartConfigs)
                {
                    sc.PropertyChanged += ToolStartConfig_PropertyChanged;
                }
            }
        }

        private void StartConfigs_CollectionChanged(object? sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            if (e.Action == System.Collections.Specialized.NotifyCollectionChangedAction.Move)
            {
                return;
            }

            if (e.OldItems != null)
            {
                foreach (var gi in e.OldItems)
                {
                    var i = gi as SettingsBase.ToolStartConfig;
                    if (i == null) continue;
                    i.PropertyChanged -= ToolStartConfig_PropertyChanged;
                }
            }
            if (e.NewItems != null)
            {
                foreach (var gi in e.NewItems)
                {
                    var i = gi as SettingsBase.ToolStartConfig;
                    if (i == null) continue;
                    i.PropertyChanged += ToolStartConfig_PropertyChanged;
                }
            }

            ToolStartConfig_PropertyChanged(null, null);
        }

        private void ToolStartConfig_PropertyChanged(object? sender, PropertyChangedEventArgs? e)
        {
            OnPropertyChanged(nameof(Description));
        }

        #endregion

        public void AssignFrom(SettingsBase.ToolInfo tool)
        {
            Title = tool.Title;
            IconFile = tool.IconFile;
            IconId = tool.IconId;
            StartConfigs = tool.StartConfigs; // no need to deep-copy, for now
            tool.StartConfigs = null;
        }

        public ImageSource? Icon { get; private set; }

        private System.Timers.Timer IconLoadDebouncer = new System.Timers.Timer();

        private void SetIcon(ImageSource? i)
        {
            i?.Freeze();
            if (Icon != i)
            {
                Icon = i;
                OnPropertyChanged(nameof(Icon));
            }
        }

        private void LoadIcon()
        {
            if (string.IsNullOrWhiteSpace(IconFile))
            {
                SetIcon(null);
                return;
            }

            try
            {
                OpenHere.SettingsBase.IconLoader loader = new SettingsBase.IconLoader();
                ImageSource icon = loader.LoadFromIconFile(IconFile, IconId, IconSizeUtility.IconSize(48));
                SetIcon(icon);
            }
            catch
            {
                SetIcon(App.Current.Resources["brokenIcon"] as DrawingImage);
            }
        }

        private void IconLoadDebouncer_Elapsed(object? sender, System.Timers.ElapsedEventArgs e)
        {
            IconLoadDebouncer.Stop();
            Dispatcher.CurrentDispatcher.Invoke(this.LoadIcon);
        }

        internal ToolInfo ToToolInfo()
        {
            ToolInfo t = new ToolInfo();
            t.Title = Title;
            t.IconFile = IconFile;
            t.IconId = IconId;
            t.StartConfigs = StartConfigs;
            return t;
        }
    }

}
