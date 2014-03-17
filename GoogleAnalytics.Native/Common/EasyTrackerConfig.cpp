//
// EasyTrackerConfig.cpp
// Implementation of the EasyTrackerConfig class.
//

#include "pch.h"
#include "EasyTrackerConfig.h"
#include <string>
#include "TimeSpanHelper.h"

using namespace GoogleAnalytics;
using namespace Platform;
using namespace Windows::Data::Xml::Dom;
using namespace Windows::Foundation;

String^ EasyTrackerConfig::ns = "xmlns:ga='http://googleanalyticssdk.codeplex.com/ns/easytracker'";

EasyTrackerConfig^ EasyTrackerConfig::Load(XmlDocument^ doc)
{
	auto result = ref new EasyTrackerConfig();
	auto root = doc->SelectSingleNodeNS("ga:analytics", ns);
	if (root)
	{
		for (unsigned int i = 0; i < root->ChildNodes->Size; i++)
		{
			auto node = root->ChildNodes->GetAt(i);
			XmlElement^ element = dynamic_cast<XmlElement^>(node);
			if (element)
			{
				if (element->NodeName == "trackingId") 
					result->TrackingId = element->InnerText;
				else if (element->NodeName == "appName")
					result->AppName = element->InnerText;
				else if (element->NodeName == "appVersion")
					result->AppVersion = element->InnerText;
				else if (element->NodeName == "sampleFrequency")
					result->SampleFrequency = std::stof(element->InnerText->Data(), nullptr);
				else if (element->NodeName == "dispatchPeriod")
				{
					int dispatchPeriodInSeconds = std::stoi(element->InnerText->Data(), nullptr);
					result->DispatchPeriod = TimeSpanHelper::FromSeconds(dispatchPeriodInSeconds);
				}
				else if (element->NodeName == "sessionTimeout")
				{
					int sessionTimeoutInSeconds = std::stoi(element->InnerText->Data(), nullptr);
					if (sessionTimeoutInSeconds >= 0) result->SessionTimeout = TimeSpanHelper::FromSeconds(sessionTimeoutInSeconds);
				}
				else if (element->NodeName == "anonymizeIp")
					result->AnonymizeIp = element->InnerText == "true";				
				else if (element->NodeName == "autoTrackNetworkConnectivity")
					result->AutoTrackNetworkConnectivity = element->InnerText == "true";
				else if (element->NodeName == "useSecure")
					result->UseSecure = element->InnerText == "true";
			}
		}
	}
	return result;
}

EasyTrackerConfig::EasyTrackerConfig()
{
	TrackingId = nullptr;
	SessionTimeout = TimeSpanHelper::FromSeconds(30);
	DispatchPeriod = TimeSpanHelper::FromTicks(0);
	SampleFrequency = 100.0F;
	AutoTrackNetworkConnectivity = true;
	AnonymizeIp = false;
	UseSecure = false;
	AppName = Windows::ApplicationModel::Package::Current->Id->Name;
	auto version = Windows::ApplicationModel::Package::Current->Id->Version;
	AppVersion = version.Major + "." + version.Minor + "." + version.Build + "." + version.Revision;
}


