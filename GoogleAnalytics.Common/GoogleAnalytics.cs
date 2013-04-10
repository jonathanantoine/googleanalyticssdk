using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace GoogleAnalytics
{
    public sealed partial class GoogleAnalytics
    {
        static GoogleAnalytics()
        {
            Instances = new Dictionary<object, GoogleAnalytics>();
        }

        static internal Dictionary<object, GoogleAnalytics> Instances { get; private set; }

        public static GoogleAnalytics GetInstance(object ctx)
        {
            if (!Instances.ContainsKey(ctx))
            {
                var instance = new GoogleAnalytics(new PlatformInfoProvider());
                Instances.Add(ctx, instance);
                return instance;
            }
            else
            {
                return Instances[ctx];
            }
        }

        readonly PlatformInfoProvider platformTrackingInfo;
        internal Dictionary<string, Tracker> Trackers { get; private set; }

        private GoogleAnalytics(PlatformInfoProvider platformTrackingInfo)
        {
            Trackers = new Dictionary<string, Tracker>();
            this.platformTrackingInfo = platformTrackingInfo;
        }

        public Tracker DefaultTracker { get; set; }

        public bool AppOptOut { get; set; }

        public bool IsDebugEnabled { get; set; }

        public Tracker GetTracker(string propertyId)
        {
            if (!Trackers.ContainsKey(propertyId))
            {
                var result = new Tracker(propertyId, platformTrackingInfo);
                Trackers.Add(propertyId, result);
                if (DefaultTracker == null)
                {
                    DefaultTracker = result;
                }
                return result;
            }
            else
            {
                return Trackers[propertyId];
            }
        }

        public void CloseTracker(Tracker tracker)
        {
            Trackers.Remove(tracker.TrackingId);
        }
    }
}
