//
// GAServiceManager.cpp
// Implementation of the GAServiceManager class.
//

#include "pch.h"
#include "GAServiceManager.h"
#include "TimeSpanHelper.h"
#include "DateTimeHelper.h"

using namespace GoogleAnalytics;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::System::Threading;
using namespace concurrency;
using namespace Web;

GAServiceManager^ GAServiceManager::current = nullptr;

Uri^ GAServiceManager::endPointUnsecureDebug = ref new Uri("http://www.google-analytics.com/debug/collect");

Uri^ GAServiceManager::endPointSecureDebug = ref new Uri("https://ssl.google-analytics.com/debug/collect");

Uri^ GAServiceManager::endPointUnsecure = ref new Uri("http://www.google-analytics.com/collect");

Uri^ GAServiceManager::endPointSecure = ref new Uri("https://ssl.google-analytics.com/collect");

String^ GAServiceManager::userAgent = nullptr;

GAServiceManager::GAServiceManager() :
isConnected(true),
timer(nullptr)
{
	BustCache = false;
	dispatchPeriod = TimeSpanHelper::FromTicks(0);
}

void GAServiceManager::SendPayload(GoogleAnalytics::Payload^ payload)
{
	if (DispatchPeriod.Duration == 0 && IsConnected)
	{
		RunDispatchingTask(DispatchImmediatePayload(payload));
	}
	else
	{
		std::lock_guard<std::mutex> lg(payloadLock);
		payloads.push(payload);
	}
}

void GAServiceManager::Clear()
{
	std::lock_guard<std::mutex> lg(payloadLock);
	while (!payloads.empty()) payloads.pop();
}

void GAServiceManager::timer_Tick(ThreadPoolTimer^ sender)
{
	Dispatch();
}

task<void> GAServiceManager::_Dispatch()
{
	if (!isConnected) return task<void>([](){});

	task<void> allDispatchingTasks;
	{
		std::lock_guard<std::mutex> lg(dispatcherLock);
		allDispatchingTasks = when_all(begin(dispatchingTasks), end(dispatchingTasks));
	}

	return allDispatchingTasks.then([this]() {
		if (!isConnected) return task<void>([](){});

		std::vector<Payload^> payloadsToSend;
		{
			std::lock_guard<std::mutex> lg(payloadLock);
			while (!payloads.empty())
			{
				payloadsToSend.push_back(payloads.front());
				payloads.pop();
			}
		}
		if (!payloadsToSend.empty())
		{
			return RunDispatchingTask(DispatchQueuedPayloads(payloadsToSend));
		}
		else
		{
			return task<void>([](){});
		}
	});
}

task<void> GAServiceManager::RunDispatchingTask(task<void> newDispatchingTask)
{
	{
		std::lock_guard<std::mutex> lg(dispatcherLock);
		dispatchingTasks.push_back(newDispatchingTask);
	}
	return newDispatchingTask.then([this, newDispatchingTask](task<void> t){
		std::lock_guard<std::mutex> lg(dispatcherLock);
		dispatchingTasks.erase(begin(dispatchingTasks), std::find(begin(dispatchingTasks), end(dispatchingTasks), newDispatchingTask));
		t.get();
	});
}

task<void> GAServiceManager::DispatchQueuedPayloads(std::vector<Payload^> payloads)
{
	Web::HttpRequest httpRequest;
	httpRequest.headers.push_back(std::tuple<std::wstring, std::wstring>(L"User-Agent", UserAgent->Data()));
	auto now = DateTimeHelper::Now();
	std::vector<task<void>> tasks;
	for (auto it = begin(payloads); it != end(payloads); ++it)
	{
		Payload^ payload = *it;

		// clone the data
		std::unordered_map<String^, String^> payloadData;
		for each (auto kvp in payload->Data)
		{
			payloadData[kvp->Key] = kvp->Value;
		}

		payloadData["qt"] = TimeSpanHelper::GetTotalMilliseconds(TimeSpanHelper::FromTicks(now.UniversalTime - payload->TimeStamp.UniversalTime)).ToString();
		tasks.push_back(DispatchPayloadData(payload, httpRequest, payloadData));
	}
	return when_all(begin(tasks), end(tasks));
}

