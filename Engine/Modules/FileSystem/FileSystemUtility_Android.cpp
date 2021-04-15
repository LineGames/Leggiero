////////////////////////////////////////////////////////////////////////////////
// FileSystemUtility_Android.cpp (Leggiero/Modules - FileSystem)
//
// Android Platform Implementation of File System Utilities
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "FileSystemUtility.h"

// Leggiero.Platform.Android
#include <AndroidPlatform/AndroidJNISupport.h>


namespace Leggiero
{
	namespace FileSystem
	{
		namespace Utility
		{
			//------------------------------------------------------------------------------
			const char kPathDelimiter = '/';

			//------------------------------------------------------------------------------
			// Check existence of the directory and create if not exists
			void PrepareDirectoryPath(const std::string &path)
			{
				if (path.empty())
				{
					return;
				}

				JNIEnv *pEnv = Platform::Android::GetJNIEnv();
				if (pEnv == nullptr)
				{
					return;
				}

				jclass fileSystemHelperClass = Platform::Android::FindJavaClass(pEnv, "com/leggiero/Engine/Modules/FileSystem/FileSystemHelper");
				if (!fileSystemHelperClass)
				{
					return;
				}

				jmethodID directoryMethod = pEnv->GetStaticMethodID(fileSystemHelperClass, "CheckAndMakeDir", "(Ljava/lang/String;)V");
				if (!directoryMethod)
				{
					pEnv->DeleteLocalRef(fileSystemHelperClass);
					return;
				}

				jstring pathArgument = pEnv->NewStringUTF(path.c_str());
				pEnv->CallStaticVoidMethod(fileSystemHelperClass, directoryMethod, pathArgument);
				pEnv->DeleteLocalRef(pathArgument);

				pEnv->DeleteLocalRef(fileSystemHelperClass);
			}

			//------------------------------------------------------------------------------
			bool IsDirectory(const std::string &path)
			{
				if (path.empty())
				{
					return false;
				}

				JNIEnv *pEnv = Platform::Android::GetJNIEnv();
				if (pEnv == nullptr)
				{
					return false;
				}

				jclass fileSystemHelperClass = Platform::Android::FindJavaClass(pEnv, "com/leggiero/Engine/Modules/FileSystem/FileSystemHelper");
				if (!fileSystemHelperClass)
				{
					return false;
				}

				jmethodID directoryMethod = pEnv->GetStaticMethodID(fileSystemHelperClass, "CheckIsDirectory", "(Ljava/lang/String;)I");
				if (!directoryMethod)
				{
					pEnv->DeleteLocalRef(fileSystemHelperClass);
					return false;
				}

				jstring pathArgument = pEnv->NewStringUTF(path.c_str());
				jint resultValue = pEnv->CallStaticIntMethod(fileSystemHelperClass, directoryMethod, pathArgument);
				pEnv->DeleteLocalRef(pathArgument);

				pEnv->DeleteLocalRef(fileSystemHelperClass);

				return (resultValue == 1);
			}

			//------------------------------------------------------------------------------
			std::vector<GameDataString> ListSubDirectories(const std::string &path)
			{
				if (path.empty())
				{
					return std::vector<GameDataString>();
				}

				JNIEnv *pEnv = Platform::Android::GetJNIEnv();
				if (pEnv == nullptr)
				{
					return std::vector<GameDataString>();
				}

				jclass fileSystemHelperClass = Platform::Android::FindJavaClass(pEnv, "com/leggiero/Engine/Modules/FileSystem/FileSystemHelper");
				if (!fileSystemHelperClass)
				{
					return std::vector<GameDataString>();
				}

				jmethodID listMethod = pEnv->GetStaticMethodID(fileSystemHelperClass, "ListSubFiles", "(Ljava/lang/String;I)Ljava/util/ArrayList;");
				if (!listMethod)
				{
					pEnv->DeleteLocalRef(fileSystemHelperClass);
					return std::vector<GameDataString>();
				}

				jstring pathArgument = pEnv->NewStringUTF(path.c_str());
				jobject resultList = pEnv->CallStaticObjectMethod(fileSystemHelperClass, listMethod, pathArgument, 1);
				pEnv->DeleteLocalRef(pathArgument);
				pEnv->DeleteLocalRef(fileSystemHelperClass);

				if (resultList == nullptr)
				{
					return std::vector<GameDataString>();
				}

				std::vector<std::string> resultCppList = Platform::Android::GetStringList(pEnv, resultList);

				pEnv->DeleteLocalRef(resultList);

				return resultCppList;
			}

			//------------------------------------------------------------------------------
			std::vector<GameDataString> ListFiles(const std::string &path)
			{
				if (path.empty())
				{
					return std::vector<GameDataString>();
				}

				JNIEnv *pEnv = Platform::Android::GetJNIEnv();
				if (pEnv == nullptr)
				{
					return std::vector<GameDataString>();
				}

				jclass fileSystemHelperClass = Platform::Android::FindJavaClass(pEnv, "com/leggiero/Engine/Modules/FileSystem/FileSystemHelper");
				if (!fileSystemHelperClass)
				{
					return std::vector<GameDataString>();
				}

				jmethodID listMethod = pEnv->GetStaticMethodID(fileSystemHelperClass, "ListSubFiles", "(Ljava/lang/String;I)Ljava/util/ArrayList;");
				if (!listMethod)
				{
					pEnv->DeleteLocalRef(fileSystemHelperClass);
					return std::vector<GameDataString>();
				}

				jstring pathArgument = pEnv->NewStringUTF(path.c_str());
				jobject resultList = pEnv->CallStaticObjectMethod(fileSystemHelperClass, listMethod, pathArgument, 0);
				pEnv->DeleteLocalRef(pathArgument);
				pEnv->DeleteLocalRef(fileSystemHelperClass);

				if (resultList == nullptr)
				{
					return std::vector<GameDataString>();
				}

				std::vector<std::string> resultCppList = Platform::Android::GetStringList(pEnv, resultList);

				pEnv->DeleteLocalRef(resultList);

				return resultCppList;
			}
		}
	}
}
