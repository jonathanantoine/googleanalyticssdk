//
// HttpHelper.cpp
// Implementation of the HttpHelper class.
//

#include "pch.h"
#include "HttpHelper.h"

using namespace GoogleAnalytics;
using namespace Platform;
using namespace Windows::Foundation;

String^ HttpHelper::UrlEncode(String^ string)
{
	return ref new String(UrlEncode(std::wstring(string->Data())).data());
}


std::wstring HttpHelper::UrlEncode(const std::wstring source)
{
	const char hex[] = "0123456789ABCDEF";
	std::wstring result;

	for (auto it = begin(source); it != end(source); ++it)
	{
		auto c = *it;
		if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '$' || c == '-' || c == '_' || c == '.' || c == '+' || c == '!' || c == '*' || c == '\'' || c == '('  || c == ')'  || c == ',') 
			result += c;
		else if (c == ' ')
			result += '+';
		else
		{
			// escape this char
			result += '%';
			result += hex[c >> 4];
			result += hex[c & 0x0F];
		}
	}
	return result;
}