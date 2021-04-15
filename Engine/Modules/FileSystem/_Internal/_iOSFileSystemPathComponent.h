////////////////////////////////////////////////////////////////////////////////
// _Internal/_iOSFileSystemPathComponent.h (Leggiero/Modules - FileSystem)
//
// iOS Platform's File System Path Component
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_FILESYSTEM___INTERNAL__IOS_FILE_SYSTEM_PATH_COMPONENT_H
#define __LM_FILESYSTEM___INTERNAL__IOS_FILE_SYSTEM_PATH_COMPONENT_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.FileSystem
#include "../FileSystemPathComponent.h"
#include "../FileSystemUtility.h"


namespace Leggiero
{
	namespace FileSystem
	{
		// Bundle File Resource Component
		class IOSFileSystemPathComponent
			: public FileSystemPathComponent
		{
		public:
            IOSFileSystemPathComponent();
			virtual ~IOSFileSystemPathComponent();

		public:	// EngineComponent
			// Initialize the Component
			virtual void InitializeComponent(Engine::GameProcessAnchor *gameAnchor) override;

		public:	// FileSystemPathComponent
			// Data path for normal data which can do cloud backup if it is possible
			virtual std::string MainDataPath() override { return m_mainDataRealPath; }
			virtual std::string MainDataPath(const std::string &virtualPath) override { return Utility::CombinePath(m_mainDataRealPath, virtualPath); }

			// Data path for normal data without backup
			virtual std::string RawDataPath() override { return m_rawDataRealPath; }
			virtual std::string RawDataPath(const std::string &virtualPath) override { return Utility::CombinePath(m_rawDataRealPath, virtualPath); }

			// Data path for cache data
			// Can be erased by cache deletion
			virtual std::string CachePath() override { return m_cacheRealPath; }
			virtual std::string CachePath(const std::string &virtualPath) override { return Utility::CombinePath(m_cacheRealPath, virtualPath); }

			// Data path for temporary files
			virtual std::string TempPath() override { return m_tempRealPath; }
			virtual std::string TempPath(const std::string &virtualPath) override { return Utility::CombinePath(m_tempRealPath, virtualPath); }

		public:	// NPO::INonPortableOptimization_FileSystemPathComponent
			// Data path for external storage of normal data which can do cloud backup if it is possible
			virtual std::string NPO_ExternalMainDataPath() override { return m_externalMainDataRealPath; }
			virtual std::string NPO_ExternalMainDataPath(const std::string &virtualPath) override { return Utility::CombinePath(m_externalMainDataRealPath, virtualPath); }

			// Data path for external storage of normal data without backup
			virtual std::string NPO_ExternalRawDataPath() override { return m_externalRawDataRealPath; }
			virtual std::string NPO_ExternalRawDataPath(const std::string &virtualPath) override { return Utility::CombinePath(m_externalRawDataRealPath, virtualPath); }

			// Data path for external storage of cache data
			virtual std::string NPO_ExternalCachePath() override { return m_externalCacheRealPath; }
			virtual std::string NPO_ExternalCachePath(const std::string &virtualPath) override { return Utility::CombinePath(m_externalCacheRealPath, virtualPath); }

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
