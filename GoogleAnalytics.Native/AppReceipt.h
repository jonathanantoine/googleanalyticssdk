//
// AppReceipt.h
// Declaration of the AppReceipt class.
//

#pragma once

namespace GoogleAnalytics
{
	public ref class AppReceipt sealed
	{
	public:

		property Platform::String^ Id;

		property Platform::String^ AppId;

		property Platform::String^ LicenseType;

		static GoogleAnalytics::AppReceipt^ Load(Platform::String^ receipt);

	};
}
