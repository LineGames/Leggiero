////////////////////////////////////////////////////////////////////////////////
// _Internal/_AndroidFileSystemPathComponent.h (Leggiero/Modules - FileSystem)
//
// Android Platform's File System Path Component
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_FILESYSTEM___INTERNAL__ANDROID_FILE_SYSTEM_PATH_COMPONENT_H
#define __LM_FILESYSTEM___INTERNAL__ANDROID_FILE_SYSTEM_PATH_COMPONENT_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.FileSystem
#include "../FileSystemPathComponent.h"


namespace Leggiero
{
	namespace FileSystem
	{
		// Bundle File Resource Component
		class AndroidFileSystemPathComponent
			: public FileSystemPathComponent
		{
		public:
			AndroidFileSystemPathComponent();
			virtual ~AndroidFileSystemPathComponent();

		public:	// FileSystemPathComponent
			// Data path for normal data which can do cloud backup if it is possible
			virtual std::string MainDataPath() override;
			virtual std::string MainDataPath(const std::string &virtualPath) override;

			// Data path for normal data without backup
			virtual std::string RawDataPath() override;
			virtual std::string RawDataPath(const std::string &virtualPath) override;

			// Data path for cache data
			// Can be erased by cache deletion
			virtual std::string CachePath() override;
			virtual std::string CachePath(const std::string &virtualPath) override;

			// Data path for temporary files
			virtual std::string TempPath() override;
			virtual std::string TempPath(const std::string &virtualPath) override;

		public:	// NPO::INonPortableOptimization_FileSystemPathComponent
			// Check whether if separated external storage is available or not
			// Usually, only works for Android
			virtual bool NPO_IsExternalStorageAvailable() override;

			// Get tuple of free space size of main / external storage each
			virtual std::tuple<size_t, size_t> NPO_GetFreeSpaceMainExternalStorages() override;

			// Data path for external storage of normal data which can do cloud backup if it is possible
			virtual std::string NPO_ExternalMainDataPath() override;
			virtual std::string NPO_ExternalMainDataPath(const std::string &virtualPath) override;

			// Data path for external storage of normal data without backup
			virtual std::string NPO_ExternalRawDataPath() override;
			virtual std::string NPO_ExternalRawDataPath(const std::string &virtualPath) override;

			// Data path for external storage of cache data
			virtual std::string NPO_ExternalCachePath() override;
			virtual std::string NPO_ExternalCachePath(const std::string &virtualPath) override;

		protected:
			std::string m_mainDataRealPath;
			std::string m_rawDataRealPath;
			std::string m_cacheRealPath;
			std::string m_tempRealPath;

			std::string m_externalMainDataRealPath;
			std::string m_externalRawDataRealPath;
			std::string m_externalCacheRealPath;
		};
	}
}

#endif
