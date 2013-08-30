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
            get
            {
                double scale = (double)Application.Current.Host.Content.ScaleFactor / 100;
                int h = (int)Math.Ceiling(Application.Current.Host.Content.ActualHeight * scale);
                int w = (int)Math.Ceiling(Application.Current.Host.Content.ActualWidth * scale);
                return new Size(h, w);
            }
        }

        public Size? ViewPortResolution
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

        public string DocumentEncoding
        {
            get { return null; }
        }

        public void OnTracking()
        { }
    }
}
