using GoogleAnalytics.Core;
using System;
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
                }).Wait(); // this MUST be synchronous and we are only loading a local file so no need to go async
        }

        public void SetContext(Application ctx)
        {
            if (Config == null) InitConfig(ConfigPath);
            PopulateMissingConfig();

            if (Config.ReportUncaughtExceptions && ctx != null)
            {
                ctx.UnhandledException += app_UnhandledException;
            }
            if (Config.AutoTrackNetworkConnectivity)
            {
                UpdateConnectionStatus();
                NetworkInformation.NetworkStatusChanged += NetworkInformation_NetworkStatusChanged;
            }

            InitTracker();

            if (Config.AutoAppLifetimeMonitoring && ctx != null)
            {
                ctx.Suspending += app_Suspending;
                ctx.Resuming += app_Resuming;
            }
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
            await Dispatch();
            deferral.Complete();
        }

        bool reportingException = false;
        async void app_UnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            if (!reportingException)
            {
                if (e.Handled)
                {
                    tracker.SendException(e.Message, false);
                }
                else
                {
                    try
                    {
                        reportingException = true;
                        e.Handled = true;
                        tracker.SendException(e.Message, true);
                        await Dispatch();
                        // rethrow the exception now that we're done logging it. wrap in another exception in order to prevent stack trace from getting reset.
                        throw new Exception("Tracked exception rethrown", e.Exception);
                    }
                    finally
                    {
                        // we have to do some trickery in order to make sure the flag is reset only after the new exception has passed all the way through the UE pipeline. Otherwise we would have an infinite loop.
                        var noawait = Window.Current.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, async () =>
                        {
                            await Task.Yield();
                            reportingException = false;
                        });
                    }
                }
            }
        }

    }
}
