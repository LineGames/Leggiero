////////////////////////////////////////////////////////////////////////////////
// Platform/AndroidSoundJNIInterface.cpp (Leggiero/Modules - Sound)
//
// JNI Interface for Android Sound Module
////////////////////////////////////////////////////////////////////////////////

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <cstdint>

// Leggiero.Platform.Android
#include <AndroidPlatform/AndroidJNISupport.h>

// Leggiero.Sound
#include "../OboeBackend/OboeCommon.h"


//////////////////////////////////////////////////////////////////////////////// JNI Helpers

//------------------------------------------------------------------------------
// Initialize bundle asset by JNI Call
JNI_INTERFACE JNIEXPORT void JNICALL Java_com_leggiero_Engine_Modules_Sound_SoundHelper_InitializeDefaultStreamValues(JNIEnv *env, jobject javaThis, jint sampleRate, jint framesPerBurst)
{
	Leggiero::Sound::Oboe::SetDefaultValuesForStream(static_cast<int32_t>(sampleRate), static_cast<int32_t>(framesPerBurst));
}
