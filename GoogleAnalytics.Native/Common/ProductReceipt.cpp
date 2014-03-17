//
// ProductReceipt.cpp
// Implementation of the ProductReceipt class.
//

#include "pch.h"
#include "ProductReceipt.h"

using namespace GoogleAnalytics;
using namespace Platform;
using namespace Windows::Data::Xml::Dom;

ProductReceipt^ ProductReceipt::Load(String^ receipt)
{
	XmlDocument^ doc = ref new XmlDocument();
	doc->LoadXml(receipt);
	auto root = (XmlElement^)doc->SelectSingleNode("Receipt/ProductReceipt");
	if (root)
	{
		auto result = ref new ProductReceipt();
		result->Id = root->GetAttribute("Id");
		result->ProductId = root->GetAttribute("ProductId");
		result->ProductType = root->GetAttribute("ProductType");
		return result;
	}
	return nullptr;
}


