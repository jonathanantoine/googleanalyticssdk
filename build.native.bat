@pushd %~dp0%

@set DEVENV=%ProgramFiles(x86)%\Microsoft Visual Studio 12.0\Common7\IDE\devenv.exe
@IF NOT EXIST "%DEVENV%" SET DEVENV=%ProgramFiles(x86)%\Microsoft Visual Studio 12.0\Common7\IDE\VSWinExpress.exe
@set ZIP=%ProgramFiles%\7-Zip\7z.exe

"%DEVENV%" /build "Release|ARM" GoogleAnalytics.VS2013.sln
"%DEVENV%" /build "Release|Win32" GoogleAnalytics.VS2013.sln
"%DEVENV%" /build "Release|x64" GoogleAnalytics.VS2013.sln

cd deploy.native
cd win80

rmdir /s /q "Redist\"
rmdir /s /q "References\"

mkdir "Redist"
mkdir "Redist\CommonConfiguration"
mkdir "Redist\CommonConfiguration\neutral"
mkdir "Redist\CommonConfiguration\ARM"
mkdir "Redist\CommonConfiguration\x64"
mkdir "Redist\CommonConfiguration\x86"
mkdir "References"
mkdir "References\CommonConfiguration"
mkdir "References\CommonConfiguration\neutral"

copy "..\..\Release\GoogleAnalytics.Native.Win80\GoogleAnalytics.winmd"				"References\CommonConfiguration\neutral\"
copy "..\..\Release\GoogleAnalytics.Native.Win80\GoogleAnalytics.pri"				"Redist\CommonConfiguration\neutral\"
copy "..\..\ARM\Release\GoogleAnalytics.Native.Win80\GoogleAnalytics.Native.dll"	"Redist\CommonConfiguration\ARM\"
copy "..\..\x64\Release\GoogleAnalytics.Native.Win80\GoogleAnalytics.Native.dll"	"Redist\CommonConfiguration\x64\"
copy "..\..\Release\GoogleAnalytics.Native.Win80\GoogleAnalytics.Native.dll"		"Redist\CommonConfiguration\x86\"

"%ZIP%" a ..\GoogleAnalyticsSDK.Win80.zip "*"

cd ..\
cd win81

rmdir /s /q "Redist\"
rmdir /s /q "References\"

mkdir "Redist"
mkdir "Redist\CommonConfiguration"
mkdir "Redist\CommonConfiguration\neutral"
mkdir "Redist\CommonConfiguration\ARM"
mkdir "Redist\CommonConfiguration\x64"
mkdir "Redist\CommonConfiguration\x86"
mkdir "References"
mkdir "References\CommonConfiguration"
mkdir "References\CommonConfiguration\neutral"

copy "..\..\Release\GoogleAnalytics.Native.Win81\GoogleAnalytics.winmd"				"References\CommonConfiguration\neutral\"
copy "..\..\Release\GoogleAnalytics.Native.Win81\GoogleAnalytics.pri"				"Redist\CommonConfiguration\neutral\"
copy "..\..\ARM\Release\GoogleAnalytics.Native.Win81\GoogleAnalytics.Native.dll"	"Redist\CommonConfiguration\ARM\"
copy "..\..\x64\Release\GoogleAnalytics.Native.Win81\GoogleAnalytics.Native.dll"	"Redist\CommonConfiguration\x64\"
copy "..\..\Release\GoogleAnalytics.Native.Win81\GoogleAnalytics.Native.dll"		"Redist\CommonConfiguration\x86\"

"%ZIP%" a ..\GoogleAnalyticsSDK.Win81.zip "*"

cd ..\
cd wp81

rmdir /s /q "Redist\"
rmdir /s /q "References\"

mkdir "Redist"
mkdir "Redist\CommonConfiguration"
mkdir "Redist\CommonConfiguration\neutral"
mkdir "Redist\CommonConfiguration\ARM"
mkdir "Redist\CommonConfiguration\x86"
mkdir "References"
mkdir "References\CommonConfiguration"
mkdir "References\CommonConfiguration\neutral"

copy "..\..\Release\GoogleAnalytics.Native.WP81\GoogleAnalytics.winmd"			"References\CommonConfiguration\neutral\"
copy "..\..\Release\GoogleAnalytics.Native.WP81\GoogleAnalytics.pri"			"Redist\CommonConfiguration\neutral\"
copy "..\..\ARM\Release\GoogleAnalytics.Native.WP81\GoogleAnalytics.Native.dll"	"Redist\CommonConfiguration\ARM\"
copy "..\..\Release\GoogleAnalytics.Native.WP81\GoogleAnalytics.Native.dll"		"Redist\CommonConfiguration\x86\"

"%ZIP%" a ..\GoogleAnalyticsSDK.WP81.zip "*"

cd ..\
move GoogleAnalyticsSDK.Win80.zip Master\GoogleAnalyticsSDK.Win80.vsix
move GoogleAnalyticsSDK.Win81.zip Master\GoogleAnalyticsSDK.Win81.vsix
move GoogleAnalyticsSDK.WP81.zip Master\GoogleAnalyticsSDK.WP81.vsix

cd Master
"%ZIP%" a ..\GoogleAnalyticsSDK.zip "*"
cd ..\
move GoogleAnalyticsSDK.zip ..\GoogleAnalyticsSDK.vsix

@popd

@echo.
@pause