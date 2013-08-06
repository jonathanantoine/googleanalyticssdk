@pushd %~dp0%

@set DEVENV=%ProgramFiles(x86)%\Microsoft Visual Studio 11.0\Common7\IDE\devenv.exe
@IF NOT EXIST "%DEVENV%" SET DEVENV=%ProgramFiles(x86)%\Microsoft Visual Studio 11.0\Common7\IDE\VSWinExpress.exe
@set ZIP=%ProgramFiles%\7-Zip\7z.exe

"%DEVENV%" /build "Release|ARM" GoogleAnalytics.sln
"%DEVENV%" /build "Release|Win32" GoogleAnalytics.sln
"%DEVENV%" /build "Release|x64" GoogleAnalytics.sln

cd deploy.native

rmdir /s /q "Redist\"
rmdir /s /q "Reference\"

mkdir "Redist"
mkdir "Redist\CommonConfiguration"
mkdir "Redist\CommonConfiguration\neutral"
mkdir "Redist\CommonConfiguration\ARM"
mkdir "Redist\CommonConfiguration\x64"
mkdir "Redist\CommonConfiguration\x86"
mkdir "References"
mkdir "References\CommonConfiguration"
mkdir "References\CommonConfiguration\neutral"

copy "..\Release\GoogleAnalytics.Native\GoogleAnalytics.winmd"			"References\CommonConfiguration\neutral\"
copy "..\Release\GoogleAnalytics.Native\GoogleAnalytics.pri"			"Redist\CommonConfiguration\neutral\"
copy "..\ARM\Release\GoogleAnalytics.Native\GoogleAnalytics.Native.dll"	"Redist\CommonConfiguration\ARM\"
copy "..\x64\Release\GoogleAnalytics.Native\GoogleAnalytics.Native.dll"	"Redist\CommonConfiguration\x64\"
copy "..\Release\GoogleAnalytics.Native\GoogleAnalytics.Native.dll"		"Redist\CommonConfiguration\x86\"

"%ZIP%" a ..\GoogleAnalyticsSDK.zip "*"

cd ..\
move GoogleAnalyticsSDK.zip GoogleAnalyticsSDK.vsix

@popd

@echo.
@pause