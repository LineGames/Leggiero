////////////////////////////////////////////////////////////////////////////////
// _Internal/_AndroidFileSystemPathComponent.cpp (Leggiero/Modules - FileSystem)
//
// Android Platform File System Path Component Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "_AndroidFileSystemPathComponent.h"

// Leggiero.FileSystem
#include "../FileSystemUtility.h"

// Leggiero.Platform.Android
#include <AndroidPlatform/AndroidJNISupport.h>


namespace Leggiero
{
	namespace FileSystem
	{
		//////////////////////////////////////////////////////////////////////////////// Global File System Data

		namespace _Internal
		{
			bool g_android_isExternalStorageAvailable = false;

			std::string g_android_mainDataRealPath;
			std::string g_android_rawDataRealPath;
			std::string g_android_cacheRealPath;
			std::string g_android_tempRealPath;

			std::string g_android_externalMainDataRealPath;
			std::string g_android_externalRawDataRealPath;
			std::string g_android_externalCacheRealPath;
		}


		//////////////////////////////////////////////////////////////////////////////// AndroidFileSystemPathComponent

		//------------------------------------------------------------------------------
		AndroidFileSystemPathComponent::AndroidFileSystemPathComponent()
		{
		}

		//------------------------------------------------------------------------------
		AndroidFileSystemPathComponent::~AndroidFileSystemPathComponent()
		{
		}

		// Data path for normal data which can do cloud backup if it is possible
		std::string AndroidFileSystemPathComponent::MainDataPath() { return _Internal::g_android_mainDataRealPath; }
		std::string AndroidFileSystemPathComponent::MainDataPath(const std::string &virtualPath) { return Utility::CombinePath(_Internal::g_android_mainDataRealPath, virtualPath); }

		// Data path for normal data without backup
		std::string AndroidFileSystemPathComponent::RawDataPath() { return _Internal::g_android_rawDataRealPath; }
		std::string AndroidFileSystemPathComponent::RawDataPath(const std::string &virtualPath) { return Utility::CombinePath(_Internal::g_android_rawDataRealPath, virtualPath); }

		// Data path for cache data
		std::string AndroidFileSystemPathComponent::CachePath() { return _Internal::g_android_cacheRealPath; }
		std::string AndroidFileSystemPathComponent::CachePath(const std::string &virtualPath) { return Utility::CombinePath(_Internal::g_android_cacheRealPath, virtualPath); }

		// Data path for temporary files
		std::string AndroidFileSystemPathComponent::TempPath() { return _Internal::g_android_tempRealPath; }
		std::string AndroidFileSystemPathComponent::TempPath(const std::string &virtualPath) { return Utility::CombinePath(_Internal::g_android_tempRealPath, virtualPath); }

		// Check whether if separated external storage is available or not
		bool AndroidFileSystemPathComponent::NPO_IsExternalStorageAvailable() { return _Internal::g_android_isExternalStorageAvailable; }

		// Get tuple of free space size of main / external storage each
		std::tuple<size_t, size_t> AndroidFileSystemPathComponent::NPO_GetFreeSpaceMainExternalStorages()
		{
			JNIEnv *pEnv = Platform::Android::GetJNIEnv();
			if (pEnv == nullptr)
			{
				return std::make_tuple(0, 0);
			}

			jclass utilityClass = Platform::Android::FindJavaClass(pEnv, "com/leggiero/Engine/Modules/FileSystem/FileSystemHelper");
			if (!utilityClass)
			{
				return std::make_tuple(0, 0);
			}

			jmethodID getMethodInternal = pEnv->GetStaticMethodID(utilityClass, "GetFreeSpaceInternal", "()J");
			if (!getMethodInternal)
			{
				pEnv->DeleteLocalRef(utilityClass);
				return std::make_tuple(0, 0);
			}

			jmethodID getMethodExternal = pEnv->GetStaticMethodID(utilityClass, "GetFreeSpaceExternal", "()J");
			if (!getMethodExternal)
			{
				pEnv->DeleteLocalRef(utilityClass);
				return std::make_tuple(0, 0);
			}

			size_t freeSizeInternal = static_cast<size_t>(pEnv->CallStaticLongMethod(utilityClass, getMethodInternal));
			size_t freeSizeExternal = static_cast<size_t>(pEnv->CallStaticLongMethod(utilityClass, getMethodExternal));
			pEnv->DeleteLocalRef(utilityClass);

			return std::make_tuple(freeSizeInternal, freeSizeExternal);
		}

		// Data path for external storage of normal data which can do cloud backup if it is possible
		std::string AndroidFileSystemPathComponent::NPO_ExternalMainDataPath() { return _Internal::g_android_externalMainDataRealPath; }
		std::string AndroidFileSystemPathComponent::NPO_ExternalMainDataPath(const std::string &virtualPath) { return Utility::CombinePath(_Internal::g_android_externalMainDataRealPath, virtualPath); }

		// Data path for external storage of normal data without backup
		std::string AndroidFileSystemPathComponent::NPO_ExternalRawDataPath() { return _Internal::g_android_externalRawDataRealPath; }
		std::string AndroidFileSystemPathComponent::NPO_ExternalRawDataPath(const std::string &virtualPath) { return Utility::CombinePath(_Internal::g_android_externalRawDataRealPath, virtualPath); }

		// Data path for external storage of cache data
		std::string AndroidFileSystemPathComponent::NPO_ExternalCachePath() { return _Internal::g_android_externalCacheRealPath; }
		std::string AndroidFileSystemPathComponent::NPO_ExternalCachePath(const std::string &virtualPath) { return Utility::CombinePath(_Internal::g_android_externalCacheRealPath, virtualPath); }


		//////////////////////////////////////////////////////////////////////////////// FileSystemPathComponent - Creation Function

		//------------------------------------------------------------------------------
		FileSystemPathComponent *FileSystemPathComponent::CreateComponentObject()
		{
			return new AndroidFileSystemPathComponent();
		}
	}
}
