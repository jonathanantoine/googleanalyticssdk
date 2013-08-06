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

		static GoogleAnalytics::EasyTracker^ current;
		
		static GoogleAnalytics::Tracker^ tracker;
		
		bool reportingException;

		Platform::IBox<Windows::Foundation::DateTime>^ suspended;
		
		void InitTracker();

		EasyTracker();

		void NetworkInformation_NetworkStatusChanged(Platform::Object^ sender);

		static void UpdateConnectionStatus();

	public:
		
		void OnAppResuming();

		Windows::Foundation::IAsyncAction^ OnAppSuspending();

		void OnUnhandledException(Platform::Exception^ ex, bool* handled);
				
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
