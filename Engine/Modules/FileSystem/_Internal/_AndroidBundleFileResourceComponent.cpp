////////////////////////////////////////////////////////////////////////////////
// _Internal/_AndroidBundleFileResourceComponent.cpp (Leggiero/Modules - FileSystem)
//
// Android Platform Bundle FIle Resource Component Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "_AndroidBundleFileResourceComponent.h"

// Standrad Library
#include <cstdlib>

// Android
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>

// Leggiero.Platform.Android
#include <AndroidPlatform/AndroidJNISupport.h>

// Leggiero.FileSystem
#include "../FileSystemUtility.h"


namespace Leggiero
{
	namespace FileSystem
	{
		//////////////////////////////////////////////////////////////////////////////// Global State for Android

		namespace _Internal
		{
			//------------------------------------------------------------------------------
			// Global Asset Manager
			AAssetManager *g_androidAssetManager = NULL;

			// Buffer Size
			constexpr size_t kAndroidFileReadBufferSize = 16384;
		}


		//////////////////////////////////////////////////////////////////////////////// AndroidBundleFileResourceComponent

		//------------------------------------------------------------------------------
		AndroidBundleFileResourceComponent::AndroidBundleFileResourceComponent()
		{
		}

		//------------------------------------------------------------------------------
		AndroidBundleFileResourceComponent::~AndroidBundleFileResourceComponent()
		{
		}

		//------------------------------------------------------------------------------
		bool AndroidBundleFileResourceComponent::IsBundleFileExists(const std::string &virtualPath)
		{
			std::string effectiveVirtualPath = Utility::TrimTopAbsoluteDelimiter(virtualPath);

			AAsset *asset = AAssetManager_open(_Internal::g_androidAssetManager, effectiveVirtualPath.c_str(), AASSET_MODE_UNKNOWN);
			if (asset != NULL)
			{
				AAsset_close(asset);
				return true;
			}
			return false;
		}

		//------------------------------------------------------------------------------
		size_t AndroidBundleFileResourceComponent::GetBundleFileLength(const std::string &virtualPath)
		{
			std::string effectiveVirtualPath = Utility::TrimTopAbsoluteDelimiter(virtualPath);

			AAsset *asset = AAssetManager_open(_Internal::g_androidAssetManager, effectiveVirtualPath.c_str(), AASSET_MODE_UNKNOWN);
			if (asset != NULL)
			{
				off_t size = AAsset_getLength(asset);
				AAsset_close(asset);
				return (size_t)size;
			}
			return 0;
		}

		//------------------------------------------------------------------------------
		size_t AndroidBundleFileResourceComponent::ReadBundleFileData(const std::string &virtualPath, size_t offset, char *buffer, size_t bufferSize)
		{
			if (buffer == NULL || bufferSize <= 0)
			{
				// Invalid Argument
				return 0;
			}

			std::string effectiveVirtualPath = Utility::TrimTopAbsoluteDelimiter(virtualPath);

			AAsset *asset = AAssetManager_open(_Internal::g_androidAssetManager, effectiveVirtualPath.c_str(), AASSET_MODE_UNKNOWN);
			if (asset == NULL)
			{
				// FIle not exists or cannot open file
				return 0;
			}

			{
				auto fileCloseFunc = [asset] { AAsset_close(asset); };
				FINALLY_OF_BLOCK(_FILE_CLOSE, fileCloseFunc);

				if (offset > 0)
				{
					if (AAsset_seek(asset, (off_t)offset, SEEK_SET) != (off_t)-1)
					{
						// Invalid offset
						return 0;
					}
				}

				return (size_t)AAsset_read(asset, buffer, bufferSize);
			}
		}

		//------------------------------------------------------------------------------
		std::streamoff AndroidBundleFileResourceComponent::ReadBundleFileData(const std::string &virtualPath, std::streamoff offset, std::ostream &buffer)
		{
			std::string effectiveVirtualPath = Utility::TrimTopAbsoluteDelimiter(virtualPath);

			AAsset *asset = AAssetManager_open(_Internal::g_androidAssetManager, effectiveVirtualPath.c_str(), AASSET_MODE_UNKNOWN);
			if (asset == NULL)
			{
				// FIle not exists or cannot open file
				return 0;
			}

			{
				auto fileCloseFunc = [asset] { AAsset_close(asset); };
				FINALLY_OF_BLOCK(_FILE_CLOSE, fileCloseFunc);

				if (offset > 0)
				{
					if (AAsset_seek(asset, (off_t)offset, SEEK_SET) != (off_t)-1)
					{
						// Invalid offset
						return 0;
					}
				}

				off_t remainSize = AAsset_getRemainingLength(asset);
				if (remainSize <= 0)
				{
					// Not need to read
					return 0;
				}

				char rawBuffer[_Internal::kAndroidFileReadBufferSize];
				std::ostream::pos_type beforePos = buffer.tellp();
				int currentReadSize;
				while ((currentReadSize = AAsset_read(asset, rawBuffer, _Internal::kAndroidFileReadBufferSize)) > 0)
				{
					buffer.write(rawBuffer, currentReadSize);
				}

				return buffer.tellp() - beforePos;
			}
		}

