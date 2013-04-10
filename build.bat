@pushd %~dp0%

@set DEVENV=%ProgramFiles(x86)%\Microsoft Visual Studio 11.0\Common7\IDE\devenv.exe
@IF NOT EXIST "%DEVENV%" SET DEVENV=%ProgramFiles(x86)%\Microsoft Visual Studio 11.0\Common7\IDE\VSWinExpress.exe

"%DEVENV%" /build "Release|AnyCPU" GoogleAnalytics.sln

copy "GoogleAnalytics.Win8\bin\Release\GoogleAnalytics.winmd" "deploy\lib\windows8\"
copy "GoogleAnalytics.WP8\bin\Release\GoogleAnalytics.dll"    "deploy\lib\windowsphone8\"
copy "GoogleAnalytics.WP7\bin\Release\GoogleAnalytics.dll"    "deploy\lib\wp71\"

.nuget\nuget pack deploy\GoogleAnalyticsSDK.nuspec

@popd

@echo.
@pause