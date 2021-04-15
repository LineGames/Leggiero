////////////////////////////////////////////////////////////////////////////////
// FileSystemUtility.h (Leggiero/Modules - FileSystem)
//
// Utilities for File System
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_FILESYSTEM__FILE_SYSTEM_UTILITY_H
#define __LM_FILESYSTEM__FILE_SYSTEM_UTILITY_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <string>
#include <vector>


namespace Leggiero
{
	namespace FileSystem
	{
		namespace Utility
		{
			extern const char kPathDelimiter;

			std::string FilterDelimiter(const std::string &path, bool makeEndWithDelimeter = false);

			std::string TrimTopAbsoluteDelimiter(const std::string &path);

			// Combine two parts as a path
			std::string CombinePath(const std::string &path1, const std::string &path2);

			std::string CombinePathWithResolvingParent(const std::string &path1, const std::string &path2, bool isGoBeyondTop = false);

			// Check existence of the directory and create if not exists
			void PrepareDirectoryPath(const std::string &path);

			std::string GetExtension(const std::string &path);

			std::string GetParent(const std::string &path);
			std::string GetName(const std::string &path, bool isIncludeExtension = true);

			bool IsDirectory(const std::string &path);

			std::vector<GameDataString> ListSubDirectories(const std::string &path);
			std::vector<GameDataString> ListFiles(const std::string &path);
		}
	}
}

#endif
