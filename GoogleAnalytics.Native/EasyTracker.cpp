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
	auto ga = AnalyticsEngine::Current;
	ga->IsDebugEnabled = Config->Debug;
	GAServiceManager::Current->DispatchPeriod = Config->DispatchPeriod;
	tracker = ga->GetTracker(Config->TrackingId);
	tracker->SetStartSession(Config->SessionTimeout != nullptr);
	tracker->AppName = Config->AppName;
	tracker->AppVersion = Config->AppVersion;
	tracker->IsAnonymizeIpEnabled = Config->AnonymizeIp;
	tracker->SampleRate = Config->SampleFrequency;
}

void EasyTracker::InitConfig(XmlDocument^ doc)
{
	Config = EasyTrackerConfig::Load(doc);
	Config->Validate();
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
	if (tracker == nullptr) throw ref new NullReferenceException("Context not set. Call EasyTracker.Current.SetContext(null) before getting tracker.");
	return tracker;
}

IAsyncAction^ EasyTracker::Dispatch()
{
	return GAServiceManager::Current->Dispatch();
}

EasyTracker::EasyTracker()
{
	ConfigPath = ref new Uri("ms-appx:///analytics.xml");
}

task<void> EasyTracker::InitConfig(Uri^ configPath)
{
	return create_task(StorageFile::GetFileFromApplicationUriAsync(configPath)).then([this](StorageFile^ file){
		return create_task(XmlDocument::LoadFromFileAsync(file)).then([this](XmlDocument^ doc){
			InitConfig(doc);
		});
	});
	//t.wait(); // this MUST be synchronous and we are only loading a local file installed with app so it should always be fast
}

void EasyTracker::PopulateMissingConfig()
{
	if (Config->AppName == nullptr || Config->AppName == "")
	{
		Config->AppName = Windows::ApplicationModel::Package::Current->Id->Name;
	}
	if (Config->AppVersion == nullptr || Config->AppVersion == "")
	{
		auto version = Windows::ApplicationModel::Package::Current->Id->Version;
		Config->AppVersion = version.Major + "." + version.Minor + "." + version.Build + "." + version.Revision;
	}
}

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

	if (Config->AutoAppLifetimeTracking)
	{
		tracker->SendEvent("app", "resume", nullptr, 0);
	}
}

IAsyncAction^ EasyTracker::OnAppSuspending()
{
	if (Config->AutoAppLifetimeTracking)
	{
		tracker->SendEvent("app", "suspend", nullptr, 0);
	}

	suspended = DateTimeHelper::Now();
	return Dispatch();
}

void EasyTracker::OnUnhandledException(Exception^ ex, bool* handled)
{
	if (Config->ReportUncaughtExceptions)
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
}

IAsyncAction^ EasyTracker::SetContext(Object^ ctx)
{
	return create_async([this, ctx]() { return _SetContext(ctx); } );
}

task<void> EasyTracker::_SetContext(Object^ ctx)
{
	isContextSet = true;
	UpdateConnectionStatus();
	networkStatusChangedEventToken = NetworkInformation::NetworkStatusChanged += ref new NetworkStatusChangedEventHandler(this, &EasyTracker::NetworkInformation_NetworkStatusChanged);
	task<void> initTask;
	if (Config == nullptr) 
	{
		initTask = InitConfig(ConfigPath);
	}
	else
	{
		initTask = task<void>([](){});
	}
	return initTask.then([this](){
		PopulateMissingConfig();
		InitTracker();
	});
}

EasyTracker::~EasyTracker()
{
	if (isContextSet)
	{
		NetworkInformation::NetworkStatusChanged -= networkStatusChangedEventToken;
		isContextSet = false;
	}
}