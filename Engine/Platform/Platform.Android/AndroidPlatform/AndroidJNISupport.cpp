////////////////////////////////////////////////////////////////////////////////
// AndroidPlatform/AndroidJNISupport.cpp (Leggiero - Platform.Android)
//
// JNI Support Common Implementations
////////////////////////////////////////////////////////////////////////////////


// My Header
#include "AndroidJNISupport.h"
#include "_JNIInternal.h"


namespace Leggiero
{
	namespace Platform
	{
		namespace Android
		{
			//////////////////////////////////////////////////////////////////////////////// Global Common Android Objects

			//------------------------------------------------------------------------------
			JavaVM *g_pJavaVM = nullptr;
			jobject g_androidActivity = nullptr;


			//////////////////////////////////////////////////////////////////////////////// Internal Multi-Thread Support

			namespace _Internal
			{
				static jobject g_cachedClassLoader = nullptr;
				static jmethodID g_cachedFindClassMethod = nullptr;

				//------------------------------------------------------------------------------
				void InitializeMultiThreadSupport(JNIEnv *pEnv, jobject activityObject)
				{
					jclass activityClass = pEnv->GetObjectClass(activityObject);
					jclass classClass = pEnv->GetObjectClass((jobject)activityClass);
					jclass classLoaderClass = pEnv->FindClass("java/lang/ClassLoader");
					jmethodID getClassLoaderMethod = pEnv->GetMethodID(classClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
					g_cachedClassLoader = pEnv->NewGlobalRef(pEnv->CallObjectMethod((jobject)activityClass, getClassLoaderMethod));
					g_cachedFindClassMethod = pEnv->GetMethodID(classLoaderClass, "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");
				}
			}


			//////////////////////////////////////////////////////////////////////////////// Common JNI Implementations

			//------------------------------------------------------------------------------
			// Java Environment
			JNIEnv *GetJNIEnv()
			{
				if (g_pJavaVM == nullptr)
				{
					return nullptr;
				}

				JNIEnv *pEnv = NULL;
				jint envGetResult = g_pJavaVM->GetEnv((void **)&pEnv, JNI_VERSION_1_6);
				switch (envGetResult)
				{
					case JNI_OK:
						return pEnv;

					case JNI_EDETACHED:
						{
							if (g_pJavaVM->AttachCurrentThread(&pEnv, NULL) != 0)
							{
								// Thread Attach Failed
								return nullptr;
							}
							return pEnv;
						}

					case JNI_EVERSION:
						// JNI Version Not Supported
						return nullptr;
				}

				return nullptr;
			}

			//------------------------------------------------------------------------------
			// Java Class Find
			jclass FindJavaClass(JNIEnv *pEnv, const std::string &className)
			{
				jstring classNameArg = pEnv->NewStringUTF(className.c_str());
				return static_cast<jclass>(pEnv->CallObjectMethod(_Internal::g_cachedClassLoader, _Internal::g_cachedFindClassMethod, classNameArg));
				pEnv->DeleteLocalRef(classNameArg);
			}

			//------------------------------------------------------------------------------
			// Convert ArrayList<String> to C++ std::vector<std::string>
			std::vector<std::string> GetStringList(JNIEnv *pEnv, jobject javaArrayList)
			{
				jclass arrayListClass = pEnv->FindClass("java/util/ArrayList");
				if (!arrayListClass)
				{
					return std::vector<std::string>();
				}

				jmethodID sizeMethod = pEnv->GetMethodID(arrayListClass, "size", "()I");
				jmethodID getMethod = pEnv->GetMethodID(arrayListClass, "get", "(I)Ljava/lang/Object;");

				jint listLength = pEnv->CallIntMethod(javaArrayList, sizeMethod);

				std::vector<std::string> resultBuffer;
				resultBuffer.reserve(listLength);
				for (jint i = 0; i < listLength; ++i)
				{
					jstring currentItem = static_cast<jstring>(pEnv->CallObjectMethod(javaArrayList, getMethod, i));

					const char *itemString = pEnv->GetStringUTFChars(currentItem, 0);
					resultBuffer.emplace_back(itemString);
					pEnv->ReleaseStringUTFChars(currentItem, itemString);
					pEnv->DeleteLocalRef(currentItem);
				}

				pEnv->DeleteLocalRef(arrayListClass);

				return resultBuffer;
			}
		}
	}
}
