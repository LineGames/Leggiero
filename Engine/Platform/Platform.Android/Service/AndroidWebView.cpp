////////////////////////////////////////////////////////////////////////////////
// Service/AndroidWebView.cpp (Leggiero - Platform.Android)
//
// Android Web View Service Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "AndroidWebView.h"

// Leggiero.Platform.Android
#include "../AndroidCommon.h"
#include "../AndroidPlatform/AndroidJNISupport.h"


namespace Leggiero
{
	namespace Platform
	{
		namespace Android
		{
			//////////////////////////////////////////////////////////////////////////////// AndroidWebView

			//------------------------------------------------------------------------------
			AndroidWebView::AndroidWebView()
			{
			}

			//------------------------------------------------------------------------------
			AndroidWebView::~AndroidWebView()
			{
			}

			//------------------------------------------------------------------------------
			// Open URL in a Page
			void AndroidWebView::OpenURLByPage(const char *pageTitle, const char *buttonCaption, const char *url)
			{
				JNIEnv *pEnv = GetJNIEnv();
				if (pEnv == nullptr)
				{
					return;
				}

				jclass utilityClass = FindJavaClass(pEnv, "com/leggiero/Engine/Service/LeggieroWebView");
				if (!utilityClass)
				{
					return;
				}

				jmethodID getMethod = pEnv->GetStaticMethodID(utilityClass, "OpenURLByPage", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
				if (!getMethod)
				{
					pEnv->DeleteLocalRef(utilityClass);
					return;
				}

				jstring stringArg1 = pEnv->NewStringUTF(pageTitle);
				jstring stringArg2 = pEnv->NewStringUTF(buttonCaption);
				jstring stringArg3 = pEnv->NewStringUTF(url);

				pEnv->CallStaticVoidMethod(utilityClass, getMethod, stringArg1, stringArg2, stringArg3);

				pEnv->DeleteLocalRef(stringArg1);
				pEnv->DeleteLocalRef(stringArg2);
				pEnv->DeleteLocalRef(stringArg3);
				pEnv->DeleteLocalRef(utilityClass);
				return;
			}

			//------------------------------------------------------------------------------
			// Check Whether URL Page is Opened or NOT
			bool AndroidWebView::IsURLPageOpened()
			{
				JNIEnv *pEnv = GetJNIEnv();
				if (pEnv == nullptr)
				{
					return false;
				}

				jclass utilityClass = FindJavaClass(pEnv, "com/leggiero/Engine/Service/LeggieroWebView");
				if (!utilityClass)
				{
					return false;
				}

				jmethodID getMethod = pEnv->GetStaticMethodID(utilityClass, "IsURLPageOpened", "()I");
				if (!getMethod)
				{
					pEnv->DeleteLocalRef(utilityClass);
					return false;
				}

				jint result = pEnv->CallStaticIntMethod(utilityClass, getMethod);
				pEnv->DeleteLocalRef(utilityClass);

				if (result == 1)
				{
					return true;
				}
				return false;
			}

			//------------------------------------------------------------------------------
			// Open URL
			void AndroidWebView::OpenURL(const char *url)
			{
				JNIEnv *pEnv = GetJNIEnv();
				if (pEnv == nullptr)
				{
					return;
				}

				jclass utilityClass = FindJavaClass(pEnv, "com/leggiero/Engine/Service/LeggieroWebView");
				if (!utilityClass)
				{
					return;
				}

				jmethodID getMethod = pEnv->GetStaticMethodID(utilityClass, "OpenURL", "(Ljava/lang/String;)V");
				if (!getMethod)
				{
					pEnv->DeleteLocalRef(utilityClass);
					return;
				}

				jstring stringArg1 = pEnv->NewStringUTF(url);

				pEnv->CallStaticVoidMethod(utilityClass, getMethod, stringArg1);

				pEnv->DeleteLocalRef(stringArg1);
				pEnv->DeleteLocalRef(utilityClass);
				return;
			}

			//------------------------------------------------------------------------------
			// Check given URL Scheme is openable or not
			bool AndroidWebView::CanOpenURL(const char *urlScheme)
			{
				JNIEnv *pEnv = GetJNIEnv();
				if (pEnv == nullptr)
				{
					return false;
				}

				jclass utilityClass = FindJavaClass(pEnv, "com/leggiero/Engine/Service/LeggieroWebView");
				if (!utilityClass)
				{
					return false;
				}

				jmethodID getMethod = pEnv->GetStaticMethodID(utilityClass, "CanOpenURL", "(Ljava/lang/String;)I");
				if (!getMethod)
				{
					pEnv->DeleteLocalRef(utilityClass);
					return false;
				}

				jstring stringArg1 = pEnv->NewStringUTF(urlScheme);

				jint result = pEnv->CallStaticIntMethod(utilityClass, getMethod, stringArg1);

				pEnv->DeleteLocalRef(stringArg1);
				pEnv->DeleteLocalRef(utilityClass);

				if (result == 1)
				{
					return true;
				}
				return false;
			}
		}
	}
}
