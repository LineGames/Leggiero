////////////////////////////////////////////////////////////////////////////////
// AndroidPlatform/OpenGL/OpenGLJNI.cpp (Leggiero - Platform.Android)
//
// OpenGL Related JNI Interfaces
////////////////////////////////////////////////////////////////////////////////

// External Library
#include <GLES3.h>

// Leggiero.Platform.Android
#include "../AndroidJNISupport.h"
#include "../../AndroidPlatformApplication.h"


//------------------------------------------------------------------------------
// Process Draw a Frame for Leggiero Application
JNI_INTERFACE JNIEXPORT void JNICALL Java_com_leggiero_Engine_OpenGL_LeggieroGLRenderer_LeggieroDrawFrame(JNIEnv *env, jobject javaThis)
{
	Leggiero::Platform::Android::AndroidPlatformApplication *leggieroApp = Leggiero::Platform::Android::AndroidPlatformApplication::s_globalAppInstance;
	if (leggieroApp == NULL)
	{
		return;
	}
	leggieroApp->DrawFrame();
}

//------------------------------------------------------------------------------
// Notify GL Surface's Size Change
JNI_INTERFACE JNIEXPORT void JNICALL Java_com_leggiero_Engine_OpenGL_LeggieroGLRenderer_OnSurfaceSizeChanged(JNIEnv *env, jobject javaThis, jint width, jint height)
{
	glViewport(0, 0, width, height);

	Leggiero::Platform::Android::AndroidPlatformApplication *leggieroApp = Leggiero::Platform::Android::AndroidPlatformApplication::s_globalAppInstance;
	if (leggieroApp == NULL)
	{
		return;
	}

	leggieroApp->SetScreenSize((int)width, (int)height, false);
}

//------------------------------------------------------------------------------
// Do Graphic Initialization of Leggiero Application
JNI_INTERFACE JNIEXPORT void JNICALL Java_com_leggiero_Engine_OpenGL_LeggieroGLRenderer_LeggieroGraphicInitialize(JNIEnv *env, jobject javaThis)
{
	Leggiero::Platform::Android::AndroidPlatformApplication *leggieroApp = Leggiero::Platform::Android::AndroidPlatformApplication::s_globalAppInstance;
	if (leggieroApp == NULL)
	{
		return;
	}
	leggieroApp->InitializeGameGraphic();
}

//------------------------------------------------------------------------------
// Do Graphic Shutdown of Leggiero Application
JNI_INTERFACE JNIEXPORT void JNICALL Java_com_leggiero_Engine_OpenGL_LeggieroGLRenderer_LeggieroGraphicShutdown(JNIEnv *env, jobject javaThis)
{
	Leggiero::Platform::Android::AndroidPlatformApplication *leggieroApp = Leggiero::Platform::Android::AndroidPlatformApplication::s_globalAppInstance;
	if (leggieroApp == NULL)
	{
		return;
	}
	leggieroApp->TerminateGameGraphic();
}
