﻿using System;
using System.IO;
using System.Threading.Tasks;
using System.Xml;
using Windows.Networking.Connectivity;
using Windows.Storage;
using Windows.UI.Xaml;

namespace GoogleAnalytics
{
    public sealed partial class EasyTracker
    {
        private EasyTracker()
        {
            ConfigPath = new Uri("ms-appx:///analytics.xml");
        }

        private void InitConfig(Uri configPath)
        {
            StorageFile.GetFileFromApplicationUriAsync(configPath).AsTask()
                .ContinueWith(t =>
                {
                    return t.Result.OpenStreamForReadAsync();
                })
                .Unwrap()
                .ContinueWith(t =>
                {
                    using (var stream = t.Result)
                    {
                        using (var reader = XmlReader.Create(stream))
                        {
                            InitConfig(reader);
                        }
                    }
                }).Wait(); // this MUST be synchronouse and we are only loading a local file so no need to go async
        }

        public void SetContext(Application ctx)
        {
            UpdateConnectionStatus();
            NetworkInformation.NetworkStatusChanged += NetworkInformation_NetworkStatusChanged;
            if (ctx != null)
            {
                ctx.UnhandledException += app_UnhandledException;
                ctx.Suspending += app_Suspending;
                ctx.Resuming += app_Resuming;
            }
            InitConfig(ConfigPath);
            PopulateMissingConfig();
            InitTracker();
        }

        void PopulateMissingConfig()
        {
            if (string.IsNullOrEmpty(Config.AppName))
            {
                Config.AppName = Windows.ApplicationModel.Package.Current.Id.Name;
            }
            if (string.IsNullOrEmpty(Config.AppVersion))
            {
                var version = Windows.ApplicationModel.Package.Current.Id.Version;
                Config.AppVersion = string.Format("{0}.{1}.{2}.{3}", version.Major, version.Minor, version.Build, version.Revision);
            }
        }
        
        void NetworkInformation_NetworkStatusChanged(object sender)
        {
            UpdateConnectionStatus();
        }

        private static void UpdateConnectionStatus()
        {
            var profile = NetworkInformation.GetInternetConnectionProfile();
            if (profile != null)
            {
                switch (profile.GetNetworkConnectivityLevel())
                {
                    case NetworkConnectivityLevel.InternetAccess:
                    case NetworkConnectivityLevel.ConstrainedInternetAccess:
                        GAServiceManager.Current.IsConnected = true;
                        break;
                    default:
                        GAServiceManager.Current.IsConnected = false;
                        break;
                }
            }
        }

        void app_Resuming(object sender, object e)
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
                tracker.SendEvent("app", "resume", null, 0);
            }
        }

        async void app_Suspending(object sender, Windows.ApplicationModel.SuspendingEventArgs e)
        {
            if (Config.AutoAppLifetimeTracking)
            {
                tracker.SendEvent("app", "suspend", null, 0);
            }

            suspended = DateTime.UtcNow;
            var deferral = e.SuspendingOperation.GetDeferral();
            await GAServiceManager.Current.Dispatch();
            deferral.Complete();
        }

        async void app_UnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            if (Config.ReportUncaughtExceptions)
            {
                try
                {
                    tracker.SendException(e.Message, !e.Handled);
                    await Dispatch();
                }
                catch { /* ignore */ }
            }
        }
    }
}
