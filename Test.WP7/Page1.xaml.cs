using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
using Microsoft.Phone.Controls;
using Microsoft.Phone.Shell;

namespace Test.WP7
{
    public partial class Page1 : PhoneApplicationPage
    {
        public Page1()
        {
            InitializeComponent();
        }

        protected override void OnNavigatedTo(System.Windows.Navigation.NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);
            GoogleAnalytics.EasyTracker.GetTracker().SendView("Page1");
        }

        private void ButtonException_Click(object sender, RoutedEventArgs e)
        {
            GoogleAnalytics.EasyTracker.GetTracker().SendException("oops, something went wrong", false);
        }

        private void ButtonEvent_Click(object sender, RoutedEventArgs e)
        {
            GoogleAnalytics.EasyTracker.GetTracker().SendEvent("test", "userclick", null, 0);
        }

        private void ButtonView_Click(object sender, RoutedEventArgs e)
        {
            GoogleAnalytics.EasyTracker.GetTracker().SendView("fake");
        }

        private void ButtonTransaction_Click(object sender, RoutedEventArgs e)
        {
            double cost = 1.99;
            long costInMicrons = (long)(cost * 1000000);
            var transaction = new GoogleAnalytics.Transaction("01234", costInMicrons);
            var item = new GoogleAnalytics.TransactionItem("myproduct", "My Product", costInMicrons, 1);
            transaction.Items.Add(item);

            GoogleAnalytics.EasyTracker.GetTracker().SendTransaction(transaction);
        }

        private void ButtonSocial_Click(object sender, RoutedEventArgs e)
        {
            GoogleAnalytics.EasyTracker.GetTracker().SendSocial("facebook", "share", "http://googleanalyticssdk.codeplex.com");
        }

        private void ButtonTiming_Click(object sender, RoutedEventArgs e)
        {
            GoogleAnalytics.EasyTracker.GetTracker().SendException("oops, something went wrong", false);
        }

        private void ButtonThrowException_Click(object sender, RoutedEventArgs e)
        {
            object y = 1;
            string x = (string)y;
        }

        private void ButtonOptOut_Click(object sender, RoutedEventArgs e)
        {
            GoogleAnalytics.GoogleAnalytics.Current.RequestAppOptOutAsync();
        }
    }
}