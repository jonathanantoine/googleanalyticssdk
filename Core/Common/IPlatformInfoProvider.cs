using System;

namespace GoogleAnalytics.Core
{
    public interface IPlatformInfoProvider
    {
        string AnonymousClientId { get; set; }
        void OnTracking();
        int? ScreenColorDepthBits { get; }
        Dimensions ScreenResolution { get; }
        string UserLanguage { get; }
        Dimensions ViewPortResolution { get; }
        string GetUserAgent();

#if NETFX_CORE
        event EventHandler<object> ViewPortResolutionChanged;
        event EventHandler<object> ScreenResolutionChanged;
#else
        event EventHandler ViewPortResolutionChanged;
        event EventHandler ScreenResolutionChanged;
#endif
    }
}
