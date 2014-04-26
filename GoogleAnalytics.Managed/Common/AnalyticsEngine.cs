using GoogleAnalytics.Core;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
#if NETFX_CORE
using Windows.Storage;
using Windows.UI.Popups;
using Windows.Foundation;
#else
using System.Windows;
using System.IO.IsolatedStorage;
#endif

namespace GoogleAnalytics
{
    public sealed class AnalyticsEngine
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

        bool isAppOptOutSet;
        public bool AppOptOut
        {
            get
            {
                if (!isAppOptOutSet) LoadAppOptOut();
                return manager.AppOptOut;
            }
            set
            {
                manager.AppOptOut = value;
                isAppOptOutSet = true;
#if NETFX_CORE
                ApplicationData.Current.LocalSettings.Values[Key_AppOptOut] = value;
#else
                IsolatedStorageSettings.ApplicationSettings[Key_AppOptOut] = value;
                IsolatedStorageSettings.ApplicationSettings.Save();
#endif
                if (value) GAServiceManager.Current.Clear();
            }
        }

        private void LoadAppOptOut()
        {
#if NETFX_CORE
            if (ApplicationData.Current.LocalSettings.Values.ContainsKey(Key_AppOptOut))
            {
                manager.AppOptOut = (bool)ApplicationData.Current.LocalSettings.Values[Key_AppOptOut];
            }
#else
            if (IsolatedStorageSettings.ApplicationSettings.Contains(Key_AppOptOut))
            {
                manager.AppOptOut = (bool)IsolatedStorageSettings.ApplicationSettings[Key_AppOptOut];
            }
#endif
            else
            {
                manager.AppOptOut = false;
            }
            isAppOptOutSet = true;
        }

#if NETFX_CORE
        public IAsyncOperation<bool> RequestAppOptOutAsync()
        {
            return _RequestAppOptOutAsync().AsAsyncOperation();
        }

        async Task<bool> _RequestAppOptOutAsync()
        {
            MessageDialog msgDialog = new MessageDialog("Allow anonomous information to be collected to help improve this application?", "Help Improve User Experience");
            var optInCommand = new UICommand("Yes");
            var optOutCommand = new UICommand("No");
            msgDialog.Commands.Add(optInCommand);
            msgDialog.Commands.Add(optOutCommand);
            var dialogResult = await msgDialog.ShowAsync();
            var result = (dialogResult != optInCommand);
            AppOptOut = result;
            return result;
        }
#else
        public bool RequestAppOptOutAsync()
        {
            var dialogResult = MessageBox.Show("Allow anonomous information to be collected to help improve this application?", "Help Improve User Experience", MessageBoxButton.OKCancel);
            var result = (dialogResult == MessageBoxResult.Cancel);
            AppOptOut = result;
            return result;
        }
#endif

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
