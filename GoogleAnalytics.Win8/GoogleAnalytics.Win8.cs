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
            MessageDialog msgDialog = new MessageDialog("Allow anonomous information to be collected to help improve this application?", "Help Improve User Experience");
            var optInCommand = new UICommand("Yes");
            var optOutCommand = new UICommand("No");
            msgDialog.Commands.Add(optInCommand);
            msgDialog.Commands.Add(optOutCommand);
            var result = await msgDialog.ShowAsync();
            AppOptOut = (result != optInCommand);
            return AppOptOut;
        }
    }
}
