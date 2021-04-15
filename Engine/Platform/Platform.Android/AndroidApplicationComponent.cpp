////////////////////////////////////////////////////////////////////////////////
// AndroidApplicationComponent.cpp (Leggiero - Platform.Android)
//
// Implementation of Application Component Functions in Android Platform
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "AndroidPlatformApplication.h"

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>

// Leggiero.Engine
#include <Engine/Application/CreateGame.h>
#include <Engine/Application/IGame.h>

// Leggiero.Application
#include <Application/SystemEventObserver.h>

// Leggiero.Platform.Android
#include "AndroidPlatform/AndroidJNISupport.h"
#include "Service/AndroidTextInputDialog.h"
#include "Service/AndroidWebView.h"


namespace Leggiero
{
	namespace Platform
	{
		namespace Android
		{
			//////////////////////////////////////////////////////////////////////////////// AndroidApplicationComponent

			//------------------------------------------------------------------------------
			std::string AndroidPlatformApplication::GetAppReferrer()
			{
				JNIEnv *pEnv = GetJNIEnv();
				if (pEnv == nullptr)
				{
					return "";
				}

				jobject activityCopy = pEnv->NewGlobalRef(g_androidActivity);
				if (activityCopy == nullptr)
				{
					return "";
				}
				auto releaseActivityReferenceFunc = [activityCopy, pEnv]() mutable { pEnv->DeleteGlobalRef(activityCopy); };
				FINALLY_OF_BLOCK(_RELEASE_ACTIVITY_REF_FUNC, releaseActivityReferenceFunc);

				jclass activityClass = pEnv->GetObjectClass(activityCopy);
				if (!activityClass)
				{
					// Failed to Find Class
					return "";
				}

				jmethodID getMethod = pEnv->GetMethodID(activityClass, "GetStoredReferrerString", "()Ljava/lang/String;");
				if (!getMethod)
				{
					// Failed to Find Method
					pEnv->DeleteLocalRef(activityClass);
					return "";
				}

				jstring idJavaString = (jstring)pEnv->CallObjectMethod(activityCopy, getMethod);

				// Extract Return String
				const char *idCString = pEnv->GetStringUTFChars(idJavaString, nullptr);
				std::string idString(idCString);
				pEnv->ReleaseStringUTFChars(idJavaString, idCString);
				pEnv->DeleteLocalRef(activityClass);

				return idString;
			}

			//------------------------------------------------------------------------------
			GameDataString AndroidPlatformApplication::GetDeviceName() const
			{
				JNIEnv *pEnv = GetJNIEnv();
				if (pEnv == nullptr)
				{
					return "";
				}

				jclass utilityClass = FindJavaClass(pEnv, "com/leggiero/Engine/LeggieroUtils");
				if (!utilityClass)
				{
					return "";
				}

				jmethodID getMethod = pEnv->GetStaticMethodID(utilityClass, "GetDeviceName", "()Ljava/lang/String;");
				if (!getMethod)
				{
					pEnv->DeleteLocalRef(utilityClass);
					return "";
				}

				jstring resultJavaString = (jstring)pEnv->CallStaticObjectMethod(utilityClass, getMethod);

				// Extract Return String
				const char *resultCString = pEnv->GetStringUTFChars(resultJavaString, nullptr);
				std::string resultString(resultCString);
				pEnv->ReleaseStringUTFChars(resultJavaString, resultCString);
				pEnv->DeleteLocalRef(utilityClass);

				return resultString;
			}


			//////////////////////////////////////////////////////////////////////////////// AndroidApplicationComponent - Services

			//------------------------------------------------------------------------------
			Application::IAppTextInputController &AndroidPlatformApplication::TextInputController()
			{
				return *m_textInputDialogController;
			}

			//------------------------------------------------------------------------------
			Application::IAppWebViewController &AndroidPlatformApplication::WebViewController()
			{
				return *m_webViewController;
			}
		}
	}
}
