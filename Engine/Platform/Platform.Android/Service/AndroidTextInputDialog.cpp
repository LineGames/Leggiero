////////////////////////////////////////////////////////////////////////////////
// Service/AndroidTextInputDialog.cpp (Leggiero - Platform.Android)
//
// Android Text Input Dialog Service Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "AndroidTextInputDialog.h"

// Leggiero.Platform.Android
#include "../AndroidCommon.h"
#include "../AndroidPlatform/AndroidJNISupport.h"


namespace Leggiero
{
	namespace Platform
	{
		namespace Android
		{
			//////////////////////////////////////////////////////////////////////////////// AndroidTextInputDialog

			//------------------------------------------------------------------------------
			AndroidTextInputDialog::AndroidTextInputDialog()
			{
			}

			//------------------------------------------------------------------------------
			AndroidTextInputDialog::~AndroidTextInputDialog()
			{
			}

			//------------------------------------------------------------------------------
			// Open Text Input Dialog
			void AndroidTextInputDialog::OpenTextInputDialog(const char *dialogTitle, const char *dialogMessage, const GameDataString &initialText, const char *okButtonCaption, const char *cancelButtonCaption)
			{
				JNIEnv *pEnv = GetJNIEnv();
				if (pEnv == nullptr)
				{
					return;
				}

				jclass utilityClass = FindJavaClass(pEnv, "com/leggiero/Engine/Service/LeggieroInputDialog");
				if (!utilityClass)
				{
					return;
				}

				jmethodID getMethod = pEnv->GetStaticMethodID(utilityClass, "OpenInputDialog", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
				if (!getMethod)
				{
					pEnv->DeleteLocalRef(utilityClass);
					return;
				}

				jstring stringArg1 = pEnv->NewStringUTF(dialogTitle);
				jstring stringArg2 = pEnv->NewStringUTF(dialogMessage);
				jstring stringArg3 = pEnv->NewStringUTF(okButtonCaption);
				jstring stringArg4 = pEnv->NewStringUTF(cancelButtonCaption);
				jstring stringArg5 = pEnv->NewStringUTF(initialText.c_str());

				pEnv->CallStaticVoidMethod(utilityClass, getMethod, stringArg1, stringArg2, stringArg3, stringArg4, stringArg5);

				pEnv->DeleteLocalRef(stringArg1);
				pEnv->DeleteLocalRef(stringArg2);
				pEnv->DeleteLocalRef(stringArg3);
				pEnv->DeleteLocalRef(stringArg4);
				pEnv->DeleteLocalRef(stringArg5);
				pEnv->DeleteLocalRef(utilityClass);
				return;
			}

			//------------------------------------------------------------------------------
			// Check whether the text input dialog is open now
			bool AndroidTextInputDialog::IsTextInputDialogOpen()
			{
				JNIEnv *pEnv = GetJNIEnv();
				if (pEnv == nullptr)
				{
					return false;
				}

				jclass utilityClass = FindJavaClass(pEnv, "com/leggiero/Engine/Service/LeggieroInputDialog");
				if (!utilityClass)
				{
					return false;
				}

				jmethodID getMethod = pEnv->GetStaticMethodID(utilityClass, "IsOpenInputDialog", "()I");
				if (!getMethod)
				{
					pEnv->DeleteLocalRef(utilityClass);
					return false;
				}

				bool result = (1 == pEnv->CallStaticIntMethod(utilityClass, getMethod));
				pEnv->DeleteLocalRef(utilityClass);

				return result;
			}

			//------------------------------------------------------------------------------
			// Get last value input to the text input dialog
			GameDataString AndroidTextInputDialog::GetTextInputDialogText()
			{
				JNIEnv *pEnv = GetJNIEnv();
				if (pEnv == nullptr)
				{
					return "";
				}

				jclass utilityClass = FindJavaClass(pEnv, "com/leggiero/Engine/Service/LeggieroInputDialog");
				if (!utilityClass)
				{
					return "";
				}

				jmethodID getMethod = pEnv->GetStaticMethodID(utilityClass, "GetInputDialogText", "()[B");
				if (!getMethod)
				{
					pEnv->DeleteLocalRef(utilityClass);
					return "";
				}

				jbyteArray result = (jbyteArray)pEnv->CallStaticObjectMethod(utilityClass, getMethod);
				jbyte *inputTextBuffer = pEnv->GetByteArrayElements(result, nullptr);
				jsize inputTextSize = pEnv->GetArrayLength(result);

				std::string inputString((const char *)inputTextBuffer, inputTextSize);

				pEnv->ReleaseByteArrayElements(result, inputTextBuffer, JNI_ABORT);
				pEnv->DeleteLocalRef(utilityClass);

				return inputString;
			}

			//------------------------------------------------------------------------------
			// Check whether the dialog was closed as OK or Canceled
			bool AndroidTextInputDialog::IsDialogClosedOK()
			{
				JNIEnv *pEnv = GetJNIEnv();
				if (pEnv == nullptr)
				{
					return false;
				}

				jclass utilityClass = FindJavaClass(pEnv, "com/leggiero/Engine/Service/LeggieroInputDialog");
				if (!utilityClass)
				{
					return false;
				}

				jmethodID getMethod = pEnv->GetStaticMethodID(utilityClass, "GetDialogClosedState", "()I");
				if (!getMethod)
				{
					pEnv->DeleteLocalRef(utilityClass);
					return false;
				}

				bool result = (0 == pEnv->CallStaticIntMethod(utilityClass, getMethod));
				pEnv->DeleteLocalRef(utilityClass);

				return result;
			}
		}
	}
}
