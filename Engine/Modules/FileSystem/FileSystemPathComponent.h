////////////////////////////////////////////////////////////////////////////////
// FileSystemPathComponent.h (Leggiero/Modules - FileSystem)
//
// Engine Component to provide real path(compatible with C/C++ std. libs)
// of categorized abstraction.
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_FILESYSTEM__FILE_SYSTEM_PATH_COMPONENT_H
#define __LM_FILESYSTEM__FILE_SYSTEM_PATH_COMPONENT_H


// Standard Library
#include <string>

// Leggiero.Engine
#include <Engine/Module/EngineComponent.h>
#include <Engine/Module/EngineComponentHolder.h>

// Leggiero.FileSystem
#include "FileSystemPathComponent_NPO.h"


namespace Leggiero
{
	namespace FileSystem
	{
		// File System Path Component
		class FileSystemPathComponent
			: public EngineComponent
			, public NPO::INonPortableOptimization_FileSystemPathComponent
		{
		public:	// Can be created by Creation Function (for each platform)
			static FileSystemPathComponent *CreateComponentObject();

		public:
			virtual ~FileSystemPathComponent();

		protected:
			FileSystemPathComponent();

		public:	// EngineComponent
			// Get Type Id of the Component
			virtual EngineComponentIdType GetComponentType() const override { return EngineComponentIdType::kFileSystemPath; };

		public:
			// Data path for normal data which can do cloud backup if it is possible
			virtual std::string MainDataPath() = 0;
			virtual std::string MainDataPath(const std::string &virtualPath) = 0;

			// Data path for normal data without backup
			virtual std::string RawDataPath() = 0;
			virtual std::string RawDataPath(const std::string &virtualPath) = 0;

			// Data path for cache data
			// Can be erased by cache deletion
			virtual std::string CachePath() = 0;
			virtual std::string CachePath(const std::string &virtualPath) = 0;

			// Data path for temporary files
			virtual std::string TempPath() = 0;
			virtual std::string TempPath(const std::string &virtualPath) = 0;
		};
	}
}


DECLARE_GET_COMPONENT_INTERFACE(Leggiero::FileSystem::FileSystemPathComponent, Leggiero::EngineComponentIdType::kFileSystemPath);

#endif
