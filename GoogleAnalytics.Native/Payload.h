//
// Payload.h
// Declaration of the Payload class.
//

#pragma once

#include "DateTimeHelper.h"

namespace GoogleAnalytics
{

	ref class Payload sealed
	{
	private:
		Windows::Foundation::DateTime timeStamp;

		Windows::Foundation::Collections::IMap<Platform::String^, Platform::String^>^ data;

	internal:

		Payload(Windows::Foundation::Collections::IMap<Platform::String^, Platform::String^>^ data)
		{
			this->data = data;
			this->timeStamp = DateTimeHelper::Now();
		}

		property Windows::Foundation::Collections::IMap<Platform::String^, Platform::String^>^ Data
		{
			Windows::Foundation::Collections::IMap<Platform::String^, Platform::String^>^ get()
			{
				return data;
			}
		}

		property Windows::Foundation::DateTime TimeStamp
		{
			Windows::Foundation::DateTime get()
			{
				return timeStamp;
			}
		}

		property bool IsUseSecure;

	};
}
