﻿using GoogleAnalytics.Core;
using System;
using System.IO.IsolatedStorage;
using System.Windows;

namespace GoogleAnalytics
{
    public sealed class PlatformInfoProvider : IPlatformInfoProvider
    {
        const string Key_AnonymousClientId = "GoogleAnaltyics.AnonymousClientId";
        string anonymousClientId;

        static PlatformInfoProvider()
        {
            OSVersionPrecision = 2;
        }

#pragma warning disable 0067
        public event EventHandler ViewPortResolutionChanged;

        public event EventHandler ScreenResolutionChanged;
#pragma warning restore 0067

        public string AnonymousClientId
        {
            get
            {
                if (anonymousClientId == null)
                {
                    var appSettings = IsolatedStorageSettings.ApplicationSettings;
                    if (!appSettings.Contains(Key_AnonymousClientId))
                    {
                        anonymousClientId = Guid.NewGuid().ToString();
                        appSettings.Add(Key_AnonymousClientId, anonymousClientId);
                        appSettings.Save();
                    }
                    else
                    {
                        anonymousClientId = (string)appSettings[Key_AnonymousClientId];
                    }
                }
                return anonymousClientId;
            }
            set { anonymousClientId = value; }
        }

        public Dimensions ScreenResolution
        {
            get
            {
                double scale = (double)Application.Current.Host.Content.ScaleFactor / 100;
                int h = (int)Math.Ceiling(Application.Current.Host.Content.ActualHeight * scale);
                int w = (int)Math.Ceiling(Application.Current.Host.Content.ActualWidth * scale);
                return new Dimensions(h, w);
            }
        }

        public Dimensions ViewPortResolution
        {
            get { return ScreenResolution; }
        }

        public string UserLanguage
        {
            get { return System.Globalization.CultureInfo.CurrentUICulture.Name; }
        }

        public int? ScreenColorDepthBits
        {
            get { return null; }
        }

        public void OnTracking()
        { }

        public string GetUserAgent()
        {
            var sysInfo = PhoneNameResolver.Resolve(Microsoft.Phone.Info.DeviceStatus.DeviceManufacturer, Microsoft.Phone.Info.DeviceStatus.DeviceName);
            var version = Environment.OSVersion.Version.ToString();
            var versions = version.Split('.');
            if (OSVersionPrecision.HasValue)
            {
                version = string.Empty;
                for (int i = 0; i < OSVersionPrecision.Value; i++)
                {
                    version += versions[i] += ".";
                }
                version = version.TrimEnd('.');
            }

            //var userAgentMask = "Mozilla/[version] ([system and browser information]) [platform] ([platform details]) [extensions]";
            return string.Format("Mozilla/5.0 (compatible; MSIE 10.0; Windows Phone {0}; Trident/6.0; IEMobile/10.0; ARM; Touch; {1}; {2})", version, sysInfo.CanonicalManufacturer, sysInfo.CanonicalModel);
        }

        public static int? OSVersionPrecision { get; set; }
    }
}
