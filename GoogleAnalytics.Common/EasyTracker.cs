using System;
using System.Xml;
#if NETFX_CORE
using Windows.Foundation;
using Windows.UI.Xaml;
#else
using System.Windows;
using System.Threading.Tasks;
#endif

namespace GoogleAnalytics
{
    public sealed partial class EasyTracker
    {
        static EasyTracker current;
        static Tracker tracker;

        EasyTrackerConfig config;
        DateTime? suspended;

        public Uri ConfigPath { get; set; }

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
                object ctx = null;
                try
                {
                    ctx = Application.Current;
                }
                catch { /* ignore, Win8 JS cannot get the Current Application. Therefore we will pass null instead as context */  }
                Current.SetContext(ctx);
            }
            return tracker;
        }

        private void InitTracker(object ctx)
        {
            var ga = GoogleAnalytics.GetInstance(ctx);
            ga.IsDebugEnabled = config.Debug;
            tracker = ga.GetTracker(config.TrackingId);
            tracker.AppName = config.AppName;
            tracker.AppVersion = config.AppVersion;
            tracker.IsAnonymizeIpEnabled = config.AnonymizeIp;
            tracker.SampleRate = config.SampleFrequency;
        }

        private void InitConfig(XmlReader reader)
        {
            config = EasyTrackerConfig.Load(reader);
            GAServiceManager.Current.DispatchPeriod = config.DispatchPeriod;
        }

#if NETFX_CORE
        public IAsyncAction Dispatch()
#else
        public Task Dispatch()
#endif
        {
            return GAServiceManager.Current.Dispatch();
        }
    }
}
