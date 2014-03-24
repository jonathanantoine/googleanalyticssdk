using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GoogleAnalytics.Core
{
    public sealed class PlatformInfoProvider : IPlatformInfoProvider
    {
        Dimensions viewPortResolution;
        Dimensions screenResolution;

#if NETFX_CORE
        public event EventHandler<object> ViewPortResolutionChanged;
        public event EventHandler<object> ScreenResolutionChanged;
#else
        public event EventHandler ViewPortResolutionChanged;
        public event EventHandler ScreenResolutionChanged;
#endif

        public string AnonymousClientId { get; set; }

        public string DocumentEncoding { get; set; }

        public void OnTracking()
        { }

        public int? ScreenColorDepthBits { get; set; }

        public string UserLanguage { get; set; }

        public Dimensions ScreenResolution
        {
            get { return screenResolution; }
            set
            {
                screenResolution = value;
                if (ScreenResolutionChanged != null) ScreenResolutionChanged(this, EventArgs.Empty);
            }
        }

        public Dimensions ViewPortResolution
        {
            get { return viewPortResolution; }
            set
            {
                viewPortResolution = value;
                if (ViewPortResolutionChanged != null) ViewPortResolutionChanged(this, EventArgs.Empty);
            }
        }

        string IPlatformInfoProvider.GetUserAgent()
        {
            return UserAgent;
        }

        public string UserAgent { get; set; }
    }
}
