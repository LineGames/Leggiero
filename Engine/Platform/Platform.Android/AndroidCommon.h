////////////////////////////////////////////////////////////////////////////////
// AndroidCommon.h (Leggiero - Platform.Android)
//
// Common Header to use Android Platform Stuffs and JNI
////////////////////////////////////////////////////////////////////////////////

#ifndef __LP_ANDROID__ANDROID_COMMON_H
#define __LP_ANDROID__ANDROID_COMMON_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <string>

// External Library
#include <jni.h>


// JNI function
#ifdef __cplusplus
	#define JNI_INTERFACE extern "C"
#else
	#define JNI_INTERFACE
#endif


#endif
