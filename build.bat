@pushd %~dp0%

@set DEVENV=%ProgramFiles(x86)%\Microsoft Visual Studio 11.0\Common7\IDE\devenv.exe
@IF NOT EXIST "%DEVENV%" SET DEVENV=%ProgramFiles(x86)%\Microsoft Visual Studio 11.0\Common7\IDE\VSWinExpress.exe

"%DEVENV%" /build "Release|AnyCPU" GoogleAnalytics.sln

copy "GoogleAnalytics.Managed.Win80\bin\Release\GoogleAnalytics.Portable.dll" 	"deploy\lib\windows8\"
copy "GoogleAnalytics.Managed.Win80\bin\Release\GoogleAnalytics.dll" 			"deploy\lib\windows8\"

copy "GoogleAnalytics.SL.WP80\bin\Release\GoogleAnalytics.Portable.dll"    		"deploy\lib\windowsphone8\"
copy "GoogleAnalytics.SL.WP80\bin\Release\GoogleAnalytics.dll"    				"deploy\lib\windowsphone8\"

copy "GoogleAnalytics.SL.WP71\bin\Release\GoogleAnalytics.Portable.dll"    		"deploy\lib\wp71\"
copy "GoogleAnalytics.SL.WP71\bin\Release\GoogleAnalytics.dll"    				"deploy\lib\wp71\"

.nuget\nuget pack deploy\GoogleAnalyticsSDK.nuspec

@popd

@echo.
@pause