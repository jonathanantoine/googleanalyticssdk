using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.UI.Popups;

namespace GoogleAnalytics
{
    public sealed partial class GoogleAnalytics
    {
        public IAsyncOperation<bool> RequestAppOptOutAsync()
        {
            return _RequestAppOptOutAsync().AsAsyncOperation();
        }

        async Task<bool> _RequestAppOptOutAsync()
        {
            MessageDialog msgDialog = new MessageDialog("Allow anonomous information to be collected to help improve this application?", "Custom Experience Improvement Program");
            var okCommand = new UICommand("OK");
            var cancelCommand = new UICommand("Cancel");
            msgDialog.Commands.Add(okCommand);
            msgDialog.Commands.Add(cancelCommand);
            var result = await msgDialog.ShowAsync();
            AppOptOut = (result != okCommand);
            return AppOptOut;
        }
    }
}
