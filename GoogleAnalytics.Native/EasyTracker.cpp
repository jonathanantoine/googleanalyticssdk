//
// EasyTracker.cpp
// Implementation of the EasyTracker class.
//

#include "pch.h"
#include "EasyTracker.h"
#include "GAServiceManager.h"
#include "TimeSpanHelper.h"
#include "DateTimeHelper.h"

using namespace GoogleAnalytics;
using namespace Platform;
using namespace Windows::Data::Xml::Dom;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace concurrency;
using namespace Windows::Networking::Connectivity;
using namespace Windows::ApplicationModel;

EasyTracker^ EasyTracker::current = nullptr;

Tracker^ EasyTracker::tracker = nullptr;

void EasyTracker::InitTracker()
{
	if (Config == nullptr) throw ref new NullReferenceException("Config not set. Set EasyTracker.Current.Config = EasyTrackerConfig before getting tracker.");
	if (Config->AutoTrackNetworkConnectivity)
	{
		UpdateConnectionStatus();
		networkStatusChangedEventToken = NetworkInformation::NetworkStatusChanged += ref new NetworkStatusChangedEventHandler(this, &EasyTracker::NetworkInformation_NetworkStatusChanged);
	}

	auto ga = AnalyticsEngine::Current;
	GAServiceManager::Current->DispatchPeriod = Config->DispatchPeriod;
	tracker = ga->GetTracker(Config->TrackingId);
	tracker->SetStartSession(Config->SessionTimeout != nullptr);
	tracker->IsUseSecure = Config->UseSecure;
	tracker->AppName = Config->AppName;
	tracker->AppVersion = Config->AppVersion;
	tracker->IsAnonymizeIpEnabled = Config->AnonymizeIp;
	tracker->SampleRate = Config->SampleFrequency;
}

EasyTracker^ EasyTracker::Current::get()
{
	if (current == nullptr)
	{
		current = ref new EasyTracker();
	}
	return current;
}

Tracker^ EasyTracker::GetTracker()
{
	if (tracker == nullptr) 
	{
		Current->InitTracker();
	}
	return tracker;
}

IAsyncAction^ EasyTracker::Dispatch()
{
	return GAServiceManager::Current->Dispatch();
}

EasyTracker::EasyTracker()
{ }

void EasyTracker::NetworkInformation_NetworkStatusChanged(Object^ sender)
{
	UpdateConnectionStatus();
}

void EasyTracker::UpdateConnectionStatus()
{
	auto profile = NetworkInformation::GetInternetConnectionProfile();
	if (profile != nullptr)
	{
		switch (profile->GetNetworkConnectivityLevel())
		{
		case NetworkConnectivityLevel::InternetAccess:
		case NetworkConnectivityLevel::ConstrainedInternetAccess:
			GAServiceManager::Current->IsConnected = true;
			break;
		default:
			GAServiceManager::Current->IsConnected = false;
			break;
		}
	}
}

void EasyTracker::OnAppResuming()
{
	if (suspended != nullptr && Config->SessionTimeout != nullptr)
	{
		long long suspendedAgo = DateTimeHelper::Now().UniversalTime - suspended->Value.UniversalTime;
		if (suspendedAgo > Config->SessionTimeout->Value.Duration)
		{
			tracker->SetStartSession(true);
		}
	}
}

IAsyncAction^ EasyTracker::OnAppSuspending()
{
	suspended = DateTimeHelper::Now();
	return Dispatch();
}

void EasyTracker::OnUnhandledException(Exception^ ex, bool* handled)
{
	if (!reportingException)
	{
		if (handled)
		{
			tracker->SendException(ex->Message, false);
		}
		else
		{
			reportingException = true;
			*handled = true;
			tracker->SendException(ex->Message, true);
			create_task(Dispatch()).then([this, ex](task<void> t){
				reportingException = false;
				// rethrow exception now that we're done logging it. Note: stack trace will be replaced by current stack.
				throw ex;
			}, task_continuation_context::use_current());
		}
	}
}

EasyTracker::~EasyTracker()
{
	if (tracker != nullptr)
	{
		if (Config->AutoTrackNetworkConnectivity)
		{
			NetworkInformation::NetworkStatusChanged -= networkStatusChangedEventToken;
		}
	}
}