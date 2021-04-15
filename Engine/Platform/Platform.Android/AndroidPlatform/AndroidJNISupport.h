////////////////////////////////////////////////////////////////////////////////
// AndroidPlatform/AndroidJNISupport.h (Leggiero - Platform.Android)
//
// Utility for Android JNI
////////////////////////////////////////////////////////////////////////////////

#ifndef __LP_ANDROID__ANDROID_PLATFORM__ANDROID_JNI_SUPPORT_H
#define __LP_ANDROID__ANDROID_PLATFORM__ANDROID_JNI_SUPPORT_H


// Leggiero.Platform.Android
#include "../AndroidCommon.h"

// Standard Library
#include <string>
#include <vector>


namespace Leggiero
{
	namespace Platform
	{
		namespace Android
		{
			// Global VM Object
			extern JavaVM *g_pJavaVM;

			// Android Activity
			extern jobject g_androidActivity;


			// Get JNI Environment in Native Codes
			JNIEnv *GetJNIEnv();

			// Find Java Class in Multi-Thread Environment
			jclass FindJavaClass(JNIEnv *pEnv, const std::string &className);


			// Convert ArrayList<String> to C++ std::vector<std::string>
			std::vector<std::string> GetStringList(JNIEnv *pEnv, jobject javaArrayList);
		}
	}
}

#endif
