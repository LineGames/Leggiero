////////////////////////////////////////////////////////////////////////////////
// _Internal/AndroidFileSystemJNIInterface.cpp (Leggiero/Modules - FileSystem)
//
// JNI Interface for FileSystem Module
////////////////////////////////////////////////////////////////////////////////


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Android
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

// Leggiero.Platform.Android
#include <AndroidPlatform/AndroidJNISupport.h>


//////////////////////////////////////////////////////////////////////////////// Internal Stuffs for Android File System

namespace Leggiero
{
	namespace FileSystem
	{
		namespace _Internal
		{
			//------------------------------------------------------------------------------
			extern AAssetManager *g_androidAssetManager;

			extern bool g_android_isExternalStorageAvailable;

			extern std::string g_android_mainDataRealPath;
			extern std::string g_android_rawDataRealPath;
			extern std::string g_android_cacheRealPath;
			extern std::string g_android_tempRealPath;

			extern std::string g_android_externalMainDataRealPath;
			extern std::string g_android_externalRawDataRealPath;
			extern std::string g_android_externalCacheRealPath;

			//------------------------------------------------------------------------------
			std::string ProcessRawPath(const char *path)
			{
				std::string pathString(path);

				if (pathString.length() > 0)
				{
					char lastChar = pathString[pathString.length() - 1];
					if (lastChar != '/')
					{
						pathString += '/';
					}
				}
				return pathString;
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////////// JNI Helpers

//------------------------------------------------------------------------------
// Initialize bundle asset by JNI Call
JNI_INTERFACE JNIEXPORT void JNICALL Java_com_leggiero_Engine_Modules_FileSystem_FileSystemHelper_InitializeBundleAsset(JNIEnv *env, jobject javaThis, jobject assetManager)
{
	Leggiero::FileSystem::_Internal::g_androidAssetManager = AAssetManager_fromJava(env, assetManager);
}

//------------------------------------------------------------------------------
// External Storage Availability
JNI_INTERFACE JNIEXPORT void JNICALL Java_com_leggiero_Engine_Modules_FileSystem_FileSystemHelper_SetIsExternalStorageAvailable(JNIEnv *env, jobject javaThis, jboolean isAvailable)
{
	Leggiero::FileSystem::_Internal::g_android_isExternalStorageAvailable = ((isAvailable == JNI_TRUE) ? true : false);
}

//------------------------------------------------------------------------------
// Initialize Directory Paths
JNI_INTERFACE JNIEXPORT void JNICALL Java_com_leggiero_Engine_Modules_FileSystem_FileSystemHelper_SetDataPaths(JNIEnv *env, jobject javaThis, jstring mainDataPath, jstring rawDataPath, jstring cachePath, jstring tmpPath)
{
	const char *str1 = env->GetStringUTFChars(mainDataPath, 0);
	Leggiero::FileSystem::_Internal::g_android_mainDataRealPath = Leggiero::FileSystem::_Internal::ProcessRawPath(str1);
	env->ReleaseStringUTFChars(mainDataPath, str1);

	const char *str2 = env->GetStringUTFChars(rawDataPath, 0);
	Leggiero::FileSystem::_Internal::g_android_rawDataRealPath = Leggiero::FileSystem::_Internal::ProcessRawPath(str2);
	env->ReleaseStringUTFChars(rawDataPath, str2);

	const char *str3 = env->GetStringUTFChars(cachePath, 0);
	Leggiero::FileSystem::_Internal::g_android_cacheRealPath = Leggiero::FileSystem::_Internal::ProcessRawPath(str3);
	env->ReleaseStringUTFChars(cachePath, str3);

	const char *str4 = env->GetStringUTFChars(tmpPath, 0);
	Leggiero::FileSystem::_Internal::g_android_tempRealPath = Leggiero::FileSystem::_Internal::ProcessRawPath(str4);
	env->ReleaseStringUTFChars(tmpPath, str4);
}

//------------------------------------------------------------------------------
// Initialize External Directory Paths
JNI_INTERFACE JNIEXPORT void JNICALL Java_com_leggiero_Engine_Modules_FileSystem_FileSystemHelper_SetExternalDataPaths(JNIEnv *env, jobject javaThis, jstring mainDataPath, jstring rawDataPath, jstring cachePath)
{
	const char *str1 = env->GetStringUTFChars(mainDataPath, 0);
	Leggiero::FileSystem::_Internal::g_android_externalMainDataRealPath = Leggiero::FileSystem::_Internal::ProcessRawPath(str1);
	env->ReleaseStringUTFChars(mainDataPath, str1);

	const char *str2 = env->GetStringUTFChars(rawDataPath, 0);
	Leggiero::FileSystem::_Internal::g_android_externalRawDataRealPath = Leggiero::FileSystem::_Internal::ProcessRawPath(str2);
	env->ReleaseStringUTFChars(rawDataPath, str2);

	const char *str3 = env->GetStringUTFChars(cachePath, 0);
	Leggiero::FileSystem::_Internal::g_android_externalCacheRealPath = Leggiero::FileSystem::_Internal::ProcessRawPath(str3);
	env->ReleaseStringUTFChars(cachePath, str3);
}
