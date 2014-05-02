//
// PayloadFactory.cpp
// Implementation of the PayloadFactory class.
//

#include "pch.h"
#include "PayloadFactory.h"
#include <collection.h>
#include "TimeSpanHelper.h"

using namespace GoogleAnalytics;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

String^ PayloadFactory::HitType_Pageview = "screenview";
String^ PayloadFactory::HitType_Event = "event";
String^ PayloadFactory::HitType_Exception = "exception";
String^ PayloadFactory::HitType_SocialNetworkInteraction = "social";
String^ PayloadFactory::HitType_UserTiming = "timing";
String^ PayloadFactory::HitType_Transaction = "transaction";
String^ PayloadFactory::HitType_TransactionItem = "item";

PayloadFactory::PayloadFactory()
{
	CustomDimensions = ref new Map<int, String^>();
	CustomMetrics = ref new Map<int, long long>();
	PropertyId = nullptr;
	AppName = nullptr;
	AppVersion = nullptr;
	AppId = nullptr;
	AppInstallerId = nullptr;
	AnonymizeIP = false;
	ViewportSize = nullptr;
	Referrer = nullptr;
	Campaign = nullptr;
	ScreenName = nullptr;
	AnonymousClientId = nullptr;
	ScreenResolution = nullptr;
	UserLanguage = nullptr;
	ScreenColorDepthBits = nullptr;
	DocumentEncoding = nullptr;
}

Payload^ PayloadFactory::TrackView(String^ screenName, SessionControl sessionControl, bool isNonInteractive)
{
	ScreenName = screenName;
	return PostData(HitType_Pageview, nullptr, isNonInteractive, sessionControl);
}

Payload^ PayloadFactory::TrackEvent(String^ category, String^ action, String^ label, long long value, SessionControl sessionControl, bool isNonInteractive)
{
	auto additionalData = ref new Map<String^, String^>();
	additionalData->Insert("ec", category);
	additionalData->Insert("ea", action);
	if (label) additionalData->Insert("el", label);
	if (value) additionalData->Insert("ev", value.ToString());
	return PostData(HitType_Event, additionalData, isNonInteractive, sessionControl);
}

Payload^ PayloadFactory::TrackException(String^ description, bool isFatal, SessionControl sessionControl, bool isNonInteractive)
{
	auto additionalData = ref new Map<String^, String^>();
	if (description) additionalData->Insert("exd", description);
	if (!isFatal) additionalData->Insert("exf", "0");
	return PostData(HitType_Exception, additionalData, isNonInteractive, sessionControl);
}

Payload^ PayloadFactory::TrackSocialInteraction(String^ network, String^ action, String^ target, SessionControl sessionControl, bool isNonInteractive)
{
	auto additionalData = ref new Map<String^, String^>();
	additionalData->Insert("sn", network);
	additionalData->Insert("sa", action);
	additionalData->Insert("st", target);
	return PostData(HitType_SocialNetworkInteraction, additionalData, isNonInteractive, sessionControl);
}

Payload^ PayloadFactory::TrackUserTiming(String^ category, String^ variable, IBox<TimeSpan>^ time, String^ label, IBox<TimeSpan>^ loadTime, IBox<TimeSpan>^ dnsTime, IBox<TimeSpan>^ downloadTime, IBox<TimeSpan>^ redirectResponseTime, IBox<TimeSpan>^ tcpConnectTime, IBox<TimeSpan>^ serverResponseTime, SessionControl sessionControl, bool isNonInteractive)
{
	auto additionalData = ref new Map<String^, String^>();
	if (category) additionalData->Insert("utc", category);
	if (variable) additionalData->Insert("utv", variable);
	if (time != nullptr) additionalData->Insert("utt", std::floor(0.5 + TimeSpanHelper::GetTotalMilliseconds(time->Value)).ToString());
	if (label) additionalData->Insert("utl", label);
	if (loadTime != nullptr) additionalData->Insert("ptl", std::floor(0.5 + TimeSpanHelper::GetTotalMilliseconds(loadTime->Value)).ToString());
	if (dnsTime != nullptr) additionalData->Insert("dns", std::floor(0.5 + TimeSpanHelper::GetTotalMilliseconds(dnsTime->Value)).ToString());
	if (downloadTime != nullptr) additionalData->Insert("pdt", std::floor(0.5 + TimeSpanHelper::GetTotalMilliseconds(downloadTime->Value)).ToString());
	if (redirectResponseTime != nullptr) additionalData->Insert("rrt", std::floor(0.5 + TimeSpanHelper::GetTotalMilliseconds(redirectResponseTime->Value)).ToString());
	if (tcpConnectTime != nullptr) additionalData->Insert("tcp", std::floor(0.5 + TimeSpanHelper::GetTotalMilliseconds(tcpConnectTime->Value)).ToString());
	if (serverResponseTime != nullptr) additionalData->Insert("srt", std::floor(0.5 + TimeSpanHelper::GetTotalMilliseconds(serverResponseTime->Value)).ToString());
	return PostData(HitType_UserTiming, additionalData, isNonInteractive, sessionControl);
}

