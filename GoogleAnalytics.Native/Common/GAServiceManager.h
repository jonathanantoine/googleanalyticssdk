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
	public ref class PayloadFailedEventArgs sealed
	{
	private:

		Platform::String^ error;
		GoogleAnalytics::Payload^ payload;

	internal:

		PayloadFailedEventArgs(GoogleAnalytics::Payload^ payload, Platform::String^ error)
			: error(error)
			, payload(payload)
		{ }

	public:

		property Platform::String^ Error
		{
			Platform::String^ get()
			{
				return error;
			}
		}

		property GoogleAnalytics::Payload^ Payload
		{
			GoogleAnalytics::Payload^ get()
			{
				return payload;
			}
		}
	};

	public ref class PayloadSentEventArgs sealed
	{
	private:

		Platform::String^ response;
		GoogleAnalytics::Payload^ payload;

	internal:

		PayloadSentEventArgs(GoogleAnalytics::Payload^ payload, Platform::String^ response)
			: response(response)
			, payload(payload)
		{ }

	public:

		property Platform::String^ Reason
		{
			Platform::String^ get()
			{
				return response;
			}
		}

		property GoogleAnalytics::Payload^ Payload
		{
			GoogleAnalytics::Payload^ get()
			{
				return payload;
			}
		}
	};

	public ref class PayloadMalformedEventArgs sealed
	{
	private:

		int httpStatusCode;
		GoogleAnalytics::Payload^ payload;

	internal:

		PayloadMalformedEventArgs(GoogleAnalytics::Payload^ payload, int httpStatusCode)
			: httpStatusCode(httpStatusCode)
			, payload(payload)
		{ }

	public:

		property int HttpStatusCode
		{
			int get()
			{
				return httpStatusCode;
			}
		}

		property GoogleAnalytics::Payload^ Payload
		{
			GoogleAnalytics::Payload^ get()
			{
				return payload;
			}
		}
	};

	public ref class GAServiceManager sealed
	{
	private:

		std::mutex dispatcherLock;

		std::mutex payloadLock;

		static GoogleAnalytics::GAServiceManager^ current;

		static Windows::Foundation::Uri^ endPointUnsecureDebug;

		static Windows::Foundation::Uri^ endPointSecureDebug;

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

		concurrency::task<std::wstring> IssueRequestAsync(GoogleAnalytics::Payload^ payload, Web::HttpRequest httpRequest, std::unordered_map<Platform::String^, Platform::String^> payloadData);

		void OnPayloadFailed(GoogleAnalytics::Payload^ payload, Platform::String^ error);

		void OnPayloadSent(GoogleAnalytics::Payload^ payload, Platform::String^ response);

		void OnPayloadMalformed(GoogleAnalytics::Payload^ payload, int httpStatusCode);

		static Platform::String^ GetCacheBuster();

		static Platform::String^ userAgent;

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

		static property Platform::String^ UserAgent
		{
			Platform::String^ get()
			{
				if (!userAgent)
				{
					userAgent = PlatformInfoProvider::ConstructUserAgent();
				}
				return userAgent;
			}
			void set(Platform::String^ value)
			{
				userAgent = value;
			}
		}

		property bool PostData;

		event Windows::Foundation::EventHandler<GoogleAnalytics::PayloadFailedEventArgs^>^ PayloadFailed;

		event Windows::Foundation::EventHandler<GoogleAnalytics::PayloadSentEventArgs^>^ PayloadSent;

		event Windows::Foundation::EventHandler<GoogleAnalytics::PayloadMalformedEventArgs^>^ PayloadMalformed;
	};
}
