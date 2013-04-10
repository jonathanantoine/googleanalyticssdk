using System.Windows;

namespace GoogleAnalytics
{
    public sealed partial class GoogleAnalytics
    {
        public bool RequestAppOptOutAsync()
        {
            var result = MessageBox.Show("Allow anonomous information to be collected to help improve this application?", "Help Improve User Experience", MessageBoxButton.OKCancel);
            AppOptOut = (result == MessageBoxResult.Cancel);
            return AppOptOut;
        }
    }
}
