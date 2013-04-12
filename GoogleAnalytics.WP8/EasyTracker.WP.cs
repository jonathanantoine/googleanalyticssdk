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
            UpdateConnectionStatus();
            NetworkChange.NetworkAddressChanged += NetworkChange_NetworkAddressChanged;
            if (ctx != null)
            {
                ctx.UnhandledException += app_UnhandledException;
                PhoneApplicationService.Current.Activated += Current_Activated;
                PhoneApplicationService.Current.Deactivated += Current_Deactivated;
                PhoneApplicationService.Current.Closing += Current_Closing;
            }
            InitConfig(ConfigPath); // we are only loading a local file, no need to go async
            PopulateMissingConfig();
            InitTracker();
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
                    tracker.StartSession = true;
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
            await GAServiceManager.Current.Dispatch();
        }

        async void Current_Closing(object sender, ClosingEventArgs e)
        {
            if (Config.AutoAppLifetimeTracking)
            {
                tracker.EndSession = true;
                tracker.SendEvent("app", "close", null, 0);
            }

            await GAServiceManager.Current.Dispatch();
        }

        void NetworkChange_NetworkAddressChanged(object sender, EventArgs e)
        {
            UpdateConnectionStatus();
        }

        private static void UpdateConnectionStatus()
        {
            GAServiceManager.Current.IsConnected = NetworkInterface.GetIsNetworkAvailable();
        }

        async void app_UnhandledException(object sender, ApplicationUnhandledExceptionEventArgs e)
        {
            if (Config.ReportUncaughtExceptions)
            {
                try
                {
                    tracker.SendException(e.ExceptionObject.StackTrace, true);
                    await Dispatch();
                }
                catch { /* ignore */ }
            }
        }

    }
}
