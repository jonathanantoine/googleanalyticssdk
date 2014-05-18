//
// PayloadFactory.h
// Declaration of the PayloadFactory class.
//

#pragma once

#include "Payload.h"
#include "SessionControl.h"

namespace GoogleAnalytics
{
	ref class PayloadFactory sealed
	{
	private:

		static Platform::String^ HitType_Pageview;
		
		static Platform::String^ HitType_Event;
		
		static Platform::String^ HitType_Exception;
		
		static Platform::String^ HitType_SocialNetworkInteraction;
		
		static Platform::String^ HitType_UserTiming;
		
		static Platform::String^ HitType_Transaction;
		
		static Platform::String^ HitType_TransactionItem;
		
		GoogleAnalytics::Payload^ PostData(Platform::String^ hitType, Windows::Foundation::Collections::IMap<Platform::String^, Platform::String^>^ additionalData, bool isNonInteractive, GoogleAnalytics::SessionControl sessionControl);
		
		Windows::Foundation::Collections::IMap<Platform::String^, Platform::String^>^ GetRequiredPayloadData(Platform::String^ hitType, bool isNonInteractive, GoogleAnalytics::SessionControl sessionControl);
		
	internal:

		property Platform::String^ PropertyId;

		property Platform::String^ AppName;

		property Platform::String^ AppVersion;

		property Platform::String^ AppId;

		property Platform::String^ AppInstallerId;

		property bool AnonymizeIP;

		property Windows::Foundation::Collections::IMap<int, Platform::String^>^ CustomDimensions;

		property Windows::Foundation::Collections::IMap<int, long long>^ CustomMetrics;

		property Platform::IBox<Windows::Foundation::Size>^ ViewportSize;

		property Platform::String^ ScreenName;

		property Platform::String^ AnonymousClientId;

		property Platform::IBox<Windows::Foundation::Size>^ ScreenResolution;

		property Platform::String^ UserLanguage;

		property Platform::IBox<int>^ ScreenColorDepthBits;

		property Platform::String^ CampaignName;
		property Platform::String^ CampaignSource;
		property Platform::String^ CampaignMedium;
		property Platform::String^ CampaignKeyword;
		property Platform::String^ CampaignContent;
		property Platform::String^ CampaignId;

		property Platform::String^ Referrer;
		property Platform::String^ DocumentEncoding;
		property Platform::String^ GoogleAdWordsId;
		property Platform::String^ GoogleDisplayAdsId;
		property Platform::String^ IpOverride;
		property Platform::String^ UserAgentOverride;
		property Platform::String^ DocumentLocationUrl;
		property Platform::String^ DocumentHostName;
		property Platform::String^ DocumentPath;
		property Platform::String^ DocumentTitle;
		property Platform::String^ LinkId;
		property Platform::String^ ExperimentId;
		property Platform::String^ ExperimentVariant;

		PayloadFactory();
		
		GoogleAnalytics::Payload^ TrackView(Platform::String^ screenName, GoogleAnalytics::SessionControl sessionControl = GoogleAnalytics::SessionControl::None, bool isNonInteractive = false);
		
		GoogleAnalytics::Payload^ TrackEvent(Platform::String^ category, Platform::String^ action, Platform::String^ label, long long value, GoogleAnalytics::SessionControl sessionControl = GoogleAnalytics::SessionControl::None, bool isNonInteractive = false);
		
		GoogleAnalytics::Payload^ TrackException(Platform::String^ description, bool isFatal, GoogleAnalytics::SessionControl sessionControl = GoogleAnalytics::SessionControl::None, bool isNonInteractive = false);
		
		GoogleAnalytics::Payload^ TrackSocialInteraction(Platform::String^ network, Platform::String^ action, Platform::String^ target, GoogleAnalytics::SessionControl sessionControl = GoogleAnalytics::SessionControl::None, bool isNonInteractive = false);
		
		GoogleAnalytics::Payload^ TrackUserTiming(Platform::String^ category, Platform::String^ variable, Platform::IBox<Windows::Foundation::TimeSpan>^ time, Platform::String^ label, Platform::IBox<Windows::Foundation::TimeSpan>^ loadTime, Platform::IBox<Windows::Foundation::TimeSpan>^ dnsTime, Platform::IBox<Windows::Foundation::TimeSpan>^ downloadTime, Platform::IBox<Windows::Foundation::TimeSpan>^ redirectResponseTime, Platform::IBox<Windows::Foundation::TimeSpan>^ tcpConnectTime, Platform::IBox<Windows::Foundation::TimeSpan>^ serverResponseTime, GoogleAnalytics::SessionControl sessionControl = GoogleAnalytics::SessionControl::None, bool isNonInteractive = false);
		
		GoogleAnalytics::Payload^ TrackTransaction(Platform::String^ id, Platform::String^ affiliation, double revenue, double shipping, double tax, Platform::String^ currencyCode, GoogleAnalytics::SessionControl sessionControl = GoogleAnalytics::SessionControl::None, bool isNonInteractive = false);
		
		GoogleAnalytics::Payload^ TrackTransactionItem(Platform::String^ transactionId, Platform::String^ name, double price, long long quantity, Platform::String^ code, Platform::String^ category, Platform::String^ currencyCode, GoogleAnalytics::SessionControl sessionControl = GoogleAnalytics::SessionControl::None, bool isNonInteractive = false);
		
	};
}
