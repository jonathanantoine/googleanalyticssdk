//
// PlatformInfoProvider.cpp
// Implementation of the PlatformInfoProvider class.
//

#include "pch.h"
#include "PlatformInfoProvider.h"
#include <string>
#include <collection.h>

using namespace GoogleAnalytics;
using namespace Platform;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Graphics::Display;

String^ PlatformInfoProvider::Key_AnonymousClientId = "GoogleAnaltyics.AnonymousClientId";

PlatformInfoProvider::PlatformInfoProvider() :
windowInitialized(false),
viewPortResolution(nullptr),
screenResolution(nullptr)
{
	InitializeWindow();
}

PlatformInfoProvider::~PlatformInfoProvider()
{
	UninitializeWindow();
}

void PlatformInfoProvider::UninitializeWindow()
{
	if (windowInitialized)
	{
		coreWindow->SizeChanged -= sizeChangedEventToken;
		coreWindow = nullptr;
		windowInitialized = false;
	}
}

void PlatformInfoProvider::InitializeWindow()
{
	try
	{
		coreWindow = Windows::UI::Core::CoreWindow::GetForCurrentThread();
		if (coreWindow.Get())
		{
			auto bounds = coreWindow->Bounds;

			float w = bounds.Width;
			float h = bounds.Height;
			auto displayInfo = DisplayInformation::GetForCurrentView();
			switch (displayInfo->ResolutionScale)
			{
			case ResolutionScale::Scale120Percent:
				w = std::floorf(.5f + w * 1.2f);
				w = std::floorf(.5f + w * 1.2f);
				break;
			case ResolutionScale::Scale140Percent:
				w = std::floorf(.5f + w * 1.4f);
				w = std::floorf(.5f + w * 1.4f);
				break;
			case ResolutionScale::Scale150Percent:
				w = std::floorf(.5f + w * 1.5f);
				w = std::floorf(.5f + w * 1.5f);
				break;
			case ResolutionScale::Scale160Percent:
				w = std::floorf(.5f + w * 1.6f);
				w = std::floorf(.5f + w * 1.6f);
				break;
			case ResolutionScale::Scale180Percent:
				w = std::floorf(.5f + w * 1.8f);
				w = std::floorf(.5f + w * 1.8f);
				break;
			case ResolutionScale::Scale225Percent:
				w = std::floorf(.5f + w * 2.25f);
				w = std::floorf(.5f + w * 2.25f);
				break;
			}

			if ((displayInfo->CurrentOrientation & DisplayOrientations::Landscape) == DisplayOrientations::Landscape)
			{
				SetScreenResolution(Size(w, h));
			}
			else // portrait
			{
				SetScreenResolution(Size(h, w));
			}
			SetViewPortResolution(Size(bounds.Width, bounds.Height)); // leave viewport at the scale unadjusted size
			sizeChangedEventToken = coreWindow->SizeChanged += ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &PlatformInfoProvider::Window_SizeChanged);
			windowInitialized = true;
		}
	}
	catch (const std::exception) { /* ignore, CoreWindow may not be ready yet */ }
}

void PlatformInfoProvider::Window_SizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ e)
{
	SetViewPortResolution(e->Size);
}

void PlatformInfoProvider::OnTracking()
{
	if (!windowInitialized)
	{
		InitializeWindow();
	}
}

String^ PlatformInfoProvider::AnonymousClientId::get()
{
	if (anonymousClientId) return anonymousClientId;
	auto appSettings = ApplicationData::Current->LocalSettings;
	if (!appSettings->Values->HasKey(Key_AnonymousClientId))
	{
		GUID guid;
		CoCreateGuid(&guid);
		std::wstring str(Guid(guid).ToString()->Data());
		auto result = ref new String(str.substr(1, str.length() - 2).data());
		appSettings->Values->Insert(Key_AnonymousClientId, result);
		return result;
	}
	else
	{
		return (String^)appSettings->Values->Lookup(Key_AnonymousClientId);
	}
}
void PlatformInfoProvider::AnonymousClientId::set(String^ value)
{
	anonymousClientId = value;
}

IBox<Size>^ PlatformInfoProvider::ViewPortResolution::get()
{
	return viewPortResolution;
}

void PlatformInfoProvider::SetViewPortResolution(IBox<Size>^ value)
{
	if (viewPortResolution != value)
	{
		viewPortResolution = value;
		ViewPortResolutionChanged(this, nullptr);
	}
}

IBox<Size>^ PlatformInfoProvider::ScreenResolution::get()
{
	return screenResolution;
}

void PlatformInfoProvider::SetScreenResolution(IBox<Size>^ value)
{
	if (screenResolution != value)
	{
		screenResolution = value;
		ScreenResolutionChanged(this, nullptr);
	}
}

String^ PlatformInfoProvider::UserLanguage::get()
{
	return Windows::Globalization::ApplicationLanguages::Languages->GetAt(0);
}

IBox<int>^ PlatformInfoProvider::ScreenColorDepthBits::get()
{
	return nullptr;
}

String^ PlatformInfoProvider::ConstructUserAgent()
{
	auto sysInfo = ref new Windows::Security::ExchangeActiveSyncProvisioning::EasClientDeviceInformation();
	auto tc = ref new Windows::Devices::Input::TouchCapabilities();
	bool hasTouch = tc->TouchPresent > 0;
	return "Mozilla/5.0 (compatible; MSIE 11.0; Windows NT 6.3; Trident/7.0" + (hasTouch ? "; Touch" : "") + "; " + sysInfo->SystemManufacturer + "; " + sysInfo->SystemProductName + ")";
}