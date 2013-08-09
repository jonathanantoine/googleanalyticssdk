//
// PlatformInfoProvider.h
// Declaration of the PlatformInfoProvider class.
//

#pragma once

#include <agile.h>

namespace GoogleAnalytics
{

	public ref class PlatformInfoProvider sealed
	{
	private:
		
		static float snappedModeSize;

		Platform::Agile<Windows::UI::Core::CoreWindow^> coreWindow;

		Windows::Foundation::EventRegistrationToken sizeChangedEventToken;

		static Platform::String^ Key_AnonymousClientId;
		
		bool windowInitialized;
		
		void InitializeWindow();

		void UninitializeWindow();
		
		void Window_SizeChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ e);
		
		Platform::IBox<Windows::Foundation::Size>^ viewPortResolution;
		
		Platform::IBox<Windows::Foundation::Size>^ screenResolution;
				
		void SetViewPortResolution(Platform::IBox<Windows::Foundation::Size>^ value);

		void SetScreenResolution(Platform::IBox<Windows::Foundation::Size>^ value);
		
	public:
		
		PlatformInfoProvider();

		virtual ~PlatformInfoProvider();

		event Windows::Foundation::EventHandler<Platform::Object^>^ ViewPortResolutionChanged;
		
		event Windows::Foundation::EventHandler<Platform::Object^>^ ScreenResolutionChanged;
		
		void OnTracking();
		
		property Platform::String^ AnonymousClientId
		{
			Platform::String^ get();
		}
		
		property Platform::IBox<Windows::Foundation::Size>^ ViewPortResolution
		{
			Platform::IBox<Windows::Foundation::Size>^ get();
		}

		property Platform::IBox<Windows::Foundation::Size>^ ScreenResolution
		{
			Platform::IBox<Windows::Foundation::Size>^ get();
		}

		property Platform::String^ UserLanguage
		{
			Platform::String^ get();
		}

		property Platform::IBox<int>^ ScreenColorDepthBits
		{
			Platform::IBox<int>^ get();
		}

		property Platform::String^ DocumentEncoding
		{
			Platform::String^ get();
		}

		static property float SnappedModeSize
		{
			float get()
			{
				return snappedModeSize;
			}
			void set(float value)
			{
				snappedModeSize = value;
			}
		}

	};
}
