using System;
using System.Windows;

namespace GoogleAnalytics
{
    // http://msdn.microsoft.com/en-us/library/windowsphone/develop/jj206974(v=vs.105).aspx
    internal enum Resolutions { Unknown, WVGA, WXGA, HD720p };

    internal static class ResolutionHelper
    {
        private static bool IsWvga
        {
            get
            {
                return Application.Current.Host.Content.ScaleFactor == 100;
            }
        }

        private static bool IsWxga
        {
            get
            {
                return Application.Current.Host.Content.ScaleFactor == 160;
            }
        }

        private static bool Is720p
        {
            get
            {
                return Application.Current.Host.Content.ScaleFactor == 150;
            }
        }

        public static Resolutions CurrentResolution
        {
            get
            {
                if (IsWvga) return Resolutions.WVGA;
                else if (IsWxga) return Resolutions.WXGA;
                else if (Is720p) return Resolutions.HD720p;
                else return Resolutions.Unknown;
            }
        }

        public static Size? CurrentScreenSize
        {
            get
            {
                switch (CurrentResolution)
                { 
                    case Resolutions.HD720p:
                        return new Size(720 ,1280);
                    case Resolutions.WVGA:
                        return new Size(480 ,800);
                    case Resolutions.WXGA:
                        return new Size(768 ,1280);
                    default:
                        return null;
                }
            }
        }
    }
}
