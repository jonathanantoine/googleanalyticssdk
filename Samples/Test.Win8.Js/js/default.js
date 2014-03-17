// For an introduction to the Split template, see the following documentation:
// http://go.microsoft.com/fwlink/?LinkID=232447
(function () {
    "use strict";

    WinJS.Binding.optimizeBindingReferences = true;

    var app = WinJS.Application;
    var activation = Windows.ApplicationModel.Activation;
    var nav = WinJS.Navigation;

    var config = new GoogleAnalytics.EasyTrackerConfig();
    config.trackingId = "UA-39959863-1";
    GoogleAnalytics.EasyTracker.current.config = config;

    app.addEventListener("activated", function (args) {
        if (args.detail.kind === activation.ActivationKind.launch) {
            //GoogleAnalytics.EasyTracker.current.setContext(null);

            if (args.detail.previousExecutionState !== activation.ApplicationExecutionState.terminated) {
                // TODO: This application has been newly launched. Initialize
                // your application here.
            } else {
                // TODO: This application has been reactivated from suspension.
                // Restore application state here.
            }

            if (app.sessionState.history) {
                nav.history = app.sessionState.history;
            }
            args.setPromise(WinJS.UI.processAll().then(function () {
                if (nav.location) {
                    nav.history.current.initialPlaceholder = true;
                    return nav.navigate(nav.location, nav.state);
                } else {
                    return nav.navigate(Application.navigator.home);
                }
            }));

            var storeUri = new Windows.Foundation.Uri("ms-appx:///WindowsStoreProxy.xml");
            Windows.Storage.StorageFile.getFileFromApplicationUriAsync(storeUri).then(function myfunction(storeFile) {
                return Windows.ApplicationModel.Store.CurrentAppSimulator.reloadSimulatorAsync(storeFile);
            });
        }
    });

    Windows.UI.WebUI.WebUIApplication.addEventListener("resuming", function (args) {
        // tell Google Analytics  that we have resumed.
        GoogleAnalytics.EasyTracker.current.onAppResuming();
        // optionally log an event that the app has resumed
        GoogleAnalytics.EasyTracker.getTracker().sendEvent("app", "resume", null, 0);
    });

    app.oncheckpoint = function (args) {
        app.sessionState.history = nav.history;

        // optionally log an event that the app has suspended
        GoogleAnalytics.EasyTracker.getTracker().sendEvent("app", "suspend", null, 0);
        // make sure all Google Analytics data has been dispatched.
        args.setPromise(GoogleAnalytics.EasyTracker.current.onAppSuspending());
    };

    var isReportingException = false; // flag to let GA dispatch the error before app crashes
    app.onerror = function (eventInfo) {
        if (!isReportingException) {
            var error = eventInfo.detail;
            var errorInfo = error.errorMessage + "\n" + error.errorUrl + " (" + error.errorLine + ")"
            GoogleAnalytics.EasyTracker.getTracker().sendException(errorInfo, true);
            isReportingException = true;
            GoogleAnalytics.EasyTracker.current.dispatch().done(function () {
                // once done logging the error, rethrow to resume normal course of action
                throw error;
            });
            return true;
        }
        else return false;
    };

    app.start();
})();
