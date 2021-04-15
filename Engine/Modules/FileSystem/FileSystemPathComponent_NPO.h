////////////////////////////////////////////////////////////////////////////////
// FileSystemPathComponent_NPO.h (Leggiero/Modules - FileSystem)
//
// Non-Portable Optimization Interface for File System Path Component
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_FILESYSTEM__FILE_SYSTEM_PATH_COMPONENT_NPO_H
#define __LM_FILESYSTEM__FILE_SYSTEM_PATH_COMPONENT_NPO_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <string>
#include <tuple>


namespace Leggiero
{
	namespace FileSystem
	{
		namespace NPO
		{
			// Non-Portable Interface for File System Path
			class INonPortableOptimization_FileSystemPathComponent
			{
			public:
				// Check whether if separated external storage is available or not
				// Usually, only works for Android
				virtual bool NPO_IsExternalStorageAvailable() { return false; }

				// Get tuple of free space size of main / external storage each
				virtual std::tuple<size_t, size_t> NPO_GetFreeSpaceMainExternalStorages() { return std::make_tuple<size_t, size_t>(0, 0); }

				// Data path for external storage of normal data which can do cloud backup if it is possible
				virtual std::string NPO_ExternalMainDataPath() { return g_EmptyGameDataString; }
				virtual std::string NPO_ExternalMainDataPath(const std::string &virtualPath) { return virtualPath; }

				// Data path for external storage of normal data without backup
				virtual std::string NPO_ExternalRawDataPath() { return g_EmptyGameDataString; }
				virtual std::string NPO_ExternalRawDataPath(const std::string &virtualPath) { return virtualPath; }

				// Data path for external storage of cache data
				virtual std::string NPO_ExternalCachePath() { return g_EmptyGameDataString; }
				virtual std::string NPO_ExternalCachePath(const std::string &virtualPath) { return virtualPath; }
			};
		}
	}
}

#endif
