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
	if (root != nullptr)
	{
		for (unsigned int i = 0; i < root->ChildNodes->Size; i++)
		{
			auto node = root->ChildNodes->GetAt(i);
			XmlElement^ element = dynamic_cast<XmlElement^>(node);
			if (element != nullptr)
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
				else if (element->NodeName == "debug")
					result->Debug = element->InnerText == "true";
				else if (element->NodeName == "autoActivityTracking")
					result->AutoActivityTracking = element->InnerText == "true";
				else if (element->NodeName == "autoAppLifetimeTracking")
					result->AutoAppLifetimeTracking = element->InnerText == "true";
				else if (element->NodeName == "autoAppLifetimeMonitoring")
					result->AutoAppLifetimeMonitoring = element->InnerText == "true";
				else if (element->NodeName == "anonymizeIp")
					result->AnonymizeIp = element->InnerText == "true";
				else if (element->NodeName == "reportUncaughtExceptions")
					result->ReportUncaughtExceptions = element->InnerText == "true";
			}
		}
	}
	return result;
}

void EasyTrackerConfig::Validate()
{
	if (AutoAppLifetimeTracking && !AutoAppLifetimeMonitoring)
	{
		throw ref new InvalidArgumentException("AutoAppLifetimeTracking cannot be true if AutoAppLifetimeMonitoring is false.");
	}
}

EasyTrackerConfig::EasyTrackerConfig()
{
	SessionTimeout = TimeSpanHelper::FromSeconds(30);
	DispatchPeriod = TimeSpan();
	SampleFrequency = 100.0F;
	AutoAppLifetimeMonitoring = true;
}


