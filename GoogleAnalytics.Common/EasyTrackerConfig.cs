using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace GoogleAnalytics
{

    internal sealed class EasyTrackerConfig
    {
        private EasyTrackerConfig()
        {
            SessionTimeout = TimeSpan.FromSeconds(30);
            DispatchPeriod = TimeSpan.FromSeconds(30);
            SampleFrequency = 100.0F;
        }

        internal static EasyTrackerConfig Load(XmlReader reader)
        {
            var result = new EasyTrackerConfig();
            if (!reader.ReadToFollowing("resources")) return result;
            reader.ReadStartElement("resources");
            do
            {
                if (reader.IsStartElement())
                {
                    var key = (string)reader.GetAttribute("name");
                    switch (reader.Name)
                    {
                        case "string":
                            {
                                var value = reader.ReadElementContentAsString();
                                switch (key)
                                {
                                    case "ga_trackingId":
                                        result.TrackingId = value;
                                        break;
                                    case "ga_appName":
                                        result.AppName = value;
                                        break;
                                    case "ga_appVersion":
                                        result.AppVersion = value;
                                        break;
                                    case "ga_sampleFrequency":
                                        result.SampleFrequency = float.Parse(value);
                                        break;
                                }
                            }
                            break;
                        case "integer":
                            {
                                var value = reader.ReadElementContentAsInt();
                                switch (key)
                                {
                                    case "ga_dispatchPeriod":
                                        result.DispatchPeriod = TimeSpan.FromSeconds(value);
                                        break;
                                    case "ga_sessionTimeout":
                                        result.SessionTimeout = (value >= 0) ? TimeSpan.FromSeconds(value) : (TimeSpan?)null;
                                        break;
                                }
                            }
                            break;
                        case "bool":
                            {
                                var value = reader.ReadElementContentAsBoolean();
                                switch (key)
                                {
                                    case "ga_debug":
                                        result.Debug = value;
                                        break;
                                    case "ga_autoActivityTracking":
                                        result.AutoActivityTracking = value;
                                        break;
                                    case "ga_autoAppLifetimeTracking":
                                        result.AutoAppLifetimeTracking = value;
                                        break;
                                    case "ga_anonymizeIp":
                                        result.AnonymizeIp = value;
                                        break;
                                    case "ga_reportUncaughtExceptions":
                                        result.ReportUncaughtExceptions = value;
                                        break;
                                }
                            }
                            break;
                        default:
                            reader.Skip();
                            break;
                    }
                }
                else
                {
                    reader.ReadEndElement();
                    break;
                }
            }
            while (true);
            return result;
        }

        /// <summary>
        /// The Google Analytics tracking ID to which to send your data. Dashes in the ID must be unencoded. You can disable your tracking by not providing this value. 
        /// </summary>
        public string TrackingId { get; set; }
        /// <summary>
        /// The name of your app, used in the app name dimension in your reports. Defaults to the value found in the package. 
        /// </summary>
        public string AppName { get; set; }
        /// <summary>
        /// The version of your application, used in the app version dimension within your reports. Defaults to the version found in the package. 
        /// </summary>
        public string AppVersion { get; set; }
        /// <summary>
        /// Flag to enable or writing of debug information to the log, useful for troubleshooting your implementation. false by default. 
        /// </summary>
        public bool Debug { get; set; }
        /// <summary>
        /// The dispatch period in seconds. Defaults to 30 seconds. 
        /// </summary>
        public TimeSpan DispatchPeriod { get; set; }
        /// <summary>
        /// The sample rate to use. Default is 100.0. It can be any value between 0.0 and 100.0 
        /// </summary>
        public float SampleFrequency { get; set; }
        /// <summary>
        /// Automatically track a screen view each time a user starts an Activity. false by default. 
        /// </summary>
        public bool AutoActivityTracking { get; set; }
        /// <summary>
        /// Tells Google Analytics to anonymize the information sent by the tracker objects by removing the last octet of the IP address prior to its storage. Note that this will slightly reduce the accuracy of geographic reporting. false by default.
        /// </summary>
        public bool AnonymizeIp { get; set; }
        /// <summary>
        /// Automatically track an Exception each time an uncaught exception is thrown in your application. false by default. 
        /// </summary>
        public bool ReportUncaughtExceptions { get; set; }
        /// <summary>
        /// The amount of time your application can stay in the background before the session is ended. Default is 30 seconds. Null value disables EasyTracker session management.
        /// </summary>
        public TimeSpan? SessionTimeout { get; set; }
        /// <summary>
        /// Automatically track application lifetime events (e.g. suspend and resume). true by default.
        /// </summary>
        public bool AutoAppLifetimeTracking { get; set; }
    }
}
