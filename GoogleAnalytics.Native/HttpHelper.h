//
// HttpHelper.h
// Declaration of the HttpHelper class.
//

#pragma once

#include <string>

namespace GoogleAnalytics
	{
		ref class HttpHelper sealed
		{

		internal:

			static Platform::String^ UrlEncode(Platform::String^ string);
			
			static std::wstring UrlEncode(const std::wstring source);

		};
}
