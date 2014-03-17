using GoogleAnalytics.Core;
using System.IO.IsolatedStorage;
using System.Windows;

namespace GoogleAnalytics
{
    public sealed partial class AnalyticsEngine
    {
        const string Key_AppOptOut = "GoogleAnaltyics.AppOptOut";

        TrackerManager manager;

        static AnalyticsEngine current;

        public static AnalyticsEngine Current
        {
            get
            {
                if (current == null)
                {
                    current = new AnalyticsEngine();
                }
                return current;
            }
        }

        private AnalyticsEngine()
        {
            manager = new TrackerManager(new PlatformInfoProvider());
        }

        bool? appOptOut;
        public bool AppOptOut
        {
            get
            {
                if (appOptOut.HasValue) return appOptOut.Value;
                return GetAppOptOut();
            }
            set
            {
                if (!appOptOut.HasValue) GetAppOptOut();
                if (appOptOut.Value != value)
                {
                    appOptOut = value;
                    IsolatedStorageSettings.ApplicationSettings[Key_AppOptOut] = value;
                    IsolatedStorageSettings.ApplicationSettings.Save();
                    if (value) GAServiceManager.Current.Clear();
                }
            }
        }

        private bool GetAppOptOut()
        {
            if (IsolatedStorageSettings.ApplicationSettings.Contains(Key_AppOptOut))
            {
                appOptOut = (bool)IsolatedStorageSettings.ApplicationSettings[Key_AppOptOut];
            }
            else
            {
                appOptOut = false;
            }
            return appOptOut.Value;
        }

        public bool RequestAppOptOutAsync()
        {
            var dialogResult = MessageBox.Show("Allow anonomous information to be collected to help improve this application?", "Help Improve User Experience", MessageBoxButton.OKCancel);
            var result = (dialogResult == MessageBoxResult.Cancel);
            AppOptOut = result;
            return result;
        }

        public bool IsDebugEnabled
        {
            get { return manager.IsDebugEnabled; }
            set { manager.IsDebugEnabled = value; }
        }

        public Tracker GetTracker(string propertyId)
        {
            return manager.GetTracker(propertyId);
        }

        public void CloseTracker(Tracker tracker)
        {
            manager.CloseTracker(tracker);
        }

        public Tracker DefaultTracker
        {
            get { return manager.DefaultTracker; }
            set { manager.DefaultTracker = value; }
        }
    }
}
