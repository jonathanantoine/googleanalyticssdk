using System;
using System.Collections.Generic;

namespace GoogleAnalytics
{
    internal sealed class Payload
    {
        public Payload(IEnumerable<KeyValuePair<string, string>> data)
        {
            Data = data;
            TimeStamp = DateTime.UtcNow;
        }

        public IEnumerable<KeyValuePair<string, string>> Data { get; private set; }
        public DateTime TimeStamp { get; private set; }
    }
}
