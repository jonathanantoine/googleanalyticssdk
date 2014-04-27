@pushd %~dp0%

@set DEVENV=%ProgramFiles(x86)%\Microsoft Visual Studio 11.0\Common7\IDE\devenv.exe
@IF NOT EXIST "%DEVENV%" SET DEVENV=%ProgramFiles(x86)%\Microsoft Visual Studio 11.0\Common7\IDE\VSWinExpress.exe
"%DEVENV%" /build "Release|AnyCPU" GoogleAnalytics.VS2012.sln

@set DEVENV=%ProgramFiles(x86)%\Microsoft Visual Studio 12.0\Common7\IDE\devenv.exe
@IF NOT EXIST "%DEVENV%" SET DEVENV=%ProgramFiles(x86)%\Microsoft Visual Studio 12.0\Common7\IDE\VSWinExpress.exe
"%DEVENV%" /build "Release|AnyCPU" GoogleAnalytics.VS2013.sln

copy "GoogleAnalytics.Managed\Win80\bin\Release\GoogleAnalytics.Core.winmd" 	"deploy\lib\windows8\"
copy "GoogleAnalytics.Managed\Win80\bin\Release\GoogleAnalytics.Core.pdb" 		"deploy\lib\windows8\"
copy "GoogleAnalytics.Managed\Win80\bin\Release\GoogleAnalytics.winmd" 			"deploy\lib\windows8\"
copy "GoogleAnalytics.Managed\Win80\bin\Release\GoogleAnalytics.pdb" 			"deploy\lib\windows8\"

copy "GoogleAnalytics.Managed\Win81\bin\Release\GoogleAnalytics.Core.winmd" 	"deploy\lib\windows81\"
copy "GoogleAnalytics.Managed\Win81\bin\Release\GoogleAnalytics.Core.pdb" 		"deploy\lib\windows81\"
copy "GoogleAnalytics.Managed\Win81\bin\Release\GoogleAnalytics.winmd" 			"deploy\lib\windows81\"
copy "GoogleAnalytics.Managed\Win81\bin\Release\GoogleAnalytics.pdb" 			"deploy\lib\windows81\"

copy "GoogleAnalytics.Managed\WP81\bin\Release\GoogleAnalytics.Core.winmd" 		"deploy\lib\wpa81\"
copy "GoogleAnalytics.Managed\WP81\bin\Release\GoogleAnalytics.Core.pdb" 		"deploy\lib\wpa81\"
copy "GoogleAnalytics.Managed\WP81\bin\Release\GoogleAnalytics.winmd" 			"deploy\lib\wpa81\"
copy "GoogleAnalytics.Managed\WP81\bin\Release\GoogleAnalytics.pdb" 			"deploy\lib\wpa81\"

copy "GoogleAnalytics.Managed\WP80\bin\Release\GoogleAnalytics.Core.dll"    	"deploy\lib\windowsphone8\"
copy "GoogleAnalytics.Managed\WP80\bin\Release\GoogleAnalytics.Core.pdb"    	"deploy\lib\windowsphone8\"
copy "GoogleAnalytics.Managed\WP80\bin\Release\GoogleAnalytics.dll"    			"deploy\lib\windowsphone8\"
copy "GoogleAnalytics.Managed\WP80\bin\Release\GoogleAnalytics.pdb"    			"deploy\lib\windowsphone8\"

copy "GoogleAnalytics.Managed\WP71\bin\Release\GoogleAnalytics.Core.dll"    	"deploy\lib\wp71\"
copy "GoogleAnalytics.Managed\WP71\bin\Release\GoogleAnalytics.Core.pdb"    	"deploy\lib\wp71\"
copy "GoogleAnalytics.Managed\WP71\bin\Release\GoogleAnalytics.dll"    			"deploy\lib\wp71\"
copy "GoogleAnalytics.Managed\WP71\bin\Release\GoogleAnalytics.pdb"    			"deploy\lib\wp71\"

copy "Core\PCL.136\bin\Release\GoogleAnalytics.Core.dll"    					"deploy\lib\portable-windows8+net45\"
copy "Core\PCL.136\bin\Release\GoogleAnalytics.Core.pdb"    					"deploy\lib\portable-windows8+net45\"

copy "Core\PCL.104\bin\Release\GoogleAnalytics.Core.dll"    					"deploy\lib\portable-sl4+wp71+windows8\"
copy "Core\PCL.104\bin\Release\GoogleAnalytics.Core.pdb"    					"deploy\lib\portable-sl4+wp71+windows8\"

.nuget\nuget pack deploy\GoogleAnalyticsSDK.nuspec

@popd

@echo.
@pause