		//------------------------------------------------------------------------------
		bool AndroidBundleFileResourceComponent::IsDirectory(const std::string &virtualPath)
		{
			std::string effectiveVirtualPath = Utility::TrimTopAbsoluteDelimiter(virtualPath);

			AAsset *asset = AAssetManager_open(_Internal::g_androidAssetManager, effectiveVirtualPath.c_str(), AASSET_MODE_UNKNOWN);
			if (asset != NULL)
			{
				// File
				AAsset_close(asset);
				return false;
			}

			std::string parentPath = Utility::GetParent(effectiveVirtualPath);
			if (parentPath == effectiveVirtualPath)
			{
				// Top-Level Directory
				return true;
			}

			std::string targetName = Utility::GetName(effectiveVirtualPath);
			
			JNIEnv *pEnv = Platform::Android::GetJNIEnv();
			if (pEnv == nullptr)
			{
				// Something Error
				return false;
			}

			jclass fileSystemHelperClass = Platform::Android::FindJavaClass(pEnv, "com/leggiero/Engine/Modules/FileSystem/FileSystemHelper");
			if (!fileSystemHelperClass)
			{
				// Something Error
				return false;
			}

			jmethodID listMethod = pEnv->GetStaticMethodID(fileSystemHelperClass, "ListInAssetPath", "(Ljava/lang/String;)Ljava/util/ArrayList;");
			if (!listMethod)
			{
				pEnv->DeleteLocalRef(fileSystemHelperClass);
				return false;
			}

			jstring pathArgument = pEnv->NewStringUTF(parentPath.c_str());
			jobject resultList = pEnv->CallStaticObjectMethod(fileSystemHelperClass, listMethod, pathArgument);
			pEnv->DeleteLocalRef(pathArgument);
			pEnv->DeleteLocalRef(fileSystemHelperClass);

			if (resultList == nullptr)
			{
				return false;
			}

			std::vector<std::string> resultCppList = Platform::Android::GetStringList(pEnv, resultList);

			pEnv->DeleteLocalRef(resultList);

			for (const std::string &currentItem : resultCppList)
			{
				if (currentItem == targetName)
				{
					// Exist. Whether is file is already checked.
					return true;
				}
			}

			// Not existing path
			return false;
		}

		//------------------------------------------------------------------------------
		std::vector<std::string> AndroidBundleFileResourceComponent::ListSubDirectories(const std::string &virtualPath)
		{
			std::string effectiveVirtualPath = Utility::TrimTopAbsoluteDelimiter(virtualPath);

			JNIEnv *pEnv = Platform::Android::GetJNIEnv();
			if (pEnv == nullptr)
			{
				return std::vector<std::string>();
			}

			jclass fileSystemHelperClass = Platform::Android::FindJavaClass(pEnv, "com/leggiero/Engine/Modules/FileSystem/FileSystemHelper");
			if (!fileSystemHelperClass)
			{
				return std::vector<std::string>();
			}

			jmethodID listMethod = pEnv->GetStaticMethodID(fileSystemHelperClass, "ListInAssetPath", "(Ljava/lang/String;)Ljava/util/ArrayList;");
			if (!listMethod)
			{
				pEnv->DeleteLocalRef(fileSystemHelperClass);
				return std::vector<std::string>();
			}

			jstring pathArgument = pEnv->NewStringUTF(effectiveVirtualPath.c_str());
			jobject resultList = pEnv->CallStaticObjectMethod(fileSystemHelperClass, listMethod, pathArgument);
			pEnv->DeleteLocalRef(pathArgument);
			pEnv->DeleteLocalRef(fileSystemHelperClass);

			if (resultList == nullptr)
			{
				return std::vector<std::string>();
			}

			std::vector<std::string> resultCppList = Platform::Android::GetStringList(pEnv, resultList);

			pEnv->DeleteLocalRef(resultList);

			std::vector<std::string> resultDirectoryList;
			resultDirectoryList.reserve(resultCppList.size());
			for (const std::string &currentItem : resultCppList)
			{
				AAsset *asset = AAssetManager_open(_Internal::g_androidAssetManager, Utility::CombinePath(effectiveVirtualPath, currentItem).c_str(), AASSET_MODE_UNKNOWN);
				if (asset != NULL)
				{
					AAsset_close(asset);
				}
				else
				{
					resultDirectoryList.push_back(currentItem);
				}
			}

			return resultDirectoryList;
		}

		//------------------------------------------------------------------------------
		std::vector<std::string> AndroidBundleFileResourceComponent::ListFiles(const std::string &virtualPath)
		{
			std::string effectiveVirtualPath = Utility::TrimTopAbsoluteDelimiter(virtualPath);

			AAssetDir *assetDir = AAssetManager_openDir(_Internal::g_androidAssetManager, effectiveVirtualPath.c_str());
			if (assetDir == NULL)
			{
				return std::vector<std::string>();
			}

			std::vector<std::string> resultBuffer;
			const char *currentFileName;
			while ((currentFileName = AAssetDir_getNextFileName(assetDir)) != NULL)
			{
				resultBuffer.emplace_back(currentFileName);
			}

			AAssetDir_close(assetDir);

			return resultBuffer;
		}


		//////////////////////////////////////////////////////////////////////////////// BundleFileResourceComponent - Creation Function

		//------------------------------------------------------------------------------
		BundleFileResourceComponent *BundleFileResourceComponent::CreateComponentObject()
		{
			return new AndroidBundleFileResourceComponent();
		}
	}
}
