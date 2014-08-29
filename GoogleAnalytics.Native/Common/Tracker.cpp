//
// Tracker.cpp
// Implementation of the Tracker class.
//

#include "pch.h"
#include "Tracker.h"

using namespace GoogleAnalytics;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;

Tracker::Tracker(String^ propertyId, PlatformInfoProvider^ platformInfoProvider, AnalyticsEngine^ analyticsEngine) :
startSession(false),
endSession(false)
{
	this->analyticsEngine = analyticsEngine;
	this->platformInfoProvider = platformInfoProvider;
	engine = ref new PayloadFactory();
	engine->PropertyId = propertyId;
	engine->AnonymousClientId = platformInfoProvider->AnonymousClientId;
	engine->ScreenColorDepthBits = platformInfoProvider->ScreenColorDepthBits;
	engine->ScreenResolution = platformInfoProvider->ScreenResolution;
	engine->UserLanguage = platformInfoProvider->UserLanguage;
	engine->ViewportSize = platformInfoProvider->ViewPortResolution;
	viewPortResolutionChangedEventToken = platformInfoProvider->ViewPortResolutionChanged += ref new EventHandler<Object^>(this, &Tracker::platformTrackingInfo_ViewPortResolutionChanged);
	screenResolutionChangedEventToken = platformInfoProvider->ScreenResolutionChanged += ref new EventHandler<Object^>(this, &Tracker::platformTrackingInfo_ScreenResolutionChanged);
	SampleRate = 100.0F;
	hitTokenBucket = ref new TokenBucket(60, .5);
}

void Tracker::platformTrackingInfo_ViewPortResolutionChanged(Object^ sender, Object^ args)
{
	engine->ViewportSize = platformInfoProvider->ViewPortResolution;
}

void Tracker::platformTrackingInfo_ScreenResolutionChanged(Object^ sender, Object^ args)
{
	engine->ScreenResolution = platformInfoProvider->ScreenResolution;
}

IVector<Payload^>^ Tracker::TrackTransaction(Transaction^ transaction, GoogleAnalytics::SessionControl sessionControl, bool isNonInteractive)
{
	auto result = ref new Vector<Payload^>();
	result->Append(engine->TrackTransaction(transaction->TransactionId, transaction->Affiliation, (double)transaction->TotalCostInMicros / 1000000, (double)transaction->ShippingCostInMicros / 1000000, (double)transaction->TotalTaxInMicros / 1000000, transaction->CurrencyCode, sessionControl, isNonInteractive));

	for each (auto item in transaction->Items)
	{
		result->Append(engine->TrackTransactionItem(transaction->TransactionId, item->Name, (double)item->PriceInMicros / 1000000, item->Quantity, item->SKU, item->Category, transaction->CurrencyCode, sessionControl, isNonInteractive));
	}
	return result;
}

GoogleAnalytics::SessionControl Tracker::SessionControl::get()
{
	if (endSession)
	{
		endSession = false;
		return GoogleAnalytics::SessionControl::End;
	}
	else if (startSession)
	{
		startSession = false;
		return GoogleAnalytics::SessionControl::Start;
	}
	else
	{
		return GoogleAnalytics::SessionControl::None;
	}
}

void Tracker::SendPayload(Payload^ payload)
{
	if (TrackingId)
	{
		if (!IsSampledOut())
		{
			if (!ThrottlingEnabled || hitTokenBucket->Consume())
			{
				payload->IsUseSecure = IsUseSecure;
				analyticsEngine->SendPayload(payload);
			}
		}
	}
}

bool Tracker::IsSampledOut()
{
	if (SampleRate <= 0.0F)
	{
		return true;
	}
	else if (SampleRate < 100.0F)
	{
		auto clientId = platformInfoProvider->AnonymousClientId;
		return ((clientId != nullptr) && (std::abs(clientId->GetHashCode()) % 10000 >= SampleRate * 100.0F));
	}
	else return false;
}

void Tracker::SetCustomDimension(int index, String^ value)
{
	engine->CustomDimensions->Insert(index, value);
}

void Tracker::SetCustomMetric(int index, long long value)
{
	engine->CustomMetrics->Insert(index, value);
}

String^ Tracker::TrackingId::get()
{
	return engine->PropertyId;
}

bool Tracker::IsAnonymizeIpEnabled::get()
{
	return engine->AnonymizeIP;
}
void Tracker::IsAnonymizeIpEnabled::set(bool value)
{
	engine->AnonymizeIP = value;
}

String^ Tracker::AppName::get()
{
	return engine->AppName;
}
void Tracker::AppName::set(String^ value)
{
	engine->AppName = value;
}

String^ Tracker::AppVersion::get()
{
	return engine->AppVersion;
}
void Tracker::AppVersion::set(String^ value)
{
	engine->AppVersion = value;
}

String^ Tracker::AppId::get()
{
	return engine->AppId;
}
void Tracker::AppId::set(String^ value)
{
	engine->AppId = value;
}

String^ Tracker::AppInstallerId::get()
{
	return engine->AppInstallerId;
}
void Tracker::AppInstallerId::set(String^ value)
{
	engine->AppInstallerId = value;
}

IBox<Windows::Foundation::Size>^ Tracker::AppScreen::get()
{
	return engine->ViewportSize;
}
void Tracker::AppScreen::set(IBox<Windows::Foundation::Size>^ value)
{
	engine->ViewportSize = value;
}

String^ Tracker::CampaignName::get()
{
	return engine->CampaignName;
}
void Tracker::CampaignName::set(String^ value)
{
	engine->CampaignName = value;
}

