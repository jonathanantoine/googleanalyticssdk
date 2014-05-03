using GoogleAnalytics.Core;
using Microsoft.Phone.Shell;
using System;
using System.Net.NetworkInformation;
using System.Threading.Tasks;
using System.Windows;
using System.Xml;

namespace GoogleAnalytics
{
    public sealed partial class EasyTracker
    {
        private EasyTracker()
        {
            ConfigPath = new Uri("analytics.xml", UriKind.Relative);
        }

        private void InitConfig(Uri configPath)
        {
            using (var stream = Application.GetResourceStream(configPath).Stream)
            {
                using (var reader = XmlReader.Create(stream))
                {
                    InitConfig(reader);
                }
            }
        }

        public void SetContext(Application ctx)
        {
            if (Config == null) InitConfig(ConfigPath);
            PopulateMissingConfig();

            if (Config.ReportUncaughtExceptions && ctx != null)
            {
                ctx.UnhandledException += app_UnhandledException;
                TaskScheduler.UnobservedTaskException += TaskScheduler_UnobservedTaskException;
            }
            if (Config.AutoTrackNetworkConnectivity)
            {
                UpdateConnectionStatus();
                NetworkChange.NetworkAddressChanged += NetworkChange_NetworkAddressChanged;
            }

            InitTracker();
            if (Config.AutoAppLifetimeMonitoring && ctx != null)
            {
                PhoneApplicationService.Current.Activated += Current_Activated;
                PhoneApplicationService.Current.Deactivated += Current_Deactivated;
            }
        }

        void PopulateMissingConfig()
        {
            if (string.IsNullOrEmpty(Config.AppName))
            {
                Config.AppName = Helpers.GetAppAttribute("Title");
            }
            if (string.IsNullOrEmpty(Config.AppVersion))
            {
                Config.AppVersion = Helpers.GetAppAttribute("Version");
            }
        }

        void Current_Activated(object sender, ActivatedEventArgs e)
        {
            if (suspended.HasValue && Config.SessionTimeout.HasValue)
            {
                var suspendedAgo = DateTime.UtcNow.Subtract(suspended.Value);
                if (suspendedAgo > Config.SessionTimeout.Value)
                {
                    tracker.SetStartSession(true);
                }
            }

            if (Config.AutoAppLifetimeTracking)
            {
                tracker.SendEvent("app", "resume", !e.IsApplicationInstancePreserved ? "tombstoned" : null, 0);
            }
        }

        async void Current_Deactivated(object sender, DeactivatedEventArgs e)
        {
            if (Config.AutoAppLifetimeTracking)
            {
#if WINDOWS_PHONE7
                tracker.SendEvent("app", "suspend", null, 0);
#else
                tracker.SendEvent("app", "suspend", e.Reason.ToString(), 0);
#endif
            }

            suspended = DateTime.UtcNow;
            await Dispatch(); // there is no way to get a deferral in WP so this will not actually happen until after we return to the app
        }

        void NetworkChange_NetworkAddressChanged(object sender, EventArgs e)
        {
            UpdateConnectionStatus();
        }

        private static void UpdateConnectionStatus()
        {
            GAServiceManager.Current.IsConnected = NetworkInterface.GetIsNetworkAvailable();
        }

        void TaskScheduler_UnobservedTaskException(object sender, UnobservedTaskExceptionEventArgs e)
        {
            var ex = e.Exception.InnerException ?? e.Exception; // inner exception contains better info for unobserved tasks
            if (e.Observed)
            {
                tracker.SendException(ex.ToString(), false);
            }
            else
            {
                //e.SetObserved();
                tracker.SendException(ex.ToString(), true);
                //await Dispatch();
                // rethrow the exception now that we're done logging it.
                //throw new TrackedException(e.Exception);
            }
        }

        async void app_UnhandledException(object sender, ApplicationUnhandledExceptionEventArgs e)
        {
            if (e.Handled)
            {
                tracker.SendException(e.ExceptionObject.ToString(), false);
            }
            else if (!(e.ExceptionObject is TrackedException))
            {
                e.Handled = true;
                tracker.SendException(e.ExceptionObject.ToString(), true);
                await Dispatch();
                // rethrow the exception now that we're done logging it.
                throw new TrackedException(e.ExceptionObject);
            }
        }

        public sealed class TrackedException : Exception
        {
            public TrackedException(Exception ex)
                : base("Exception rethrown after tracked by Google Analytics", ex)
            { }
        }

    }
}
