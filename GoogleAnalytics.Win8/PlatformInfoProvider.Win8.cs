using System;
using Windows.Foundation;
using Windows.Storage;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;

namespace GoogleAnalytics
{
    public sealed class PlatformInfoProvider
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
                    if (ApplicationView.Value == ApplicationViewState.FullScreenLandscape)
                    {
                        ScreenResolution = new Size(Window.Current.Bounds.Width, Window.Current.Bounds.Height);
                    }
                    else if (ApplicationView.Value == ApplicationViewState.FullScreenPortrait)
                    {
                        ScreenResolution = new Size(Window.Current.Bounds.Height, Window.Current.Bounds.Width);
                    }
                    ViewPortResolution = new Size(Window.Current.Bounds.Width, Window.Current.Bounds.Height);
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

        void Current_SizeChanged(object sender, Windows.UI.Core.WindowSizeChangedEventArgs e)
        {
            ViewPortResolution = e.Size;
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

        Size? viewPortResolution;
        public Size? ViewPortResolution
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

        Size? screenResolution;
        // Another possible solution: http://blogs.microsoft.co.il/blogs/tomershamam/archive/2012/07/24/get-screen-resolution-in-windows-8-metro-style-application.aspx
        public Size? ScreenResolution
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

        public int? ScreenColorDepthBits
        {
            get { return null; }
        }

        public string DocumentEncoding
        {
            get { return null; }
        }
    }
}
