using System;
using System.Xml;
using System.Threading.Tasks;
#if NETFX_CORE
using Windows.Foundation;
using Windows.UI.Xaml;
#else
using System.Windows;
#endif

namespace GoogleAnalytics
{
    public sealed partial class EasyTracker
    {
        static EasyTracker current;
        static Tracker tracker;
        DateTime? suspended;

        public Uri ConfigPath { get; set; }
        public EasyTrackerConfig Config { get; set; }

        public static EasyTracker Current
        {
            get
            {
                if (current == null)
                {
                    current = new EasyTracker();
                }
                return current;
            }
        }

        public static Tracker GetTracker()
        {
            if (tracker == null)
            {
                Application ctx = null;
                try
                {
                    ctx = Application.Current;
                }
                catch { /* ignore, Win8 JS cannot get the Current Application. Therefore we will pass null instead as context */  }
                Current.SetContext(ctx);
            }
            return tracker;
        }

        private void InitTracker()
        {
            var analyticsEngine = AnalyticsEngine.Current;
            analyticsEngine.IsDebugEnabled = Config.Debug;
            GAServiceManager.Current.DispatchPeriod = Config.DispatchPeriod;
            tracker = analyticsEngine.GetTracker(Config.TrackingId);
            tracker.SetStartSession(Config.SessionTimeout.HasValue);
            tracker.IsUseSecure = Config.UseSecure;
            tracker.AppName = Config.AppName;
            tracker.AppVersion = Config.AppVersion;
            tracker.IsAnonymizeIpEnabled = Config.AnonymizeIp;
            tracker.SampleRate = Config.SampleFrequency;
        }

        private void InitConfig(XmlReader reader)
        {
            Config = EasyTrackerConfig.Load(reader);
            Config.Validate();
        }

#if WINRT
        public IAsyncAction Dispatch()
        {
            return GAServiceManager.Current.Dispatch();
        }
#elif NETFX_CORE
        public IAsyncAction Dispatch()
        {
            return GAServiceManager.Current.Dispatch().AsAsyncAction();
        }
#else
        public Task Dispatch()
        {
            return GAServiceManager.Current.Dispatch();
        }
#endif
    }
}
