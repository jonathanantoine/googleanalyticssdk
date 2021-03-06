//
// Tracker.h
// Declaration of the Tracker class.
//

#pragma once

#include "PayloadFactory.h"
//#include "PlatformInfoProvider.h"
#include "TokenBucket.h"
#include "AnalyticsEngine.h"
#include "Transaction.h"
#include "SessionControl.h"
#include "Payload.h"

namespace GoogleAnalytics
{
	ref class AnalyticsEngine;

	public ref class Tracker sealed
	{
	private:

		Windows::Foundation::EventRegistrationToken viewPortResolutionChangedEventToken;

		Windows::Foundation::EventRegistrationToken screenResolutionChangedEventToken;

		GoogleAnalytics::PayloadFactory^ engine;

		GoogleAnalytics::PlatformInfoProvider^ platformInfoProvider;

		GoogleAnalytics::TokenBucket^ hitTokenBucket;

		GoogleAnalytics::AnalyticsEngine^ analyticsEngine;

		void platformTrackingInfo_ViewPortResolutionChanged(Platform::Object^ sender, Platform::Object^ args);

		void platformTrackingInfo_ScreenResolutionChanged(Platform::Object^ sender, Platform::Object^ args);

		Windows::Foundation::Collections::IVector<GoogleAnalytics::Payload^>^ TrackTransaction(GoogleAnalytics::Transaction^ transaction, GoogleAnalytics::SessionControl sessionControl = GoogleAnalytics::SessionControl::None, bool isNonInteractive = false);

		property GoogleAnalytics::SessionControl SessionControl
		{
			GoogleAnalytics::SessionControl get();
		}

		bool startSession;

		bool endSession;

		void SendPayload(GoogleAnalytics::Payload^ payload);

		bool IsSampledOut();

	internal:

		Tracker(Platform::String^ propertyId, GoogleAnalytics::PlatformInfoProvider^ platformInfoProvider, GoogleAnalytics::AnalyticsEngine^ analyticsEngine);

	public:

		void SetCustomDimension(int index, Platform::String^ value);

		void SetCustomMetric(int index, long long value);

		property Platform::String^ TrackingId
		{
			Platform::String^ get();
		}

		property bool IsAnonymizeIpEnabled
		{
			bool get();
			void set(bool value);
		}

		property Platform::String^ AppName
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

		property Platform::String^ AppVersion
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

		property Platform::String^ AppId
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

		property Platform::String^ AppInstallerId
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

		property Platform::IBox<Windows::Foundation::Size>^ AppScreen
		{
			Platform::IBox<Windows::Foundation::Size>^ get();
			void set(Platform::IBox<Windows::Foundation::Size>^ value);
		}

		property Platform::String^ CampaignName
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

		property Platform::String^ CampaignSource
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

		property Platform::String^ CampaignMedium
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

		property Platform::String^ CampaignKeyword
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

		property Platform::String^ CampaignContent
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

		property Platform::String^ CampaignId
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

		property Platform::String^ Referrer
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

		property Platform::String^ DocumentEncoding
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

		property Platform::String^ GoogleAdWordsId
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

		property Platform::String^ GoogleDisplayAdsId
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

		property Platform::String^ IpOverride
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

		property Platform::String^ UserAgentOverride
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

		property Platform::String^ DocumentLocationUrl
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

		property Platform::String^ DocumentHostName
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

		property Platform::String^ DocumentPath
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

		property Platform::String^ DocumentTitle
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

		property Platform::String^ LinkId
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

		property Platform::String^ ExperimentId
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

		property Platform::String^ ExperimentVariant
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

		property Platform::String^ DataSource
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

		property Platform::String^ UserId
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

		property Platform::String^ GeographicalId
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

		property float SampleRate;

		property bool IsUseSecure;

		property bool IsDebug;

		property bool ThrottlingEnabled;

		void SendView(Platform::String^ screenName);

		void SendException(Platform::String^ description, bool isFatal);

		void SendSocial(Platform::String^ network, Platform::String^ action, Platform::String^ target);

		void SendTiming(Windows::Foundation::TimeSpan time, Platform::String^ category, Platform::String^ variable, Platform::String^ label);

		void SendEvent(Platform::String^ category, Platform::String^ action, Platform::String^ label, long long value);

		void SendTransaction(GoogleAnalytics::Transaction^ transaction);

		void SendTransactionItem(GoogleAnalytics::TransactionItem^ transactionItem);

		void SetStartSession(bool value);

		void SetEndSession(bool value);

	};
}