task<void> GAServiceManager::DispatchImmediatePayload(Payload^ payload)
{
	Web::HttpRequest httpRequest;
	httpRequest.headers.push_back(std::tuple<std::wstring, std::wstring>(L"User-Agent", UserAgent->Data()));
	// clone the data
	std::unordered_map<String^, String^> payloadData;
	for each (auto kvp in payload->Data)
	{
		payloadData[kvp->Key] = kvp->Value;
	}
	return DispatchPayloadData(payload, httpRequest, payloadData);
}

task<void> GAServiceManager::DispatchPayloadData(Payload^ payload, HttpRequest httpRequest, std::unordered_map<String^, String^> payloadData)
{
	if (isConnected)
	{
		if (BustCache) payloadData["z"] = GetCacheBuster();
		
		return IssueRequestAsync(payload, httpRequest, payloadData).then([this, payload](task<std::wstring> t) {
			try
			{
				std::wstring response(t.get());
				OnPayloadSent(payload, ref new String(response.c_str()));
			}
#if defined(__cplusplus_winrt)
			catch (Exception^ _E)
#else
			catch (const std::exception)
#endif
			{
				// TODO: get real error & determine http status code if not 200.
				OnPayloadFailed(payload, L"Unknown error");
			}
		}, task_continuation_context::use_current());
	}
	else
	{
		std::lock_guard<std::mutex> lg(payloadLock);
		payloads.push(payload);
		return task<void>([](){});
	}
}

task<std::wstring> GAServiceManager::IssueRequestAsync(Payload^ payload, HttpRequest httpRequest, std::unordered_map<String^, String^> payloadData)
{
	auto endPoint = payload->IsDebug ? (payload->IsUseSecure ? endPointSecureDebug : endPointUnsecureDebug) : (payload->IsUseSecure ? endPointSecure : endPointUnsecure);

	std::wstring content;
	auto it = begin(payloadData);
	while (true)
	{
		content += std::wstring(it->first->Data()) + L"=" + std::wstring(Uri::EscapeComponent(it->second)->Data());
		it++;
		if (it == end(payloadData)) break;
		content += '&';
	}

	if (PostData)
	{
		return httpRequest.PostAsync(endPoint, content);
	}
	else
	{
		auto url = endPoint->RawUri + L"?" + ref new String(content.c_str());
		return httpRequest.GetAsync(ref new Uri(url));
	}
}

void GAServiceManager::OnPayloadFailed(Payload^ payload, String^ error)
{
	// TODO: store in isolated storage and retry next session
	PayloadSent(this, ref new PayloadSentEventArgs(payload, error));
}

void GAServiceManager::OnPayloadSent(Payload^ payload, String^ response)
{
	PayloadSent(this, ref new PayloadSentEventArgs(payload, response));
}

void GAServiceManager::OnPayloadMalformed(GoogleAnalytics::Payload^ payload, int httpStatusCode)
{
	PayloadMalformed(this, ref new PayloadMalformedEventArgs(payload, httpStatusCode));
}

String^ GAServiceManager::GetCacheBuster()
{
	return (rand() % 100000000).ToString();
}

GAServiceManager^ GAServiceManager::Current::get()
{
	if (!current)
	{
		current = ref new GAServiceManager();
	}
	return current;
}

TimeSpan GAServiceManager::DispatchPeriod::get()
{
	return dispatchPeriod;
}
void GAServiceManager::DispatchPeriod::set(TimeSpan value)
{
	if (dispatchPeriod.Duration != value.Duration)
	{
		dispatchPeriod = value;
		if (timer)
		{
			timer->Cancel();
			timer = nullptr;
		}
		if (dispatchPeriod.Duration > 0)
		{
			timer = ThreadPoolTimer::CreatePeriodicTimer(ref new TimerElapsedHandler(this, &GAServiceManager::timer_Tick), dispatchPeriod);
		}
	}
}

bool GAServiceManager::IsConnected::get()
{
	return isConnected;
}
void GAServiceManager::IsConnected::set(bool value)
{
	if (isConnected != value)
	{
		isConnected = value;
		if (isConnected)
		{
			if (dispatchPeriod.Duration >= 0)
			{
				Dispatch();
			}
		}
	}
}

IAsyncAction^ GAServiceManager::Dispatch()
{
	return create_async([this]() { return _Dispatch(); });
}

