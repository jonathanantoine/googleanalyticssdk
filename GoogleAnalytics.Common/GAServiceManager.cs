using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Threading.Tasks;
using System.Threading;
#if NETFX_CORE
using Windows.Foundation;
using Windows.System.Threading;
#else
#endif
#if WINDOWS_PHONE
using Microsoft.Phone.Info;
#endif

namespace GoogleAnalytics
{
    public sealed class GAServiceManager
    {
        static GAServiceManager current;
        static readonly Random random = new Random();
        static readonly Uri endPointUnsecure = new Uri("http://www.google-analytics.com/collect");
        static readonly Uri endPointSecure = new Uri("https://ssl.google-analytics.com/collect");

#if NETFX_CORE
        ThreadPoolTimer timer;
#else
        Timer timer;
#endif
        bool isDispatching;

        private GAServiceManager()
        {
            DispatchPeriod = TimeSpan.FromSeconds(30);
#if NETFX_CORE
            timer = ThreadPoolTimer.CreatePeriodicTimer(timer_Tick, DispatchPeriod);
#else
            timer = new Timer(timer_Tick, null, DispatchPeriod, DispatchPeriod);
#endif
        }

        async void timer_Tick(object sender)
        {
            await Dispatch();
        }

        public static GAServiceManager Current
        {
            get
            {
                if (current == null)
                {
                    current = new GAServiceManager();
                }
                return current;
            }
        }

        TimeSpan dispatchPeriod;
        public TimeSpan DispatchPeriod
        {
            get { return dispatchPeriod; }
            set
            {
                if (dispatchPeriod != value)
                {
                    dispatchPeriod = value;
                    if (timer != null)
                    {
#if NETFX_CORE
                        timer.Cancel();
                        if (dispatchPeriod > TimeSpan.Zero)
                        {
                            timer = ThreadPoolTimer.CreatePeriodicTimer(timer_Tick, dispatchPeriod);
                        }
#else
                        timer.Dispose();
                        if (dispatchPeriod > TimeSpan.Zero)
                        {
                            timer = new Timer(timer_Tick, null, DispatchPeriod, DispatchPeriod);
                        }
#endif
                    }
                }
            }
        }

        bool isConnected = true; // assume true. The app can tell us differently
        public bool IsConnected
        {
            get { return isConnected; }
            set
            {
                if (isConnected != value)
                {
                    isConnected = value;
                    if (isConnected)
                    {
                        var nowait = Dispatch();
                    }
                }
            }
        }

#if NETFX_CORE
        public IAsyncAction Dispatch()
        {
            return _Dispatch().AsAsyncAction();
        }

        async Task _Dispatch()
#else
        public async Task Dispatch()
#endif
        {
            if (isDispatching) return;
            if (!isConnected) return;
            isDispatching = true;
            try
            {
                foreach (var tracker in GoogleAnalytics.Current.Trackers.Values)
                {
                    await DispatchQueuedPayloads(tracker, tracker.GetPayloads().ToArray());
                }
            }
            finally
            {
                isDispatching = false;
            }
        }

        private async Task DispatchQueuedPayloads(Tracker tracker, params Payload[] payloads)
        {
            using (var httpClient = GetHttpClient())
            {
                var now = DateTime.UtcNow;
                foreach (var payload in payloads)
                {
                    var payloadData = payload.Data.ToDictionary(kvp => kvp.Key, kvp => kvp.Value);
                    payloadData.Add("qt", ((long)now.Subtract(payload.TimeStamp).TotalMilliseconds).ToString());
                    await DispatchPayloadData(tracker, payload, httpClient, payloadData);
                }
            }
        }

        internal async Task DispatchImmediatePayload(Tracker tracker, Payload payload)
        {
            using (var httpClient = GetHttpClient())
            {
                var payloadData = payload.Data.ToDictionary(kvp => kvp.Key, kvp => kvp.Value);
                await DispatchPayloadData(tracker, payload, httpClient, payloadData);
            }
        }

        static async Task DispatchPayloadData(Tracker tracker, Payload payload, HttpClient httpClient, Dictionary<string, string> payloadData)
        {
            if (tracker.BustCache) payloadData.Add("z", GetCacheBuster());
            var endPoint = tracker.IsUseSecure ? endPointSecure : endPointUnsecure;
            using (var content = new FormUrlEncodedContent(payloadData))
            {
                try
                {
                    var response = await httpClient.PostAsync(endPoint, content);
                    if (!response.IsSuccessStatusCode)
                    {
                        tracker.PayloadFailed(payload);
                    }
                }
                catch { /* ignore */ }
            }
        }

        static HttpClient GetHttpClient()
        {
            var result = new HttpClient();
            result.DefaultRequestHeaders.UserAgent.ParseAdd(GetUserAgent());
            return result;
        }

        static string GetCacheBuster()
        {
            return random.Next().ToString();
        }

#if NETFX_CORE
        static string GetUserAgent()
        {
            // unfortunately, there isn't much info we can get from Windows 8 Store apps
            return "Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.2; Trident/6.0)";
        }

#elif WINDOWS_PHONE
        static string GetUserAgent()
        {
            //var userAgentMask = "Mozilla/[version] ([system and browser information]) [platform] ([platform details]) [extensions]";
#if WINDOWS_PHONE7
            return string.Format("Mozilla/5.0 (compatible; MSIE 9.0; Windows Phone OS {0}; Trident/5.0; IEMobile/9.0; {1}; {2})", Environment.OSVersion.Version, DeviceManufacturer, DeviceType);
#else
            return string.Format("Mozilla/5.0 (compatible; MSIE 10.0; Windows Phone {0}; Trident/6.0; IEMobile/10.0; ARM; Touch; {1}; {2})", Environment.OSVersion.Version, DeviceManufacturer, DeviceType);
#endif
        }

        public static string DeviceManufacturer
        {
            get
            {
                return DeviceExtendedProperties.GetValue("DeviceManufacturer").ToString();
            }
        }

        public static string DeviceType
        {
            get
            {
                return DeviceExtendedProperties.GetValue("DeviceName").ToString();
            }
        }
#endif
    }
}
