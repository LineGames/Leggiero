////////////////////////////////////////////////////////////////////////////////
// AndroidPlatform/ApplicationJNIInterface.cpp (Leggiero - Platform.Android)
//
// Android JNI Interface for Application Functions
////////////////////////////////////////////////////////////////////////////////


// Leggiero.Platform.Android
#include "AndroidJNISupport.h"
#include "_JNIInternal.h"
#include "../AndroidPlatformApplication.h"


//////////////////////////////////////////////////////////////////////////////// Internal States

namespace _Internal
{
	static int s_cachedSafePaddingLeft = 0;
	static int s_cachedSafePaddingTop = 0;
	static int s_cachedSafePaddingRight = 0;
	static int s_cachedSafePaddingBottom = 0;
}


//------------------------------------------------------------------------------
// Initialize Leggiero Application
JNI_INTERFACE JNIEXPORT void JNICALL Java_com_leggiero_Engine_LeggieroActivity_InitializeLeggieroApp(JNIEnv *env, jobject javaThis, jboolean isTablet, jint width, jint height, jfloat screenXPPI, jfloat screenYPPI, jstring localeString)
{
	// Release Exsiting Activity Reference
	if (Leggiero::Platform::Android::g_androidActivity != nullptr)
	{
		env->DeleteGlobalRef(Leggiero::Platform::Android::g_androidActivity);
		Leggiero::Platform::Android::g_androidActivity = nullptr;
	}

	// Store Global VM
	env->GetJavaVM(&Leggiero::Platform::Android::g_pJavaVM);
	Leggiero::Platform::Android::g_androidActivity = env->NewGlobalRef(javaThis);

	// Initialize Multi-Thread Support
	Leggiero::Platform::Android::_Internal::InitializeMultiThreadSupport(env, javaThis);

	// Create Game Application
	if (Leggiero::Platform::Android::AndroidPlatformApplication::s_globalAppInstance != NULL)
	{
		return;
	}
	Leggiero::Platform::Android::AndroidPlatformApplication *pApp = new Leggiero::Platform::Android::AndroidPlatformApplication();
	Leggiero::Platform::Android::AndroidPlatformApplication::s_globalAppInstance = pApp;

	// Initial Application Metadata Setting
	pApp->SetScreenSize(width, height, true);
	pApp->SetSafePadding(_Internal::s_cachedSafePaddingLeft, _Internal::s_cachedSafePaddingTop, _Internal::s_cachedSafePaddingRight, _Internal::s_cachedSafePaddingBottom, true);
	pApp->SetApproximateScreenPPI(screenXPPI, screenYPPI);
	pApp->SetIsTablet((isTablet == JNI_TRUE) ? true : false);

	const char *localeNativeString = (const char *)(env->GetStringUTFChars(localeString, JNI_FALSE));
	pApp->SetSystemLocale(localeNativeString);
	env->ReleaseStringUTFChars(localeString, localeNativeString);

	// Initialize the Game
	pApp->InitializeGame();
}

//------------------------------------------------------------------------------
// Process Leggiero Application's Final Termination
JNI_INTERFACE JNIEXPORT void JNICALL Java_com_leggiero_Engine_LeggieroActivity_OnAppDestroy(JNIEnv *env, jobject javaThis)
{
	Leggiero::Platform::Android::AndroidPlatformApplication *leggieroApp = Leggiero::Platform::Android::AndroidPlatformApplication::s_globalAppInstance;
	if (leggieroApp != nullptr)
	{
		leggieroApp->TerminateGame();
		delete leggieroApp;
		leggieroApp = nullptr;
	}

	// Release Activity Reference
	if (Leggiero::Platform::Android::g_androidActivity != nullptr)
	{
		env->DeleteGlobalRef(Leggiero::Platform::Android::g_androidActivity);
		Leggiero::Platform::Android::g_androidActivity = nullptr;
	}

	// Discard Global VM
	Leggiero::Platform::Android::g_pJavaVM = nullptr;
}

//------------------------------------------------------------------------------
// Notify Application Start
JNI_INTERFACE JNIEXPORT void JNICALL Java_com_leggiero_Engine_LeggieroActivity_OnAppStart(JNIEnv *env, jobject javaThis)
{
	// Do Nothing for Now
}


