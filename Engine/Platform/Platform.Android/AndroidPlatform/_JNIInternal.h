////////////////////////////////////////////////////////////////////////////////
// AndroidPlatform/_JNIInternal.h (Leggiero - Platform.Android)
//   * DO NOT directly include this header file outside of Leggiero Engine
//
// Engine Internal Implementations for JNI
////////////////////////////////////////////////////////////////////////////////

#ifndef __LP_ANDROID__ANDROID_PLATFORM___JNI_INTERNAL_H
#define __LP_ANDROID__ANDROID_PLATFORM___JNI_INTERNAL_H


// Leggiero.Platform.Android
#include "../AndroidCommon.h"


namespace Leggiero
{
	namespace Platform
	{
		namespace Android
		{
			namespace _Internal
			{
				// For Multi-Thread Support
				void InitializeMultiThreadSupport(JNIEnv *pEnv, jobject activityObject);
			}
		}
	}
}

#endif
