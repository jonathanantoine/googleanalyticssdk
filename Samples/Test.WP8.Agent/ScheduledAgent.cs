using System.Diagnostics;
using System.Windows;
using Microsoft.Phone.Scheduler;
using GoogleAnalytics.Core;

namespace Test.WP8.Agent
{
    public class ScheduledAgent : ScheduledTaskAgent
    {
        static Tracker tracker;

        /// <remarks>
        /// ScheduledAgent constructor, initializes the UnhandledException handler
        /// </remarks>
        static ScheduledAgent()
        {
            tracker = new Tracker("UA-39959863-1", new GoogleAnalytics.PlatformInfoProvider(), GAServiceManager.Current);
            tracker.AppName = "My agent";
            tracker.AppVersion = "1.0";
            
            // Subscribe to the managed exception handler
            Deployment.Current.Dispatcher.BeginInvoke(delegate
            {
                Application.Current.UnhandledException += UnhandledException;
            });
        }

        /// Code to execute on Unhandled Exceptions
        private static void UnhandledException(object sender, ApplicationUnhandledExceptionEventArgs e)
        {
            tracker.SendException(e.ExceptionObject.ToString(), true);

            if (Debugger.IsAttached)
            {
                // An unhandled exception has occurred; break into the debugger
                Debugger.Break();
            }
        }

        /// <summary>
        /// Agent that runs a scheduled task
        /// </summary>
        /// <param name="task">
        /// The invoked task
        /// </param>
        /// <remarks>
        /// This method is called when a periodic or resource intensive task is invoked
        /// </remarks>
        protected override void OnInvoke(ScheduledTask task)
        {
            //TODO: Add code to perform your task in background
            tracker.SendEvent("OnInvoke", "action", "label", 0);

            NotifyComplete();
        }
    }
}