////////////////////////////////////////////////////////////////////////////////
// Service/AndroidTextInputDialog.h (Leggiero - Platform.Android)
//
// Android Platform Text Input Dialog Service
////////////////////////////////////////////////////////////////////////////////

#ifndef __LP_ANDROID__SERVICE__ANDROID_TEXT_INPUT_DIALOG_H
#define __LP_ANDROID__SERVICE__ANDROID_TEXT_INPUT_DIALOG_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Utility
#include <Utility/Sugar/NonCopyable.h>

// Leggiero.Application
#include <Application/IAppTextInputController.h>


namespace Leggiero
{
	namespace Platform
	{
		namespace Android
		{
			// Text Input Dialog Service
			class AndroidTextInputDialog
				: public Application::IAppTextInputController
				, private Utility::SyntacticSugar::NonCopyable
			{
			public:
				AndroidTextInputDialog();
				virtual ~AndroidTextInputDialog();

			public:	// IAppTextInputController
				// Open Text Input Dialog
				virtual void OpenTextInputDialog(const char *dialogTitle, const char *dialogMessage, const GameDataString &initialText, const char *okButtonCaption, const char *cancelButtonCaption) override;

				// Check whether the text input dialog is open now
				virtual bool IsTextInputDialogOpen() override;

				// Get last value input to the text input dialog
				virtual GameDataString GetTextInputDialogText() override;

				// Check whether the dialog was closed as OK or Canceled
				virtual bool IsDialogClosedOK() override;
			};
		}
	}
}

#endif
