﻿using Microsoft.Phone.Shell;
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
            await GAServiceManager.Current.Dispatch();
        }

        async void Current_Closing(object sender, ClosingEventArgs e)
        {
            if (Config.AutoAppLifetimeTracking)
            {
                tracker.SetEndSession(true);
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

        bool reportingException = false;
        async void app_UnhandledException(object sender, ApplicationUnhandledExceptionEventArgs e)
        {
            if (Config.ReportUncaughtExceptions)
            {
                if (!reportingException)
                {
                    if (e.Handled)
                    {
                        tracker.SendException(e.ExceptionObject.ToString(), false);
                    }
                    else
                    {
                        reportingException = true;
                        try
                        {
                            tracker.SendException(e.ExceptionObject.ToString(), true);
                            e.Handled = true;
                            await Dispatch();
                            // rethrow the exception now that we're done logging it. wrap in another exception in order to prevent stack trace from getting reset.
                            throw new Exception("Tracked exception rethrown", e.ExceptionObject);
                        }
                        finally
                        {
                            // we have to do some trickery in order to make sure the flag is reset only after the new exception has passed all the way through the UE pipeline. Otherwise we would have an infinite loop.
                            System.Windows.Deployment.Current.Dispatcher.BeginInvoke(async () =>
                            {
#if WINDOWS_PHONE7
                                await TaskEx.Yield();
#else
                                await Task.Yield();
#endif
                                reportingException = false;
                            });
                        }
                    }
                }
            }
        }

    }
}
