////////////////////////////////////////////////////////////////////////////////
// WinPCMain.cpp (${{ProgramName}} - ${{ProgramName}}.WinPC)
//
// Program Entry Point of Windows PC Game Application
////////////////////////////////////////////////////////////////////////////////

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Platform.WinPC
#include <WindowsCommon.h>
#include <WinPCPlatformApplication.h>

// ${{ProgramName}}.WinPC
#include "resource.h"
#include "${{ProgramName}}WinPCSetting.h"


//------------------------------------------------------------------------------
// Program Entry

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);

	// Load Setting
	${{ProgramName}}::${{ProgramName}}WinPCSetting appSetting;
	appSetting.ParseCommandLine(lpCmdLine);

	// Create Application
	Leggiero::Platform::Windows::WinPCPlatformApplication application(
		hInstance, 
		nCmdShow, 
		L"${{ProgramName}}", 
		appSetting.windowName,
		LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON)),
		(HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR),
		appSetting.screenWidth, appSetting.screenHeight, 
		appSetting.screenXPPI, appSetting.screenYPPI
	);

	// Set Metadata
	application.SetIsTablet(appSetting.isTablet);
	application.SetSystemLocale(appSetting.localeString);
	application.SetReferrer(appSetting.referrer);
	application.SetSafePadding(appSetting.simulatedSafePaddingLeft, appSetting.simulatedSafePaddingTop, appSetting.simulatedSafePaddingRight, appSetting.simulatedSafePaddingBottom);

	// Run Game (Game Loop)
	application.Run(&appSetting);

	return 0;
}
