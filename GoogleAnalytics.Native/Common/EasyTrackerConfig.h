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

	public:
		
		/// <summary>
		/// Loads an EasyTrackerConfig file from an Xml Document
		/// </summary>
		static GoogleAnalytics::EasyTrackerConfig^ Load(Windows::Data::Xml::Dom::XmlDocument^ doc);
		
		/// <summary>
		/// Creates a new instance of EasyTrackerConfig.
		/// </summary>
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
		/// The dispatch period in seconds. Defaults to 0 seconds (which indicates to send immediately). 
		/// </summary>
		property Windows::Foundation::TimeSpan DispatchPeriod;

		/// <summary>
		/// The sample rate to use. Default is 100.0. It can be any value between 0.0 and 100.0 
		/// </summary>
		property float SampleFrequency;

		/// <summary>
		/// Tells Google Analytics to anonymize the information sent by the tracker objects by removing the last octet of the IP address prior to its storage. Note that this will slightly reduce the accuracy of geographic reporting. false by default.
		/// </summary>
		property bool AnonymizeIp;

		/// <summary>
		/// If true, causes all hits to be sent to the secure (SSL) Google Analytics endpoint. Default is false.
		/// </summary>
		property bool UseSecure;

		/// <summary>
		/// Tells Google Analytics to automatically monitor network connectivity and avoid sending logs while not connected. Default is true.
		/// </summary>
		property bool AutoTrackNetworkConnectivity;

		/// <summary>
		/// The amount of time your application can stay in the background before the session is ended. Default is 30 seconds. Null value disables EasyTracker session management.
		/// </summary>
		property Platform::IBox<Windows::Foundation::TimeSpan>^ SessionTimeout;
		
	};
}