String^ Tracker::CampaignSource::get()
{
	return engine->CampaignSource;
}
void Tracker::CampaignSource::set(String^ value)
{
	engine->CampaignSource = value;
}

String^ Tracker::CampaignMedium::get()
{
	return engine->CampaignMedium;
}
void Tracker::CampaignMedium::set(String^ value)
{
	engine->CampaignMedium = value;
}

String^ Tracker::CampaignKeyword::get()
{
	return engine->CampaignKeyword;
}
void Tracker::CampaignKeyword::set(String^ value)
{
	engine->CampaignKeyword = value;
}

String^ Tracker::CampaignContent::get()
{
	return engine->CampaignContent;
}
void Tracker::CampaignContent::set(String^ value)
{
	engine->CampaignContent = value;
}

String^ Tracker::CampaignId::get()
{
	return engine->CampaignId;
}
void Tracker::CampaignId::set(String^ value)
{
	engine->CampaignId = value;
}

String^ Tracker::Referrer::get()
{
	return engine->Referrer;
}
void Tracker::Referrer::set(String^ value)
{
	engine->Referrer = value;
}

String^ Tracker::DocumentEncoding::get()
{
	return engine->DocumentEncoding;
}
void Tracker::DocumentEncoding::set(String^ value)
{
	engine->DocumentEncoding = value;
}

String^ Tracker::GoogleAdWordsId::get()
{
	return engine->GoogleAdWordsId;
}
void Tracker::GoogleAdWordsId::set(String^ value)
{
	engine->GoogleAdWordsId = value;
}

String^ Tracker::GoogleDisplayAdsId::get()
{
	return engine->GoogleDisplayAdsId;
}
void Tracker::GoogleDisplayAdsId::set(String^ value)
{
	engine->GoogleDisplayAdsId = value;
}

String^ Tracker::IpOverride::get()
{
	return engine->IpOverride;
}
void Tracker::IpOverride::set(String^ value)
{
	engine->IpOverride = value;
}

String^ Tracker::UserAgentOverride::get()
{
	return engine->UserAgentOverride;
}
void Tracker::UserAgentOverride::set(String^ value)
{
	engine->UserAgentOverride = value;
}

String^ Tracker::DocumentLocationUrl::get()
{
	return engine->DocumentLocationUrl;
}
void Tracker::DocumentLocationUrl::set(String^ value)
{
	engine->DocumentLocationUrl = value;
}

String^ Tracker::DocumentHostName::get()
{
	return engine->DocumentHostName;
}
void Tracker::DocumentHostName::set(String^ value)
{
	engine->DocumentHostName = value;
}

String^ Tracker::DocumentPath::get()
{
	return engine->DocumentPath;
}
void Tracker::DocumentPath::set(String^ value)
{
	engine->DocumentPath = value;
}

String^ Tracker::DocumentTitle::get()
{
	return engine->DocumentTitle;
}
void Tracker::DocumentTitle::set(String^ value)
{
	engine->DocumentTitle = value;
}

String^ Tracker::LinkId::get()
{
	return engine->LinkId;
}
void Tracker::LinkId::set(String^ value)
{
	engine->LinkId = value;
}

String^ Tracker::ExperimentId::get()
{
	return engine->ExperimentId;
}
void Tracker::ExperimentId::set(String^ value)
{
	engine->ExperimentId = value;
}

String^ Tracker::ExperimentVariant::get()
{
	return engine->ExperimentVariant;
}
void Tracker::ExperimentVariant::set(String^ value)
{
	engine->ExperimentVariant = value;
}

void Tracker::SendView(String^ screenName)
{
	platformInfoProvider->OnTracking(); // give platform info provider a chance to refresh.
	auto payload = engine->TrackView(screenName, SessionControl);
	SendPayload(payload);
}

void Tracker::SendException(String^ description, bool isFatal)
{
	platformInfoProvider->OnTracking(); // give platform info provider a chance to refresh.
	auto payload = engine->TrackException(description, isFatal, SessionControl);
	SendPayload(payload);
}

void Tracker::SendSocial(String^ network, String^ action, String^ target)
{
	platformInfoProvider->OnTracking(); // give platform info provider a chance to refresh.
	auto payload = engine->TrackSocialInteraction(network, action, target, SessionControl);
	SendPayload(payload);
}

void Tracker::SendTiming(TimeSpan time, String^ category, String^ variable, String^ label)
{
	platformInfoProvider->OnTracking(); // give platform info provider a chance to refresh.
	auto payload = engine->TrackUserTiming(category, variable, time, label, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, SessionControl);
	SendPayload(payload);
}

void Tracker::SendEvent(String^ category, String^ action, String^ label, long long value)
{
	platformInfoProvider->OnTracking(); // give platform info provider a chance to refresh.
	auto payload = engine->TrackEvent(category, action, label, value, SessionControl);
	SendPayload(payload);
}

void Tracker::SendTransaction(Transaction^ transaction)
{
	platformInfoProvider->OnTracking(); // give platform info provider a chance to refresh.
	for each (auto payload in TrackTransaction(transaction, SessionControl))
	{
		SendPayload(payload);
	}
}

void Tracker::SendTransactionItem(TransactionItem^ transactionItem)
{
	platformInfoProvider->OnTracking(); // give platform info provider a chance to refresh.
	auto payload = engine->TrackTransactionItem(transactionItem->TransactionId, transactionItem->Name, (double)transactionItem->PriceInMicros / 1000000, transactionItem->Quantity, transactionItem->SKU, transactionItem->Category, transactionItem->CurrencyCode, SessionControl);
	SendPayload(payload);
}

void Tracker::SetStartSession(bool value)
{
	startSession = value;
}

void Tracker::SetEndSession(bool value)
{
	endSession = value;
}


