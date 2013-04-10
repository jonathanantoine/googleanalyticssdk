using System;
using System.IO.IsolatedStorage;
using System.Windows;

namespace GoogleAnalytics
{
    public sealed class PlatformInfoProvider
    {
        const string Key_AnonymousClientId = "GoogleAnaltyics.AnonymousClientId";

        public event EventHandler ViewPortResolutionChanged;

        public event EventHandler ScreenResolutionChanged;

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

        public Size? ScreenResolution
        {
            get { return new Size(480, 800); }
        }

        public Size? ViewPortResolution
        {
            get { return new Size(480, 800); }
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
    }
}