Payload^ PayloadFactory::TrackTransaction(String^ id, String^ affiliation, double revenue, double shipping, double tax, String^ currencyCode, SessionControl sessionControl, bool isNonInteractive)
{
	auto additionalData = ref new Map<String^, String^>();
	additionalData->Insert("ti", id);
	if (affiliation) additionalData->Insert("ta", affiliation);
	if (revenue) additionalData->Insert("tr", revenue.ToString());
	if (shipping) additionalData->Insert("ts", shipping.ToString());
	if (tax) additionalData->Insert("tt", tax.ToString());
	if (currencyCode) additionalData->Insert("cu", currencyCode);
	return PostData(HitType_Transaction, additionalData, isNonInteractive, sessionControl);
}

Payload^ PayloadFactory::TrackTransactionItem(String^ transactionId, String^ name, double price, long long quantity, String^ code, String^ category, String^ currencyCode, SessionControl sessionControl, bool isNonInteractive)
{
	auto additionalData = ref new Map<String^, String^>();
	additionalData->Insert("ti", transactionId);
	if (name) additionalData->Insert("in", name);
	if (price) additionalData->Insert("ip", price.ToString());
	if (quantity) additionalData->Insert("iq", quantity.ToString());
	if (code) additionalData->Insert("ic", code);
	if (category) additionalData->Insert("iv", category);
	if (currencyCode) additionalData->Insert("cu", currencyCode);
	return PostData(HitType_TransactionItem, additionalData, isNonInteractive, sessionControl);
}

Payload^ PayloadFactory::PostData(String^ hitType, IMap<String^, String^>^ additionalData, bool isNonInteractive, SessionControl sessionControl)
{
	auto payloadData = GetRequiredPayloadData(hitType, isNonInteractive, sessionControl);
	if (additionalData)
	{
		for each (auto item in additionalData)
		{
			payloadData->Insert(item->Key, item->Value);
		}
	}
	return ref new Payload(payloadData);
}

IMap<String^, Platform::String^>^ PayloadFactory::GetRequiredPayloadData(String^ hitType, bool isNonInteractive, SessionControl sessionControl)
{
	auto result = ref new Map<String^, String^>();
	result->Insert("v", "1");
	result->Insert("tid", PropertyId);
	result->Insert("cid", AnonymousClientId);
	result->Insert("an", AppName);
	result->Insert("av", AppVersion);
	result->Insert("t", hitType);
	if (AppId) result->Insert("aid", AppId);
    if (AppInstallerId) result->Insert("aiid", AppInstallerId);
	if (ScreenName) result->Insert("cd", ScreenName);
	if (isNonInteractive) result->Insert("ni", "1");
	if (AnonymizeIP) result->Insert("aip", "1");
	if (sessionControl != SessionControl::None) result->Insert("sc", sessionControl == SessionControl::Start ? "start" : "end");
	if (ScreenResolution != nullptr) result->Insert("sr", ScreenResolution->Value.Width.ToString() + "x" + ScreenResolution->Value.Height.ToString());
	if (ViewportSize != nullptr) result->Insert("vp", ViewportSize->Value.Width.ToString() + "x" + ViewportSize->Value.Height.ToString());
	if (UserLanguage) result->Insert("ul", UserLanguage);
	if (ScreenColorDepthBits) result->Insert("sd", ScreenColorDepthBits->Value.ToString() + "-bits");
	if (DocumentEncoding) result->Insert("de", DocumentEncoding);
	for each (auto dimension in CustomDimensions)
	{
		result->Insert("cd" + dimension->Key, dimension->Value);
	}
	for each (auto metric in CustomMetrics)
	{
		result->Insert("cm" + metric->Key, metric->Value.ToString());
	}
	CustomDimensions->Clear();
	CustomMetrics->Clear();

	return result;
}
