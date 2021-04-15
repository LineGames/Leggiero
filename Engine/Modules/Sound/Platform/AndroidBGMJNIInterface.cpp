////////////////////////////////////////////////////////////////////////////////
// Platform/AndroidBGMJNIInterface.cpp (Leggiero/Modules - Sound)
//
// JNI Interface for Android BGM Playing
////////////////////////////////////////////////////////////////////////////////

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <cstdint>

// Leggiero.Platform.Android
#include <AndroidPlatform/AndroidJNISupport.h>

// Leggiero.Sound
#include "AndroidBGMPlayer.h"
#include "_AndroidBGMPlayingContext.h"


//////////////////////////////////////////////////////////////////////////////// JNI Helpers

//------------------------------------------------------------------------------
// Initialize bundle asset by JNI Call
JNI_INTERFACE JNIEXPORT void JNICALL Java_com_leggiero_Engine_Modules_Sound_BGMPlayingContext_NotifyOnPlayFinished(JNIEnv *env, jobject javaThis, jint contextId, jint isFinished)
{
	bool isFinishedSuccess = (isFinished == 1);
	std::shared_ptr<Leggiero::Sound::Android::_Internal::AndroidBGMPlayingContext> cppContext = 
		Leggiero::Sound::Android::AndroidBGMPlayer::_SearchPlayingContext(static_cast<Leggiero::Sound::Android::AndroidBGMPlayer::ContextIdType>(contextId));
	if (!cppContext)
	{
		return;
	}

	cppContext->NotifyFinishPlayingEvent(isFinishedSuccess);
}
