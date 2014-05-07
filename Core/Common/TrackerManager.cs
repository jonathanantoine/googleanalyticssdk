using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace GoogleAnalytics.Core
{
    public sealed class TrackerManager : IServiceManager
    {
        readonly IPlatformInfoProvider platformTrackingInfo;
        readonly Dictionary<string, Tracker> trackers;

        public TrackerManager(IPlatformInfoProvider platformTrackingInfo)
        {
            trackers = new Dictionary<string, Tracker>();
            this.platformTrackingInfo = platformTrackingInfo;
        }

        public Tracker DefaultTracker { get; set; }

        public bool IsDebugEnabled { get; set; }

        public bool AppOptOut { get; set; }

        public Tracker GetTracker(string propertyId)
        {
            propertyId = propertyId ?? string.Empty;
            if (!trackers.ContainsKey(propertyId))
            {
                var tracker = new Tracker(propertyId, platformTrackingInfo, this);
                trackers.Add(propertyId, tracker);
                if (DefaultTracker == null)
                {
                    DefaultTracker = tracker;
                }
                return tracker;
            }
            else
            {
                return trackers[propertyId];
            }
        }

        public void CloseTracker(Tracker tracker)
        {
            trackers.Remove(tracker.TrackingId);
            if (DefaultTracker == tracker)
            {
                DefaultTracker = null;
            }
        }

        void IServiceManager.SendPayload(Payload payload)
        {
            if (!AppOptOut)
            {
                ((IServiceManager)GAServiceManager.Current).SendPayload(payload);
            }
        }

        string IServiceManager.UserAgent
        {
            get
            {
                return GAServiceManager.Current.UserAgent;
            }
            set
            {
                GAServiceManager.Current.UserAgent = value;
            }
        }

        public IPlatformInfoProvider PlatformTrackingInfo
        {
            get { return platformTrackingInfo; }
        }
    }
}