//------------------------------------------------------------------------------
// Notify Application Stop
JNI_INTERFACE JNIEXPORT void JNICALL Java_com_leggiero_Engine_LeggieroActivity_OnAppStop(JNIEnv *env, jobject javaThis)
{
	Leggiero::Platform::Android::AndroidPlatformApplication *leggieroApp = Leggiero::Platform::Android::AndroidPlatformApplication::s_globalAppInstance;
	if (leggieroApp == NULL)
	{
		return;
	}

	leggieroApp->OnGoToBackground();
}

//------------------------------------------------------------------------------
// Notify Application Restart
JNI_INTERFACE JNIEXPORT void JNICALL Java_com_leggiero_Engine_LeggieroActivity_OnAppRestart(JNIEnv *env, jobject javaThis)
{
	Leggiero::Platform::Android::AndroidPlatformApplication *leggieroApp = Leggiero::Platform::Android::AndroidPlatformApplication::s_globalAppInstance;
	if (leggieroApp == NULL)
	{
		return;
	}

	leggieroApp->OnReturnFromBackground();
}

//------------------------------------------------------------------------------
// Notify Application Resume
JNI_INTERFACE JNIEXPORT void JNICALL Java_com_leggiero_Engine_LeggieroActivity_OnAppResume(JNIEnv *env, jobject javaThis)
{
	Leggiero::Platform::Android::AndroidPlatformApplication *leggieroApp = Leggiero::Platform::Android::AndroidPlatformApplication::s_globalAppInstance;
	if (leggieroApp == NULL)
	{
		return;
	}

	leggieroApp->OnResume();
}

//------------------------------------------------------------------------------
// Notify Application Pause
JNI_INTERFACE JNIEXPORT void JNICALL Java_com_leggiero_Engine_LeggieroActivity_OnAppPause(JNIEnv *env, jobject javaThis)
{
	Leggiero::Platform::Android::AndroidPlatformApplication *leggieroApp = Leggiero::Platform::Android::AndroidPlatformApplication::s_globalAppInstance;
	if (leggieroApp == NULL)
	{
		return;
	}

	// All touch will be canceled on pause
	leggieroApp->CancelAllTouch();

	leggieroApp->OnPause();
}

//------------------------------------------------------------------------------
// Process Back Button Event
JNI_INTERFACE JNIEXPORT jint JNICALL Java_com_leggiero_Engine_LeggieroActivity_OnSystemBackButtonPressed(JNIEnv *env, jobject javaThis)
{
	Leggiero::Platform::Android::AndroidPlatformApplication *leggieroApp = Leggiero::Platform::Android::AndroidPlatformApplication::s_globalAppInstance;
	if (leggieroApp == NULL)
	{
		return 0;
	}

	bool isHandled = leggieroApp->OnBackButtonPressed();
	return (isHandled ? 1 : 0);
}

//------------------------------------------------------------------------------
// Receive System Low Memory Warning
JNI_INTERFACE JNIEXPORT void JNICALL Java_com_leggiero_Engine_LeggieroActivity_OnLowMemoryWarning(JNIEnv *env, jobject javaThis)
{
	Leggiero::Platform::Android::AndroidPlatformApplication *leggieroApp = Leggiero::Platform::Android::AndroidPlatformApplication::s_globalAppInstance;
	if (leggieroApp == NULL)
	{
		return;
	}

	leggieroApp->OnMemoryWarning();
}

//------------------------------------------------------------------------------
// Handle Safe Padding Area Notification
JNI_INTERFACE JNIEXPORT void JNICALL Java_com_leggiero_Engine_LeggieroActivity_NotifySafePaddingArea(JNIEnv *env, jobject javaThis, jint left, jint top, jint right, jint bottom)
{
	_Internal::s_cachedSafePaddingLeft = left;
	_Internal::s_cachedSafePaddingTop = top;
	_Internal::s_cachedSafePaddingRight = right;
	_Internal::s_cachedSafePaddingBottom = bottom;

	Leggiero::Platform::Android::AndroidPlatformApplication *leggieroApp = Leggiero::Platform::Android::AndroidPlatformApplication::s_globalAppInstance;
	if (leggieroApp == NULL)
	{
		return;
	}

	leggieroApp->SetSafePadding(_Internal::s_cachedSafePaddingLeft, _Internal::s_cachedSafePaddingTop, _Internal::s_cachedSafePaddingRight, _Internal::s_cachedSafePaddingBottom, false);
}
