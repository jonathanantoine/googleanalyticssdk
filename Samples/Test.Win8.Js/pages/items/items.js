(function () {
    "use strict";

    var appViewState = Windows.UI.ViewManagement.ApplicationViewState;
    var ui = WinJS.UI;

    ui.Pages.define("/pages/items/items.html", {
        // This function is called whenever a user navigates to this page. It
        // populates the page elements with the app's data.
        ready: function (element, options) {
            var listView = element.querySelector(".itemslist").winControl;
            listView.itemDataSource = Data.groups.dataSource;
            listView.itemTemplate = element.querySelector(".itemtemplate");
            listView.oniteminvoked = this._itemInvoked.bind(this);

            this._initializeLayout(listView, Windows.UI.ViewManagement.ApplicationView.value);
            listView.element.focus();

            GoogleAnalytics.EasyTracker.getTracker().sendView("items");
        },

        // This function updates the page layout in response to viewState changes.
        updateLayout: function (element, viewState, lastViewState) {
            /// <param name="element" domElement="true" />

            var listView = element.querySelector(".itemslist").winControl;
            if (lastViewState !== viewState) {
                if (lastViewState === appViewState.snapped || viewState === appViewState.snapped) {
                    var handler = function (e) {
                        listView.removeEventListener("contentanimating", handler, false);
                        e.preventDefault();
                    }
                    listView.addEventListener("contentanimating", handler, false);
                    var firstVisible = listView.indexOfFirstVisible;
                    this._initializeLayout(listView, viewState);
                    if (firstVisible >= 0 && listView.itemDataSource.list.length > 0) {
                        listView.indexOfFirstVisible = firstVisible;
                    }
                }
            }
        },

        // This function updates the ListView with new layouts
        _initializeLayout: function (listView, viewState) {
            /// <param name="listView" value="WinJS.UI.ListView.prototype" />

            if (viewState === appViewState.snapped) {
                listView.layout = new ui.ListLayout();
            } else {
                listView.layout = new ui.GridLayout();
            }
        },

        _itemInvoked: function (args) {
            var groupKey = Data.groups.getAt(args.detail.itemIndex).key;
            
            switch (groupKey) {
                case "error":
                    this._trackError();
                    break;
                case "event":
                    this._trackEvent();
                    break;
                case "pageview":
                    this._trackPageview();
                    break;
                case "apppurchase":
                    this._trackAppPurchase();
                    break;
                case "productpurchase":
                    this._trackProductPurchase();
                    break;
                case "social":
                    this._trackSocial();
                    break;
                case "unhandledexception":
                    this._throwUnhandledException();
                    break;
                case "optout":
                    this._optOut();
                    break;
            }
        },

        _trackError: function () {
            GoogleAnalytics.EasyTracker.getTracker().sendException("oops, something went wrong", false);
        },
        
        _trackEvent: function () {
            GoogleAnalytics.EasyTracker.getTracker().sendEvent("test", "userclick", null, 0);
        },
        
        _trackPageview: function () {
            GoogleAnalytics.EasyTracker.getTracker().sendView("fake");
        },
        
        _trackAppPurchase: function () {
            try
            {
                var currentAppSimulator = Windows.ApplicationModel.Store.CurrentAppSimulator;
                var productId = "test";
                currentAppSimulator.requestAppPurchaseAsync(true).then(function(receipt) {
                    if (currentAppSimulator.licenseInformation.isActive)
                    {
                        currentAppSimulator.loadListingInformationAsync().then(function(listing) {
                            var transaction = GoogleAnalytics.TransactionBuilder.getAppPurchaseTransaction(listing, receipt);
                            GoogleAnalytics.EasyTracker.getTracker().sendTransaction(transaction);
                        });
                    }
                });
            }
            catch (ex)
            {
                GoogleAnalytics.EasyTracker.getTracker().sendException(ex.stackTrace, false);
            }
        },
        
        _trackProductPurchase: function () {
            try
            {
                var currentAppSimulator = Windows.ApplicationModel.Store.CurrentAppSimulator;
                var productId = "test";
                currentAppSimulator.requestProductPurchaseAsync(productId, true).then(function(receipt) {
                    if (currentAppSimulator.licenseInformation.productLicenses.lookup(productId).isActive)
                    {
                        currentAppSimulator.loadListingInformationAsync().then(function(listing) {
                            var transaction = GoogleAnalytics.TransactionBuilder.getProductPurchaseTransaction(listing, receipt);
                            GoogleAnalytics.EasyTracker.getTracker().sendTransaction(transaction);
                        });
                    }
                });
            }
            catch (ex)
            {
                GoogleAnalytics.EasyTracker.getTracker().sendException(ex.stackTrace, false);
            }
        },
        
        _trackSocial: function () {
            GoogleAnalytics.EasyTracker.getTracker().sendSocial("facebook", "share", "http://googleanalyticssdk.codeplex.com");
        },
        
        _throwUnhandledException: function () {
            var x = 1;
            x.notGunnaWork();
        },
        
        _optOut: function () {
            var msgDialog = new Windows.UI.Popups.MessageDialog("Allow anonomous information to be collected to help improve this application?", "Help Improve User Experience");
            var optInCommand = new Windows.UI.Popups.UICommand("Yes");
            var optOutCommand = new Windows.UI.Popups.UICommand("No");
            msgDialog.commands.push(optInCommand);
            msgDialog.commands.push(optOutCommand);
            msgDialog.showAsync().then(function(dialogResult) {
                var boolResult = (dialogResult != optInCommand);
                GoogleAnalytics.AnalyticsEngine.current.appOptOut = boolResult;
            });
        },
    });
})();
