﻿using GoogleAnalytics.Core;
using System;
using Windows.Foundation;
using Windows.Graphics.Display;
using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;

namespace GoogleAnalytics
{
    public sealed class PlatformInfoProvider : IPlatformInfoProvider
    {
        const string Key_AnonymousClientId = "GoogleAnaltyics.AnonymousClientId";
        bool windowInitialized = false;

#if NETFX_CORE
        public event EventHandler<object> ViewPortResolutionChanged;
        public event EventHandler<object> ScreenResolutionChanged;
#else
        public event EventHandler ViewPortResolutionChanged;
        public event EventHandler ScreenResolutionChanged;
#endif

        public PlatformInfoProvider()
        {
            InitializeWindow();
        }

        private void InitializeWindow()
        {
            try
            {
                if (Window.Current != null && Window.Current.Content != null)
                {
                    var bounds = Window.Current.Bounds;
                    double w = bounds.Width;
                    double h = bounds.Height;
                    var displayInfo = DisplayInformation.GetForCurrentView();
                    switch (displayInfo.ResolutionScale)
                    {
                        case ResolutionScale.Scale120Percent:
                            w = Math.Round(w * 1.2);
                            h = Math.Round(h * 1.2);
                            break;
                        case ResolutionScale.Scale140Percent:
                            w = Math.Round(w * 1.4);
                            h = Math.Round(h * 1.4);
                            break;
                        case ResolutionScale.Scale150Percent:
                            w = Math.Round(w * 1.5);
                            h = Math.Round(h * 1.5);
                            break;
                        case ResolutionScale.Scale160Percent:
                            w = Math.Round(w * 1.6);
                            h = Math.Round(h * 1.6);
                            break;
                        case ResolutionScale.Scale180Percent:
                            w = Math.Round(w * 1.8);
                            h = Math.Round(h * 1.8);
                            break;
                        case ResolutionScale.Scale225Percent:
                            w = Math.Round(w * 2.25);
                            h = Math.Round(h * 2.25);
                            break;
                    }

                    if ((displayInfo.CurrentOrientation & DisplayOrientations.Landscape) == DisplayOrientations.Landscape)
                    {
                        ScreenResolution = new Dimensions((int)w, (int)h);
                    }
                    else // portrait
                    {
                        ScreenResolution = new Dimensions((int)h, (int)w);
                    }
                    ViewPortResolution = new Dimensions((int)bounds.Width, (int)bounds.Height); // leave viewport at the scale unadjusted size
                    Window.Current.SizeChanged += Current_SizeChanged;
                    windowInitialized = true;
                }
            }
            catch { /* ignore, Bounds may not be ready yet */ }
        }

        public void OnTracking()
        {
            if (!windowInitialized)
            {
                InitializeWindow();
            }
        }

        void Current_SizeChanged(object sender, WindowSizeChangedEventArgs e)
        {
            ViewPortResolution = new Dimensions((int)e.Size.Width, (int)e.Size.Height);
        }

        public string AnonymousClientId
        {
            get
            {
                var appSettings = ApplicationData.Current.LocalSettings;
                if (!appSettings.Values.ContainsKey(Key_AnonymousClientId))
                {
                    var result = Guid.NewGuid().ToString();
                    appSettings.Values[Key_AnonymousClientId] = result;
                    return result;
                }
                else
                {
                    return (string)appSettings.Values[Key_AnonymousClientId];
                }
            }
        }

        Dimensions viewPortResolution;
        public Dimensions ViewPortResolution
        {
            get { return viewPortResolution; }
            private set
            {
                if (viewPortResolution != value)
                {
                    viewPortResolution = value;
                    if (ViewPortResolutionChanged != null) ViewPortResolutionChanged(this, EventArgs.Empty);
                }
            }
        }

        Dimensions screenResolution;
        // Another possible solution: http://blogs.microsoft.co.il/blogs/tomershamam/archive/2012/07/24/get-screen-resolution-in-windows-8-metro-style-application.aspx
        public Dimensions ScreenResolution
        {
            get { return screenResolution; }
            private set
            {
                if (screenResolution != value)
                {
                    screenResolution = value;
                    if (ScreenResolutionChanged != null) ScreenResolutionChanged(this, EventArgs.Empty);
                }
            }
        }

        public string UserLanguage
        {
            get { return System.Globalization.CultureInfo.CurrentUICulture.Name; }
        }

        // Not sure how to get this information in Windows 8
        public int? ScreenColorDepthBits
        {
            get { return null; }
        }

        public string DocumentEncoding
        {
            get { return null; }
        }


        public string GetUserAgent()
        {
            // unfortunately, there isn't much info we can get from Windows 8 Store apps
            Windows.Devices.Input.TouchCapabilities tc = new Windows.Devices.Input.TouchCapabilities();
            var hasTouch = tc.TouchPresent > 0;
            return string.Format("Mozilla/5.0 (compatible; MSIE 11.0; Windows NT 6.3; Trident/7.0{0})", hasTouch ? "; Touch" : "");
        }
    }
}
