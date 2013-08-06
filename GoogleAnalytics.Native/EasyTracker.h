//
// EasyTracker.h
// Declaration of the EasyTracker class.
//

#pragma once

#include "Tracker.h"
#include "EasyTrackerConfig.h"
#include <ppltasks.h>

namespace GoogleAnalytics
{

	public ref class EasyTracker sealed
	{
	private:

		Windows::Foundation::EventRegistrationToken networkStatusChangedEventToken;

		bool isContextSet;

		static GoogleAnalytics::EasyTracker^ current;
		
		static GoogleAnalytics::Tracker^ tracker;
		
		bool reportingException;

		Platform::IBox<Windows::Foundation::DateTime>^ suspended;
		
		void InitTracker();

		concurrency::task<void> InitConfig(Windows::Foundation::Uri^ configPath);
		
		void InitConfig(Windows::Data::Xml::Dom::XmlDocument^ doc);

		EasyTracker();

		void PopulateMissingConfig();

		void NetworkInformation_NetworkStatusChanged(Platform::Object^ sender);

		static void UpdateConnectionStatus();

		concurrency::task<void> _SetContext(Platform::Object^ ctx);

	public:
		
		void OnAppResuming();

		Windows::Foundation::IAsyncAction^ OnAppSuspending();

		void OnUnhandledException(Platform::Exception^ ex, bool* handled);

		Windows::Foundation::IAsyncAction^ SetContext(Platform::Object^ ctx);

		property Windows::Foundation::Uri^ ConfigPath;

		property GoogleAnalytics::EasyTrackerConfig^ Config;

		static property GoogleAnalytics::EasyTracker^ Current
		{
			GoogleAnalytics::EasyTracker^ get();
		}
		
		static GoogleAnalytics::Tracker^ GetTracker();
		
		Windows::Foundation::IAsyncAction^ Dispatch();
		
		virtual ~EasyTracker();

	};
}
