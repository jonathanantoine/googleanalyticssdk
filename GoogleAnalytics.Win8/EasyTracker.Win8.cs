using System;
using System.Threading.Tasks;
using System.Xml;
using Windows.Networking.Connectivity;
using Windows.Storage;
using Windows.UI.Xaml;
using System.IO;

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
            InitTracker();
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
            if (suspended.HasValue && config.SessionTimeout.HasValue)
            {
                var suspendedAgo = DateTime.UtcNow.Subtract(suspended.Value);
                if (suspendedAgo > config.SessionTimeout.Value)
                {
                    tracker.StartSession = true;
                }
            }

            if (config.AutoAppLifetimeTracking)
            {
                tracker.SendEvent("app", "resume", null, 0);
            }
        }

        async void app_Suspending(object sender, Windows.ApplicationModel.SuspendingEventArgs e)
        {
            if (config.AutoAppLifetimeTracking)
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
            if (config.ReportUncaughtExceptions)
            {
                try
                {
                    tracker.SendException(e.Message, true);
                    await Dispatch();
                }
                catch { /* ignore */ }
            }
        }
    }
}
