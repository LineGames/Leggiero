////////////////////////////////////////////////////////////////////////////////
// AndroidPlatformApplication_AppControl.cpp (Leggiero - Platform.Android)
//
// Implementation of Android Platform Application's App Control Parts
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "AndroidPlatformApplication.h"

// Leggiero.Platform.Android
#include "AndroidPlatform/AndroidJNISupport.h"


namespace Leggiero
{
	namespace Platform
	{
		namespace Android
		{
			//////////////////////////////////////////////////////////////////////////////// AndroidPlatformApplication - Platform Application Controls

			//------------------------------------------------------------------------------
			// Set whether to back button(Android) is always handled as default or not.
			// Initial value is true.
			void AndroidPlatformApplication::SetBackButtonDefaultHandling(bool isHandle)
			{
				m_isDefaultBackButtonHandling = isHandle;
			}

			//------------------------------------------------------------------------------
			// Set whether to prevent device sleep on idle
			void AndroidPlatformApplication::SetNoSleepMode(bool isPreventSleep)
			{
				JNIEnv *pEnv = GetJNIEnv();
				if (pEnv == nullptr)
				{
					return;
				}

				jobject activityCopy = pEnv->NewGlobalRef(g_androidActivity);
				if (activityCopy == nullptr)
				{
					return;
				}
				auto releaseActivityReferenceFunc = [activityCopy, pEnv]() mutable { pEnv->DeleteGlobalRef(activityCopy); };
				FINALLY_OF_BLOCK(_RELEASE_ACTIVITY_REF_FUNC, releaseActivityReferenceFunc);

				jclass activityClass = pEnv->GetObjectClass(activityCopy);
				if (!activityClass)
				{
					// Failed to Find Class
					return;
				}

				jmethodID setMethod = pEnv->GetMethodID(activityClass, "SetNoSleepMode", "(Z)V");
				if (!setMethod)
				{
					// Failed to Find Method
					pEnv->DeleteLocalRef(activityClass);
					return;
				}

				pEnv->CallVoidMethod(activityCopy, setMethod, isPreventSleep);

				pEnv->DeleteLocalRef(activityClass);
			}
		}
	}
}
