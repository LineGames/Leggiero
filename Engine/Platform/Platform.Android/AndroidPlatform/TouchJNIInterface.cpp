////////////////////////////////////////////////////////////////////////////////
// AndroidPlatform/TouchJNIInterface.cpp (Leggiero - Platform.Android)
//
// Android JNI Interface to Handle Touch Events
////////////////////////////////////////////////////////////////////////////////


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.Application
#include <Application/DeviceCommonTypes.h>

// Leggiero.Platform.Android
#include "AndroidJNISupport.h"
#include "../AndroidPlatformApplication.h"


//////////////////////////////////////////////////////////////////////////////// Internal Utility

namespace Leggiero
{
	namespace Platform
	{
		namespace Android
		{
			namespace _Internal
			{
				//------------------------------------------------------------------------------
				Leggiero::GameTimeClockType::time_point _CalculateTouchEventGameTime(jlong eventTime, jlong currentTime, Leggiero::GameTimeClockType::time_point currentReference)
				{
					std::chrono::milliseconds delayedTime(currentTime - eventTime);
					return (currentReference - delayedTime);
				}
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////////// Touch Event Handler

//------------------------------------------------------------------------------
// Handle Android Touch Down Event
JNI_INTERFACE JNIEXPORT void JNICALL Java_com_leggiero_Engine_OpenGL_LeggieroGLSurfaceView_AndroidTouchDown(JNIEnv *env, jobject javaThis,
	jint pointerId, jfloat x, jfloat y, jlong eventTime, jlong currentTime)
{
	Leggiero::GameTimeClockType::time_point callTime = Leggiero::GameTimeClockType::now();

	Leggiero::Platform::Android::AndroidPlatformApplication *leggieroApp = Leggiero::Platform::Android::AndroidPlatformApplication::s_globalAppInstance;
	if (leggieroApp == NULL)
	{
		return;
	}

	leggieroApp->OnTouchDown(
		static_cast<Leggiero::Application::DeviceTouchIdType>(pointerId), 
		static_cast<Leggiero::Application::DeviceScreenCoordType>(x), 
		static_cast<Leggiero::Application::DeviceScreenCoordType>(y),
		Leggiero::Platform::Android::_Internal::_CalculateTouchEventGameTime(eventTime, currentTime, callTime));
}

//------------------------------------------------------------------------------
// Handle Android Touch Move Event
JNI_INTERFACE JNIEXPORT void JNICALL Java_com_leggiero_Engine_OpenGL_LeggieroGLSurfaceView_AndroidTouchMove(JNIEnv *env, jobject javaThis,
	jint pointerId, jfloat x, jfloat y, jlong eventTime, jlong currentTime)
{
	Leggiero::GameTimeClockType::time_point callTime = Leggiero::GameTimeClockType::now();

	Leggiero::Platform::Android::AndroidPlatformApplication *leggieroApp = Leggiero::Platform::Android::AndroidPlatformApplication::s_globalAppInstance;
	if (leggieroApp == NULL)
	{
		return;
	}

	leggieroApp->OnTouchMove(
		static_cast<Leggiero::Application::DeviceTouchIdType>(pointerId),
		static_cast<Leggiero::Application::DeviceScreenCoordType>(x),
		static_cast<Leggiero::Application::DeviceScreenCoordType>(y),
		Leggiero::Platform::Android::_Internal::_CalculateTouchEventGameTime(eventTime, currentTime, callTime));
}

//------------------------------------------------------------------------------
// Handle Android Touch Up Event
JNI_INTERFACE JNIEXPORT void JNICALL Java_com_leggiero_Engine_OpenGL_LeggieroGLSurfaceView_AndroidTouchUp(JNIEnv *env, jobject javaThis,
	jint pointerId, jfloat x, jfloat y, jlong eventTime, jlong currentTime)
{
	Leggiero::GameTimeClockType::time_point callTime = Leggiero::GameTimeClockType::now();

	Leggiero::Platform::Android::AndroidPlatformApplication *leggieroApp = Leggiero::Platform::Android::AndroidPlatformApplication::s_globalAppInstance;
	if (leggieroApp == NULL)
	{
		return;
	}

	leggieroApp->OnTouchUp(
		static_cast<Leggiero::Application::DeviceTouchIdType>(pointerId),
		static_cast<Leggiero::Application::DeviceScreenCoordType>(x),
		static_cast<Leggiero::Application::DeviceScreenCoordType>(y),
		Leggiero::Platform::Android::_Internal::_CalculateTouchEventGameTime(eventTime, currentTime, callTime));
}
