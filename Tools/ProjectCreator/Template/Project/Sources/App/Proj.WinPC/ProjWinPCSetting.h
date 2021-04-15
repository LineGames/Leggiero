////////////////////////////////////////////////////////////////////////////////
// ${{ProgramName}}WinPCSetting.h (${{ProgramName}} - ${{ProgramName}}.WinPC)
//
// WinPC Application Setting
////////////////////////////////////////////////////////////////////////////////

#pragma once


// Standard Library
#include <string>

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Platform.WinPC
#include <WindowsCommon.h>

// Leggiero.Application
#include <Engine/Application/IGameLaunchOption.h>


namespace ${{ProgramName}}
{
	// WinPC Application Setting
	class ${{ProgramName}}WinPCSetting
		: public Leggiero::IGameLaunchOption
	{
	public:
		${{ProgramName}}WinPCSetting();
		virtual ~${{ProgramName}}WinPCSetting();

	public:
		void ParseCommandLine(LPTSTR cmdLine);

	public:
		LPCTSTR windowName;

		int screenWidth;
		int screenHeight;

		float screenXPPI;
		float screenYPPI;

		bool isTablet;

		std::string localeString;
		std::string referrer;

	public:
		float simulatedSafePaddingLeft;
		float simulatedSafePaddingTop;
		float simulatedSafePaddingRight;
		float simulatedSafePaddingBottom;

	protected:
		void _InitializeDefaultValues();
	};
}
