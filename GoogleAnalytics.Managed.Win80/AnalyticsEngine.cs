using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Storage;
using Windows.UI.Popups;

namespace GoogleAnalytics
{
    public class AnalyticsEngine : TrackerFactory
    {
        const string Key_AppOptOut = "GoogleAnaltyics.AppOptOut";

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

        private AnalyticsEngine() : base(new PlatformInfoProvider())
        { }
        
        bool? appOptOut;
        public override bool AppOptOut
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
                    ApplicationData.Current.LocalSettings.Values[Key_AppOptOut] = value;
                    if (value) GAServiceManager.Current.Clear();
                }
            }
        }

        private bool GetAppOptOut()
        {
            if (ApplicationData.Current.LocalSettings.Values.ContainsKey(Key_AppOptOut))
            {
                appOptOut = (bool)ApplicationData.Current.LocalSettings.Values[Key_AppOptOut];
            }
            else
            {
                appOptOut = false;
            }
            return appOptOut.Value;
        }

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
    }
}
