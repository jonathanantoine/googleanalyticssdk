using GoogleAnalytics.Core;
using System;
using System.IO.IsolatedStorage;
using System.Windows;

namespace GoogleAnalytics
{
    public sealed class PlatformInfoProvider : IPlatformInfoProvider
    {
        const string Key_AnonymousClientId = "GoogleAnaltyics.AnonymousClientId";

#pragma warning disable 0067
        public event EventHandler ViewPortResolutionChanged;

        public event EventHandler ScreenResolutionChanged;
#pragma warning restore 0067

        public string AnonymousClientId
        {
            get
            {
                var appSettings = IsolatedStorageSettings.ApplicationSettings;
                if (!appSettings.Contains(Key_AnonymousClientId))
                {
                    var result = Guid.NewGuid().ToString();
                    appSettings.Add(Key_AnonymousClientId, result);
                    appSettings.Save();
                    return result;
                }
                else
                {
                    return (string)appSettings[Key_AnonymousClientId];
                }
            }
        }

        public Dimensions ScreenResolution
        {
            get { return new Dimensions(480, 800); }
        }

        public Dimensions ViewPortResolution
        {
            get { return new Dimensions(480, 800); }
        }

        public string UserLanguage
        {
            get { return System.Globalization.CultureInfo.CurrentUICulture.Name; }
        }

        public int? ScreenColorDepthBits
        {
            get { return null; }
        }

        public string DocumentEncoding
        {
            get { return null; }
        }

        public void OnTracking()
        { }

        public string GetUserAgent()
        {
            //var userAgentMask = "Mozilla/[version] ([system and browser information]) [platform] ([platform details]) [extensions]";
            if (Environment.OSVersion.Version.Major == 7)
            {
                return string.Format("Mozilla/5.0 (compatible; MSIE 9.0; Windows Phone OS {0}; Trident/5.0; IEMobile/9.0; Touch; {1}; {2})", Environment.OSVersion.Version, DeviceManufacturer, DeviceType);
            }
            else
            {
                return string.Format("Mozilla/5.0 (compatible; MSIE 10.0; Windows Phone OS {0}; Trident/6.0; IEMobile/10.0; ARM; Touch; {1}; {2})", Environment.OSVersion.Version, DeviceManufacturer, DeviceType);
            }
        }

        static string DeviceManufacturer
        {
            get
            {
                return Microsoft.Phone.Info.DeviceStatus.DeviceManufacturer;
            }
        }

        static string DeviceType
        {
            get
            {
                return Microsoft.Phone.Info.DeviceStatus.DeviceName;
            }
        }
    }
}
