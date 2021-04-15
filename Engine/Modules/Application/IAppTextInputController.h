////////////////////////////////////////////////////////////////////////////////
// IAppTextInputController.h (Leggiero/Modules - Application)
//
// Interface for Text Input Service Controller Sub-Component
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_APPLICATION__I_APP_TEXT_INPUT_CONTROLLER_H
#define __LM_APPLICATION__I_APP_TEXT_INPUT_CONTROLLER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>


namespace Leggiero
{
	namespace Application
	{
		// Text Input Service Controller Interface
		class IAppTextInputController
		{
		public:
			// Open Text Input Dialog
			virtual void OpenTextInputDialog(const char *dialogTitle, const char *dialogMessage, const GameDataString &initialText, const char *okButtonCaption, const char *cancelButtonCaption) = 0;

			// Check whether the text input dialog is open now
			virtual bool IsTextInputDialogOpen() = 0;

			// Get last value input to the text input dialog
			virtual GameDataString GetTextInputDialogText() = 0;

			// Check whether the dialog was closed as OK or Canceled
			virtual bool IsDialogClosedOK() = 0;
		};
	}
}

#endif
