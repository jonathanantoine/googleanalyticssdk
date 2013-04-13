using System;
using System.Collections.Generic;
#if NETFX_CORE
using Windows.Foundation;
#else
using System.Windows;
#endif

namespace GoogleAnalytics
{
    public sealed class Tracker
    {
        readonly PayloadFactory engine;
        readonly Queue<Payload> payloads;
        readonly PlatformInfoProvider platformInfoProvider;

        internal Tracker(string propertyId, PlatformInfoProvider platformInfoProvider)
        {
            this.platformInfoProvider = platformInfoProvider;
            payloads = new Queue<Payload>();
            engine = new PayloadFactory()
            {
                PropertyId = propertyId,
                AnonymousClientId = platformInfoProvider.AnonymousClientId,
                DocumentEncoding = platformInfoProvider.DocumentEncoding,
                ScreenColorDepthBits = platformInfoProvider.ScreenColorDepthBits,
                ScreenResolution = platformInfoProvider.ScreenResolution,
                UserLanguage = platformInfoProvider.UserLanguage,
                ViewportSize = platformInfoProvider.ViewPortResolution
            };
            platformInfoProvider.ViewPortResolutionChanged += platformTrackingInfo_ViewPortResolutionChanged;
            platformInfoProvider.ScreenResolutionChanged += platformTrackingInfo_ScreenResolutionChanged;
            SampleRate = 100.0F;
        }

        bool isEnabled = true;
        internal bool IsEnabled
        {
            get { return isEnabled; }
            set
            {
                isEnabled = value;
                if (!isEnabled)
                {
                    lock (payloads)
                    {
                        payloads.Clear();
                    }
                }
            }
        }

        public void SetCustomDimension(int index, string value)
        {
            engine.CustomDimensions[index] = value;
        }

        public void SetCustomMetric(int index, int value)
        {
            engine.CustomMetrics[index] = value;
        }

#if NETFX_CORE
        private void platformTrackingInfo_ViewPortResolutionChanged(object sender, object args)
#else
        private void platformTrackingInfo_ViewPortResolutionChanged(object sender, EventArgs args)
#endif
        {
            engine.ViewportSize = platformInfoProvider.ViewPortResolution;
        }

#if NETFX_CORE
        private void platformTrackingInfo_ScreenResolutionChanged(object sender, object args)
#else
        private void platformTrackingInfo_ScreenResolutionChanged(object sender, EventArgs args)
#endif
        {
            engine.ScreenResolution = platformInfoProvider.ScreenResolution;
        }

        public string TrackingId
        {
            get { return engine.PropertyId; }
        }

        public bool IsAnonymizeIpEnabled
        {
            get { return engine.AnonymizeIP; }
            set { engine.AnonymizeIP = value; }
        }

        public string AppName
        {
            get { return engine.AppName; }
            set { engine.AppName = value; }
        }

        public string AppVersion
        {
            get { return engine.AppVersion; }
            set { engine.AppVersion = value; }
        }

        public Size? AppScreen
        {
            get { return engine.ViewportSize; }
            set { engine.ViewportSize = value; }
        }

        public string Referrer
        {
            get { return engine.Referrer; }
            set { engine.Referrer = value; }
        }

        public string Campaign
        {
            get { return engine.Campaign; }
            set { engine.Campaign = value; }
        }

        public bool BustCache { get; set; }
        public float SampleRate { get; set; }
        public bool IsUseSecure { get; set; }
        public bool ThrottlingEnabled { get; set; }

        public void SendView(string screenName)
        {
            platformInfoProvider.OnTracking(); // give platform info provider a chance to refresh.
            var payload = engine.TrackView(screenName, SessionControl);
            AddPayload(payload);
        }

        public void SendException(string description, bool isFatal)
        {
            platformInfoProvider.OnTracking(); // give platform info provider a chance to refresh.
            var payload = engine.TrackException(description, isFatal, SessionControl);
            AddPayload(payload);
        }

        public void SendSocial(string network, string action, string target)
        {
            platformInfoProvider.OnTracking(); // give platform info provider a chance to refresh.
            var payload = engine.TrackSocialInteraction(network, action, target, SessionControl);
            AddPayload(payload);
        }

        public void SendTiming(TimeSpan time, string category, string variable, string label)
        {
            platformInfoProvider.OnTracking(); // give platform info provider a chance to refresh.
            var payload = engine.TrackUserTiming(category, variable, null, label, time, null, null, null, null, null, SessionControl);
            AddPayload(payload);
        }

        public void SendEvent(string category, string action, string label, int value)
        {
            platformInfoProvider.OnTracking(); // give platform info provider a chance to refresh.
            var payload = engine.TrackEvent(category, action, label, value, SessionControl);
            AddPayload(payload);
        }

        public void SendTransaction(Transaction transaction)
        {
            platformInfoProvider.OnTracking(); // give platform info provider a chance to refresh.
            foreach (var payload in TrackTransaction(transaction, SessionControl))
            {
                AddPayload(payload);
            }
        }

        IEnumerable<Payload> TrackTransaction(Transaction transaction, SessionControl sessionControl = SessionControl.None, bool isNonInteractive = false)
        {
            yield return engine.TrackTransaction(transaction.TransactionId, transaction.Affiliation, (double)transaction.TotalCostInMicros / 1000000, (double)transaction.ShippingCostInMicros / 1000000, (double)transaction.TotalTaxInMicros / 1000000, transaction.CurrencyCode, sessionControl, isNonInteractive);

            foreach (var item in transaction.Items)
            {
                yield return engine.TrackTransactionItem(transaction.TransactionId, item.Name, (double)item.PriceInMicros / 1000000, item.Quantity, item.SKU, item.Category, transaction.CurrencyCode, sessionControl, isNonInteractive);
            }
        }

        SessionControl SessionControl
        {
            get
            {
                if (EndSession)
                {
                    EndSession = false;
                    return SessionControl.End;
                }
                else if (StartSession)
                {
                    StartSession = false;
                    return SessionControl.Start;
                }
                else
                {
                    return SessionControl.None;
                }
            }
        }

        public bool StartSession { get; set; }

        public bool EndSession { get; set; }

        void AddPayload(Payload payload)
        {
            if (IsEnabled)
            {
                var serviceManager = GAServiceManager.Current;
                if (serviceManager.DispatchPeriod == TimeSpan.Zero && serviceManager.IsConnected)
                {
                    var nowait = GAServiceManager.Current.DispatchImmediatePayload(this, payload);
                }
                else
                {
                    lock (payloads)
                    {
                        payloads.Enqueue(payload);
                    }
                }
            }
        }

        internal void RecyclePayload(Payload payload)
        {
            lock (payloads)
            {
                payloads.Enqueue(payload);
            }
        }

        internal IEnumerable<Payload> GetPayloads()
        {
            lock (payloads)
            {
                var total = payloads.Count;
                var payloadsToSample = ThrottlingEnabled ? SampleRate / 100.0F * total : total;
                for (int i = 0; i < payloadsToSample; i++)
                {
                    yield return payloads.Dequeue();
                }
            }
        }
    }
}
