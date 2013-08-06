//
// AnalyticsEngine.cpp
// Implementation of the AnalyticsEngine class.
//

#include "pch.h"
#include "AnalyticsEngine.h"
#include "GAServiceManager.h"

using namespace GoogleAnalytics;
using namespace Platform;
using namespace Windows::Storage;

String^ AnalyticsEngine::Key_AppOptOut = "GoogleAnaltyics.AppOptOut";

AnalyticsEngine^ AnalyticsEngine::current = nullptr;

void AnalyticsEngine::SendPayload(Payload^ payload)
{
	if (!AppOptOut)
	{
		GAServiceManager::Current->SendPayload(payload);
	}
}

AnalyticsEngine::AnalyticsEngine(PlatformInfoProvider^ platformTrackingInfo)
{
	this->platformTrackingInfo = platformTrackingInfo;
}

AnalyticsEngine^ AnalyticsEngine::Current::get()
{
	if (current == nullptr)
	{
		current = ref new AnalyticsEngine(ref new PlatformInfoProvider());
	}
	return current;
}

Tracker^ AnalyticsEngine::GetTracker(String^ propertyId)
{
	if (propertyId == nullptr) propertyId = "";
	if (trackers.find(propertyId) == end(trackers))
	{
		auto tracker = ref new Tracker(propertyId, platformTrackingInfo, this);
		trackers[propertyId] = tracker;
		if (DefaultTracker == nullptr)
		{
			DefaultTracker = tracker;
		}
		return tracker;
	}
	else
	{
		return trackers[propertyId];
	}
}

void AnalyticsEngine::CloseTracker(Tracker^ tracker)
{
	trackers.erase(tracker->TrackingId);
	if (DefaultTracker == tracker)
	{
		DefaultTracker = nullptr;
	}
}
bool AnalyticsEngine::GetAppOptOut()
{
	if (ApplicationData::Current->LocalSettings->Values->HasKey(Key_AppOptOut))
	{
		appOptOut = (bool)ApplicationData::Current->LocalSettings->Values->Lookup(Key_AppOptOut);
	}
	else
	{
		appOptOut = false;
	}
	return appOptOut->Value;
}

bool AnalyticsEngine::AppOptOut::get()
{
	if (appOptOut != nullptr) return appOptOut->Value;
	return GetAppOptOut();
}
void AnalyticsEngine::AppOptOut::set(bool value)
{
	if (appOptOut == nullptr) GetAppOptOut();
	if (appOptOut->Value != value)
	{
		appOptOut = value;
		ApplicationData::Current->LocalSettings->Values->Insert(Key_AppOptOut, value);
		if (value) GAServiceManager::Current->Clear();
	}
}

