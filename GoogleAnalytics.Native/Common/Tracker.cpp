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
	engine->DocumentEncoding = platformInfoProvider->DocumentEncoding;
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

IBox<Windows::Foundation::Size>^ Tracker::AppScreen::get()
{
	return engine->ViewportSize;
}
void Tracker::AppScreen::set(IBox<Windows::Foundation::Size>^ value)
{
	engine->ViewportSize = value;
}

String^ Tracker::Referrer::get()
{
	return engine->Referrer;
}
void Tracker::Referrer::set(String^ value)
{
	engine->Referrer = value;
}

String^ Tracker::Campaign::get()
{
	return engine->Campaign;
}
void Tracker::Campaign::set(String^ value)
{
	engine->Campaign = value;
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

void Tracker::SetStartSession(bool value)
{
	startSession = value;
}

void Tracker::SetEndSession(bool value)
{
	endSession = value;
}


