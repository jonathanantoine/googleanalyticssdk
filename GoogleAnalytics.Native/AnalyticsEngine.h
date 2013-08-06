//
// AnalyticsEngine.h
// Declaration of the AnalyticsEngine class.
//

#pragma once

#include <collection.h>
#include <unordered_map>
#include "PlatformInfoProvider.h"
#include "Tracker.h"
#include "Payload.h"

namespace GoogleAnalytics
{
	ref class Tracker;

	public ref class AnalyticsEngine sealed
	{
	private:

		static GoogleAnalytics::AnalyticsEngine^ current;

		GoogleAnalytics::PlatformInfoProvider^ platformTrackingInfo;

		std::unordered_map<Platform::String^, GoogleAnalytics::Tracker^> trackers;

		AnalyticsEngine(GoogleAnalytics::PlatformInfoProvider^ platformTrackingInfo);
				
        bool GetAppOptOut();
		
        Platform::Box<bool>^ appOptOut;
		
        static Platform::String^ Key_AppOptOut;

	internal:

		void SendPayload(GoogleAnalytics::Payload^ payload);

	public:

		static property GoogleAnalytics::AnalyticsEngine^ Current
		{
			GoogleAnalytics::AnalyticsEngine^ get();
		}
		
		property GoogleAnalytics::Tracker^ DefaultTracker;
		
		property bool IsDebugEnabled;
		
		GoogleAnalytics::Tracker^ GetTracker(Platform::String^ propertyId);
		
		void CloseTracker(GoogleAnalytics::Tracker^ tracker);
		
		property bool AppOptOut
		{
			bool get();
			void set(bool value);
		}
	};
}
