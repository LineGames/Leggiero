////////////////////////////////////////////////////////////////////////////////
// ${{ProgramName}}WinPCSetting.h (${{ProgramName}} - ${{ProgramName}}.WinPC)
//
// Implementation for WinPC Application Setting
////////////////////////////////////////////////////////////////////////////////


// My Header
#include "${{ProgramName}}WinPCSetting.h"


namespace ${{ProgramName}}
{
	//////////////////////////////////////////////////////////////////////////////// ${{ProgramName}}WinPCSetting

	//------------------------------------------------------------------------------
	${{ProgramName}}WinPCSetting::${{ProgramName}}WinPCSetting()
	{
		_InitializeDefaultValues();
	}

	//------------------------------------------------------------------------------
	${{ProgramName}}WinPCSetting::~${{ProgramName}}WinPCSetting()
	{

	}

	//------------------------------------------------------------------------------
	void ${{ProgramName}}WinPCSetting::ParseCommandLine(LPTSTR cmdLine)
	{
		//TODO
	}

	//------------------------------------------------------------------------------
	void ${{ProgramName}}WinPCSetting::_InitializeDefaultValues()
	{
#ifdef _DEBUG
		windowName = L"${{GameTitle}} - x64 (Debug)";
#else
		windowName = L"${{GameTitle}}";
#endif

${{Resolution-WinPC}}

		screenXPPI = 326.0f;
		screenYPPI = 326.0f;

		isTablet = false;

		localeString = "en_US";

		simulatedSafePaddingLeft = 0.0f;
		simulatedSafePaddingTop = 0.0f;
		simulatedSafePaddingRight = 0.0f;
		simulatedSafePaddingBottom = 0.0f;
	}
}
