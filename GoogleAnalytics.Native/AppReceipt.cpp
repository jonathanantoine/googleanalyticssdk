//
// AppReceipt.cpp
// Implementation of the AppReceipt class.
//

#include "pch.h"
#include "AppReceipt.h"

using namespace GoogleAnalytics;
using namespace Platform;
using namespace Windows::Data::Xml::Dom;

AppReceipt^ AppReceipt::Load(String^ receipt)
{
	XmlDocument^ doc = ref new XmlDocument();
	doc->LoadXml(receipt);

	auto root = (XmlElement^)doc->SelectSingleNode("Receipt/AppReceipt");
	if (root)
	{
		auto result = ref new AppReceipt();
		result->Id = root->GetAttribute("Id");
		result->AppId = root->GetAttribute("AppId");
		result->LicenseType = root->GetAttribute("LicenseType");
		return result;
	}

	return nullptr;
}


