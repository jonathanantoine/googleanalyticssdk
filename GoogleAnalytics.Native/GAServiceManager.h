//
// GAServiceManager.h
// Declaration of the GAServiceManager class.
//

#pragma once

#include <ppltasks.h>
#include <queue>
#include <collection.h>
#include <unordered_map>
#include <mutex>
#include "Payload.h"
#include "HttpRequest.h"

namespace GoogleAnalytics
{
	public ref class GAServiceManager sealed
	{
	private:
		
		std::mutex dispatcherLock;
		
		std::mutex payloadLock;

		static GoogleAnalytics::GAServiceManager^ current;
		
		static Windows::Foundation::Uri^ endPointUnsecure;
		
		static Windows::Foundation::Uri^ endPointSecure;
		
		std::queue<GoogleAnalytics::Payload^> payloads;
		
		std::vector<concurrency::task<void>> dispatchingTasks;
		
		Windows::System::Threading::ThreadPoolTimer^ timer;
		
		GAServiceManager();
		
		void timer_Tick(Windows::System::Threading::ThreadPoolTimer^ sender);
		
		Windows::Foundation::TimeSpan dispatchPeriod;
		
		bool isConnected;
		
		concurrency::task<void> _Dispatch();
		
		concurrency::task<void> RunDispatchingTask(concurrency::task<void> newDispatchingTask);
		
		concurrency::task<void> DispatchQueuedPayloads(std::vector<Payload^> payloads);
		
		concurrency::task<void> DispatchImmediatePayload(GoogleAnalytics::Payload^ payload);
		
		concurrency::task<void> DispatchPayloadData(GoogleAnalytics::Payload^ payload, Web::HttpRequest httpRequest, std::unordered_map<Platform::String^, Platform::String^> payloadData);
		
		void OnPayloadFailed(GoogleAnalytics::Payload^ payload);
		
		static Platform::String^ ConstructUserAgent();
		
		static Platform::String^ GetCacheBuster();
		
	internal: 
		
		void Clear();

		void SendPayload(GoogleAnalytics::Payload^ payload);

	public:

		property bool BustCache;
		
		static property GoogleAnalytics::GAServiceManager^ Current
		{
			GoogleAnalytics::GAServiceManager^ get();
		}
		
		property Windows::Foundation::TimeSpan DispatchPeriod
		{
			Windows::Foundation::TimeSpan get();
			void set(Windows::Foundation::TimeSpan value);
		}
		
		property bool IsConnected
		{
			bool get();
			void set(bool value);
		}
		
		Windows::Foundation::IAsyncAction^ Dispatch();
		
		static property Platform::String^ UserAgent;
		
	};
}
