@pushd %~dp0%

@set DEVENV=%ProgramFiles(x86)%\Microsoft Visual Studio 11.0\Common7\IDE\devenv.exe
@IF NOT EXIST "%DEVENV%" SET DEVENV=%ProgramFiles(x86)%\Microsoft Visual Studio 11.0\Common7\IDE\VSWinExpress.exe
"%DEVENV%" /build "Release|AnyCPU" GoogleAnalytics.VS2012.sln

@set DEVENV=%ProgramFiles(x86)%\Microsoft Visual Studio 12.0\Common7\IDE\devenv.exe
@IF NOT EXIST "%DEVENV%" SET DEVENV=%ProgramFiles(x86)%\Microsoft Visual Studio 12.0\Common7\IDE\VSWinExpress.exe
"%DEVENV%" /build "Release|AnyCPU" GoogleAnalytics.VS2013.sln

@set DEVENV=%ProgramFiles(x86)%\Microsoft Visual Studio 14.0\Common7\IDE\devenv.exe
@IF NOT EXIST "%DEVENV%" SET DEVENV=%ProgramFiles(x86)%\Microsoft Visual Studio 14.0\Common7\IDE\VSWinExpress.exe
"%DEVENV%" /build "Release|AnyCPU" GoogleAnalytics.VS2015.sln

copy "Core\UWP\bin\Release\GoogleAnalytics.Core.winmd" 							"deploy\lib\uap10.0\"
copy "Core\UWP\bin\Release\GoogleAnalytics.Core.pdb" 							"deploy\lib\uap10.0\"
copy "GoogleAnalytics.Managed\UWP\bin\Release\GoogleAnalytics.winmd" 			"deploy\lib\uap10.0\"
copy "GoogleAnalytics.Managed\UWP\bin\Release\GoogleAnalytics.pdb" 				"deploy\lib\uap10.0\"

copy "Core\Universal\bin\Release\GoogleAnalytics.Core.winmd" 					"deploy\lib\windows81\"
copy "Core\Universal\bin\Release\GoogleAnalytics.Core.pdb" 						"deploy\lib\windows81\"
copy "GoogleAnalytics.Managed\Win81\bin\Release\GoogleAnalytics.winmd" 			"deploy\lib\windows81\"
copy "GoogleAnalytics.Managed\Win81\bin\Release\GoogleAnalytics.pdb" 			"deploy\lib\windows81\"

copy "Core\Universal\bin\Release\GoogleAnalytics.Core.winmd" 					"deploy\lib\wpa81\"
copy "Core\Universal\bin\Release\GoogleAnalytics.Core.pdb" 						"deploy\lib\wpa81\"
copy "GoogleAnalytics.Managed\WP81\bin\Release\GoogleAnalytics.winmd" 			"deploy\lib\wpa81\"
copy "GoogleAnalytics.Managed\WP81\bin\Release\GoogleAnalytics.pdb" 			"deploy\lib\wpa81\"

copy "Core\WinRT.Win80\bin\Release\GoogleAnalytics.Core.winmd" 					"deploy\lib\windows8\"
copy "Core\WinRT.Win80\bin\Release\GoogleAnalytics.Core.pdb" 					"deploy\lib\windows8\"
copy "GoogleAnalytics.Managed\Win80\bin\Release\GoogleAnalytics.winmd" 			"deploy\lib\windows8\"
copy "GoogleAnalytics.Managed\Win80\bin\Release\GoogleAnalytics.pdb" 			"deploy\lib\windows8\"

copy "Core\SL.WP80\bin\Release\GoogleAnalytics.Core.dll"    					"deploy\lib\windowsphone8\"
copy "Core\SL.WP80\bin\Release\GoogleAnalytics.Core.pdb"    					"deploy\lib\windowsphone8\"
copy "GoogleAnalytics.Managed\WP80\bin\Release\GoogleAnalytics.dll"    			"deploy\lib\windowsphone8\"
copy "GoogleAnalytics.Managed\WP80\bin\Release\GoogleAnalytics.pdb"    			"deploy\lib\windowsphone8\"

copy "Core\SL.WP71\bin\Release\GoogleAnalytics.Core.dll"    					"deploy\lib\wp71\"
copy "Core\SL.WP71\bin\Release\GoogleAnalytics.Core.pdb"    					"deploy\lib\wp71\"
copy "GoogleAnalytics.Managed\WP71\bin\Release\GoogleAnalytics.dll"    			"deploy\lib\wp71\"
copy "GoogleAnalytics.Managed\WP71\bin\Release\GoogleAnalytics.pdb"    			"deploy\lib\wp71\"

copy "Core\Universal\bin\Release\GoogleAnalytics.Core.winmd" 					"deploy\lib\portable-win81+wpa81\"
@rem copy "Core\Universal\bin\Release\GoogleAnalytics.Core.pdb" 						"deploy\lib\portable-win81+wpa81\"

copy "Core\PCL.136\bin\Release\GoogleAnalytics.Core.dll"    					"deploy\lib\portable-net40+win8+wp8+sl5\"
copy "Core\PCL.136\bin\Release\GoogleAnalytics.Core.pdb"    					"deploy\lib\portable-net40+win8+wp8+sl5\"

copy "Core\PCL.78\bin\Release\GoogleAnalytics.Core.dll"    						"deploy\lib\portable-net45+win8+wp8\"
copy "Core\PCL.78\bin\Release\GoogleAnalytics.Core.pdb"    						"deploy\lib\portable-net45+win8+wp8\"

copy "Core\PCL.104\bin\Release\GoogleAnalytics.Core.dll"    					"deploy\lib\portable-sl4+wp71+win8\"
copy "Core\PCL.104\bin\Release\GoogleAnalytics.Core.pdb"    					"deploy\lib\portable-sl4+wp71+win8\"

.nuget\nuget pack deploy\GoogleAnalyticsSDK.nuspec

@popd

@echo.
@pause