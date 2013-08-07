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

String^ PlatformInfoProvider::Key_AnonymousClientId = "GoogleAnaltyics.AnonymousClientId";

PlatformInfoProvider::PlatformInfoProvider()
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
		auto bounds = coreWindow->Bounds;
		if (ApplicationView::Value == ApplicationViewState::FullScreenLandscape)
		{
			SetScreenResolution(Size(bounds.Width, bounds.Height));
		}
		else if (ApplicationView::Value == ApplicationViewState::FullScreenPortrait)
		{
			SetScreenResolution(Size(bounds.Height, bounds.Width));
		}
		SetViewPortResolution(Size(bounds.Width, bounds.Height));
		sizeChangedEventToken = coreWindow->SizeChanged += ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &PlatformInfoProvider::Window_SizeChanged);
		windowInitialized = true;
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
	// TEST
	return Windows::Globalization::ApplicationLanguages::Languages->GetAt(0);
}

IBox<int>^ PlatformInfoProvider::ScreenColorDepthBits::get()
{
	return nullptr;
}

String^ PlatformInfoProvider::DocumentEncoding::get()
{
	return nullptr;
}


