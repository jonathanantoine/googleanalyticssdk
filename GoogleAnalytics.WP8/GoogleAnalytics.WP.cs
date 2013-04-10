using System.Windows;

namespace GoogleAnalytics
{
    public sealed partial class GoogleAnalytics
    {
        public bool RequestAppOptOutAsync()
        {
            var result = MessageBox.Show("Allow anonomous information to be collected to help improve this application?", "Custom Experience Improvement Program", MessageBoxButton.OKCancel);
            AppOptOut = (result == MessageBoxResult.Cancel);
            return AppOptOut;
        }
    }
}
