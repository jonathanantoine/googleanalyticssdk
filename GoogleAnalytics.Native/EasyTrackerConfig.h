//
// EasyTrackerConfig.h
// Declaration of the EasyTrackerConfig class.
//

#pragma once

namespace GoogleAnalytics
{
	public ref class EasyTrackerConfig sealed
	{
	private:
		static Platform::String^ ns;

	internal:
		
		void Validate();

		static GoogleAnalytics::EasyTrackerConfig^ Load(Windows::Data::Xml::Dom::XmlDocument^ doc);

	public:

		EasyTrackerConfig();

		/// <summary>
		/// The Google Analytics tracking ID to which to send your data. Dashes in the ID must be unencoded. You can disable your tracking by not providing this value. 
		/// </summary>
		property Platform::String^ TrackingId;

		/// <summary>
		/// The name of your app, used in the app name dimension in your reports. Defaults to the value found in the package. 
		/// </summary>
		property Platform::String^ AppName;

		/// <summary>
		/// The version of your application, used in the app version dimension within your reports. Defaults to the version found in the package. 
		/// </summary>
		property Platform::String^ AppVersion;

		/// <summary>
		/// Flag to enable or writing of debug information to the log, useful for troubleshooting your implementation. false by default. 
		/// </summary>
		property bool Debug;

		/// <summary>
		/// The dispatch period in seconds. Defaults to 30 seconds. 
		/// </summary>
		property Windows::Foundation::TimeSpan DispatchPeriod;

		/// <summary>
		/// The sample rate to use. Default is 100.0. It can be any value between 0.0 and 100.0 
		/// </summary>
		property float SampleFrequency;

		/// <summary>
		/// Automatically track a screen view each time a user starts an Activity. false by default. 
		/// </summary>
		property bool AutoActivityTracking;

		/// <summary>
		/// Tells Google Analytics to anonymize the information sent by the tracker objects by removing the last octet of the IP address prior to its storage. Note that this will slightly reduce the accuracy of geographic reporting. false by default.
		/// </summary>
		property bool AnonymizeIp;

		/// <summary>
		/// Automatically track an Exception each time an uncaught exception is thrown in your application. false by default. 
		/// </summary>
		property bool ReportUncaughtExceptions;

		/// <summary>
		/// The amount of time your application can stay in the background before the session is ended. Default is 30 seconds. Null value disables EasyTracker session management.
		/// </summary>
		property Platform::IBox<Windows::Foundation::TimeSpan>^ SessionTimeout;

		/// <summary>
		/// Automatically track application lifetime events (e.g. suspend and resume). true by default.
		/// </summary>
		property bool AutoAppLifetimeTracking;

		/// <summary>
		/// Automatically monitor suspend and resume events. If true, all dispatched events will be sent on suspend and session timeout will be honored. true by default.
		/// </summary>
		property bool AutoAppLifetimeMonitoring;

	};
}
