using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace GoogleAnalytics
{
    public sealed partial class GoogleAnalytics
    {
        static GoogleAnalytics current;

        public static GoogleAnalytics Current
        {
            get
            {
                if (current == null)
                {
                    current = new GoogleAnalytics(new PlatformInfoProvider());
                }
                return current;
            }
        }

        readonly PlatformInfoProvider platformTrackingInfo;
        readonly Dictionary<string, Tracker> Trackers;

        private GoogleAnalytics(PlatformInfoProvider platformTrackingInfo)
        {
            Trackers = new Dictionary<string, Tracker>();
            this.platformTrackingInfo = platformTrackingInfo;
        }

        public Tracker DefaultTracker { get; set; }

        bool appOptOut;
        public bool AppOptOut
        {
            get { return appOptOut; }
            set
            {
                appOptOut = value;
                if (appOptOut)
                {
                    GAServiceManager.Current.Clear();
                }
            }
        }

        public bool IsDebugEnabled { get; set; }

        public Tracker GetTracker(string propertyId)
        {
            propertyId = propertyId ?? string.Empty;
            if (!Trackers.ContainsKey(propertyId))
            {
                var tracker = new Tracker(propertyId, platformTrackingInfo, this);
                Trackers.Add(propertyId, tracker);
                if (DefaultTracker == null)
                {
                    DefaultTracker = tracker;
                }
                return tracker;
            }
            else
            {
                return Trackers[propertyId];
            }
        }

        public void CloseTracker(Tracker tracker)
        {
            Trackers.Remove(tracker.TrackingId);
            if (DefaultTracker == tracker)
            {
                DefaultTracker = null;
            }
        }

        internal void SendPayload(Payload payload)
        {
            if (!AppOptOut)
            {
                GAServiceManager.Current.SendPayload(payload);
            }
        }
    }
}
