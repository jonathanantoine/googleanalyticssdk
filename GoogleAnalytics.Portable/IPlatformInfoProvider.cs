using System;

namespace GoogleAnalytics
{
    public interface IPlatformInfoProvider
    {
        string AnonymousClientId { get; }
        string DocumentEncoding { get; }
        void OnTracking();
        int? ScreenColorDepthBits { get; }
        Dimensions? ScreenResolution { get; }
        event EventHandler ScreenResolutionChanged;
        string UserLanguage { get; }
        Dimensions? ViewPortResolution { get; }
        event EventHandler ViewPortResolutionChanged;
        string GetUserAgent();
    }
}
