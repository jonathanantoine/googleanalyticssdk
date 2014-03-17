using System;
using System.Threading;
using System.Threading.Tasks;

// Workaround for missing Timer class in PCL profile. 
// Code starting point from: http://stackoverflow.com/questions/12555049/timer-in-portable-library

namespace GoogleAnalytics
{
    internal delegate void TimerCallback(object state);

    internal sealed class Timer : CancellationTokenSource, IDisposable
    {
        internal Timer(TimerCallback callback, object state, TimeSpan dueTime, TimeSpan period)
            : this(callback, state, (int)dueTime.TotalMilliseconds, (int)period.TotalMilliseconds)
        {
        }

        internal Timer(TimerCallback callback, object state, int dueTime, int period)
        {
            Task.Run(() => WaitTimer(callback, state, period, Token));
        }

        static async Task WaitTimer(TimerCallback callback, object state, int period, CancellationToken token)
        {
            await WaitPeriod(callback, state, period, token);
            if (period > 0)
            {
                while (!token.IsCancellationRequested)
                {
                    await WaitPeriod(callback, state, period, token);
                }
            }
        }

        static async Task WaitPeriod(TimerCallback callback, object state, int period, CancellationToken token)
        {
            try
            {
                await Task.Delay(period, token);
                callback(state);
            }
            catch (TaskCanceledException) { /* ignore */ }
        }

        public new void Dispose() { base.Cancel(); }
    }
}
