using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
#if NETFX_CORE
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.System.Threading;
#endif

namespace GoogleAnalytics.Core
{
    public sealed class GAServiceManager : IServiceManager
    {
        static Random random;
        static GAServiceManager current;
        static readonly Uri endPointUnsecure = new Uri("http://www.google-analytics.com/collect");
        static readonly Uri endPointSecure = new Uri("https://ssl.google-analytics.com/collect");
        readonly Queue<Payload> payloads;
        readonly IList<Task> dispatchingTasks;
#if NETFX_CORE
        ThreadPoolTimer timer;
#else
        Timer timer;
#endif

        private GAServiceManager()
        {
            dispatchingTasks = new List<Task>();
            payloads = new Queue<Payload>();
            DispatchPeriod = TimeSpan.Zero;
        }

        public bool BustCache { get; set; }

        public void Clear()
        {
            lock (payloads)
            {
                payloads.Clear();
            }
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
#else
                        timer.Dispose();
#endif
                        timer = null;
                    }
                    if (dispatchPeriod > TimeSpan.Zero)
                    {
#if NETFX_CORE
                        timer = ThreadPoolTimer.CreatePeriodicTimer(timer_Tick, dispatchPeriod);
#else
                        timer = new Timer(timer_Tick, null, DispatchPeriod, DispatchPeriod);
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
                        if (DispatchPeriod >= TimeSpan.Zero)
                        {
                            var nowait = Dispatch();
                        }
                    }
                }
            }
        }

        async void IServiceManager.SendPayload(Payload payload)
        {
            if (DispatchPeriod == TimeSpan.Zero && IsConnected)
            {
                await RunDispatchingTask(DispatchImmediatePayload(payload));
            }
            else
            {
                lock (payloads)
                {
                    payloads.Enqueue(payload);
                }
            }
        }
        
#if NETFX_CORE
        public IAsyncAction Dispatch()
        {
            return _Dispatch().AsAsyncAction();
        }

        internal async Task _Dispatch()
#else
        public async Task Dispatch()
#endif
        {
            if (!isConnected) return;

            Task allDispatchingTasks = null;
            lock (dispatchingTasks)
            {
                if (dispatchingTasks.Any())
                {
#if ASYNCEXTENSIONS
                    allDispatchingTasks = TaskEx.WhenAll(dispatchingTasks);
#else
                    allDispatchingTasks = Task.WhenAll(dispatchingTasks);
#endif
                }
            }
            if (allDispatchingTasks != null)
            {
                await allDispatchingTasks;
            }

            if (!isConnected) return;

            IList<Payload> payloadsToSend = new List<Payload>();
            lock (payloads)
            {
                while (payloads.Count > 0)
                {
                    payloadsToSend.Add(payloads.Dequeue());
                }
            }
            if (payloadsToSend.Any())
            {
                await RunDispatchingTask(DispatchQueuedPayloads(payloadsToSend));
            }
        }

        async Task RunDispatchingTask(Task newDispatchingTask)
        {
            lock (dispatchingTasks)
            {
                dispatchingTasks.Add(newDispatchingTask);
            }
            try
            {
                await newDispatchingTask;
            }
            finally
            {
                lock (dispatchingTasks)
                {
                    dispatchingTasks.Remove(newDispatchingTask);
                }
            }
        }

        private async Task DispatchQueuedPayloads(IEnumerable<Payload> payloads)
        {
            using (var httpClient = GetHttpClient())
            {
                var now = DateTimeOffset.UtcNow;
                foreach (var payload in payloads)
                {
                    if (isConnected)
                    {
                        // clone the data
                        var payloadData = payload.Data.ToDictionary(kvp => kvp.Key, kvp => kvp.Value);
                        payloadData.Add("qt", ((long)now.Subtract(payload.TimeStamp).TotalMilliseconds).ToString());
                        await DispatchPayloadData(payload, httpClient, payloadData);
                    }
                    else
                    {
                        lock (payloads) // add back to queue
                        {
                            this.payloads.Enqueue(payload);
                        }
                    }
                }
            }
        }

        async Task DispatchImmediatePayload(Payload payload)
        {
            using (var httpClient = GetHttpClient())
            {
                // clone the data
                var payloadData = payload.Data.ToDictionary(kvp => kvp.Key, kvp => kvp.Value);
                await DispatchPayloadData(payload, httpClient, payloadData);
            }
        }

        async Task DispatchPayloadData(Payload payload, HttpClient httpClient, Dictionary<string, string> payloadData)
        {
            if (BustCache) payloadData.Add("z", GetCacheBuster());
            var endPoint = payload.IsUseSecure ? endPointSecure : endPointUnsecure;
            using (var content = GetEncodedContent(payloadData))
            {
                try
                {
                    await httpClient.PostAsync(endPoint, content);
                }
                catch
                {
                    OnPayloadFailed(payload);
                }
            }
        }

        void OnPayloadFailed(Payload payload)
        {
            // TODO: store in isolated storage and retry next session
        }

        HttpClient GetHttpClient()
        {
            var result = new HttpClient();
            if (!string.IsNullOrEmpty(UserAgent))
            {
                result.DefaultRequestHeaders.UserAgent.ParseAdd(UserAgent);
            }
            return result;
        }

        public string UserAgent { get; set; }

        static string GetCacheBuster()
        {
            if (random == null)
            {
                random = new Random();
            }
            return random.Next().ToString();
        }

        static ByteArrayContent GetEncodedContent(IEnumerable<KeyValuePair<string, string>> nameValueCollection)
        {
            return new StringContent(GetUrlEncodedString(nameValueCollection));
        }

        static string GetUrlEncodedString(IEnumerable<KeyValuePair<string, string>> nameValueCollection)
        {
            var result = new StringBuilder();
            bool isFirst = true;
            foreach (var item in nameValueCollection)
            {
                var value = item.Value;
                if (value != null)
                {
                    if (isFirst)
                    {
                        isFirst = false;
                    }
                    else
                    {
                        result.Append("&");
                    }
                    result.Append(item.Key);
                    result.Append("=");
                    result.Append(Uri.EscapeDataString(value));
                }
            }
            return result.ToString();
        }
    }
}
