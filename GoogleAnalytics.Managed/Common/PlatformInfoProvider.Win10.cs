using GoogleAnalytics.Core;
using System;
using System.Linq;
using Windows.ApplicationModel;
using Windows.Devices.Input;
using Windows.Foundation;
using Windows.Graphics.Display;
using Windows.Storage;
using Windows.System.Profile;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;

namespace GoogleAnalytics
{
    public sealed class PlatformInfoProvider : IPlatformInfoProvider
    {
        const string Key_AnonymousClientId = "GoogleAnaltyics.AnonymousClientId";
        bool windowInitialized = false;
        string anonymousClientId;

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
                    double w, h;
                    var pointerDevice = PointerDevice.GetPointerDevices().FirstOrDefault();
                    if (pointerDevice != null)
                    {
                        w = pointerDevice.ScreenRect.Width;
                        h = pointerDevice.ScreenRect.Height;
                    }
                    else
                    {
                        w = bounds.Width;
                        h = bounds.Height;
                    }

                    var displayInfo = DisplayInformation.GetForCurrentView();
                    var scale = (double)(int)displayInfo.ResolutionScale / 100d;
                    w = Math.Round(w * scale);
                    h = Math.Round(h * scale);

                    if ((displayInfo.NativeOrientation & DisplayOrientations.Landscape) == DisplayOrientations.Landscape)
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
                if (anonymousClientId == null)
                {
                    var appSettings = ApplicationData.Current.LocalSettings;
                    if (!appSettings.Values.ContainsKey(Key_AnonymousClientId))
                    {
                        anonymousClientId = Guid.NewGuid().ToString();
                        appSettings.Values[Key_AnonymousClientId] = anonymousClientId;
                    }
                    else
                    {
                        anonymousClientId = (string)appSettings.Values[Key_AnonymousClientId];
                    }
                }
                return anonymousClientId;
            }
            set { anonymousClientId = value; }
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

        public string GetUserAgent()
        {
            var sysInfo = new Windows.Security.ExchangeActiveSyncProvisioning.EasClientDeviceInformation();
            Windows.Devices.Input.TouchCapabilities tc = new Windows.Devices.Input.TouchCapabilities();
            var hasTouch = tc.TouchPresent > 0;
            var ai = AnalyticsInfo.VersionInfo;
            string sv = ai.DeviceFamilyVersion;
            ulong v = ulong.Parse(sv);
            ulong v1 = (v & 0xFFFF000000000000L) >> 48;
            ulong v2 = (v & 0x0000FFFF00000000L) >> 32;
            var systemVersion = $"{v1}.{v2}";
            string os;
            switch (ai.DeviceFamily)
            {
                case "Windows.Desktop":
                    os = "Windows NT";
                    break;
                case "Windows.Mobile":
                    os = "Windows Phone";
                    break;
                case "Windows.Xbox":
                    os = "Windows NT";
                    break;
                default:
                    os = "Windows";
                    break;
            }

            return string.Format("Mozilla/5.0 ({0} {1}{2}; {3}; {4}; {5})", os, systemVersion, hasTouch ? "; Touch" : "", Package.Current.Id.Architecture.ToString(), sysInfo.SystemManufacturer, sysInfo.SystemProductName);
        }
    }
}
