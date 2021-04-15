////////////////////////////////////////////////////////////////////////////////
// Service/iOSTextInputDialog.h (Leggiero - Platform.iOS)
//
// iOS Platform Text Input Dialog Service
////////////////////////////////////////////////////////////////////////////////

#ifndef __LP_IOS__SERVICE__IOS_TEXT_INPUT_DIALOG_H
#define __LP_IOS__SERVICE__IOS_TEXT_INPUT_DIALOG_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// System Library
#import <Foundation/Foundation.h>

// Leggiero.Utility
#include <Utility/Sugar/NonCopyable.h>

// Leggiero.Application
#include <Application/IAppTextInputController.h>


namespace Leggiero
{
	namespace Platform
	{
		namespace iOS
		{
			// Text Input Dialog Service
			class IOSTextInputDialog
				: public Application::IAppTextInputController
				, private Leggiero::Utility::SyntacticSugar::NonCopyable
			{
			public:
                IOSTextInputDialog();
				virtual ~IOSTextInputDialog();

			public:	// IAppTextInputController
				// Open Text Input Dialog
				virtual void OpenTextInputDialog(const char *dialogTitle, const char *dialogMessage, const GameDataString &initialText, const char *okButtonCaption, const char *cancelButtonCaption) override;

				// Check whether the text input dialog is open now
				virtual bool IsTextInputDialogOpen() override;

				// Get last value input to the text input dialog
				virtual GameDataString GetTextInputDialogText() override;

				// Check whether the dialog was closed as OK or Canceled
				virtual bool IsDialogClosedOK() override;
                
            private:
                id m_alertDialog;
			};
		}
	}
}

#endif
