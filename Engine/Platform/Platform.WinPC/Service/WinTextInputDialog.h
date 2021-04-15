////////////////////////////////////////////////////////////////////////////////
// Service/WinTextInputDialog.h (Leggiero - Platform.WinPC)
//
// Windows Platform Text Input Dialog Service
////////////////////////////////////////////////////////////////////////////////

#pragma once


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Utility
#include <Utility/Sugar/NonCopyable.h>

// Leggiero.Application
#include <Application/IAppTextInputController.h>

// Leggiero.Platform.WinPC
#include "../WindowsCommon.h"


namespace Leggiero
{
	namespace Platform
	{
		namespace Windows
		{
			// Text Input Dialog Service
			class WinTextInputDialog
				: public Application::IAppTextInputController
				, private Utility::SyntacticSugar::NonCopyable
			{
			public:
				WinTextInputDialog(HINSTANCE hInstance, HWND hWndParent);
				virtual ~WinTextInputDialog();

			public:	// IAppTextInputController
				// Open Text Input Dialog
				virtual void OpenTextInputDialog(const char *dialogTitle, const char *dialogMessage, const GameDataString &initialText, const char *okButtonCaption, const char *cancelButtonCaption) override;

				// Check whether the text input dialog is open now
				virtual bool IsTextInputDialogOpen() override;

				// Get last value input to the text input dialog
				virtual GameDataString GetTextInputDialogText() override;

				// Check whether the dialog was closed as OK or Canceled
				virtual bool IsDialogClosedOK() override { return m_isOk; }

			protected:
				bool m_isOpen;

				GameDataString m_dialogTitle;
				GameDataString m_initialText;

				HINSTANCE	m_hInstance;
				HWND		m_hWndParent;

				HWND		m_hWnd;

				GameDataString m_textValue;
				bool m_isOk;

			protected:
				static LRESULT CALLBACK _InputDialogProcess(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
				
				void _OnInitDialog(HWND hWnd);
				void _OnOk();
				void _OnCancel();
			};
		}
	}
}
