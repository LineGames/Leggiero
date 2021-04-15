////////////////////////////////////////////////////////////////////////////////
// FileSystemUtility_WinPC.cpp (Leggiero/Modules - FileSystem)
//
// WinPC Platform Implementation of File System Utilities
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "FileSystemUtility.h"

// Leggiero.Platform.WinPC
#include <WindowsUtility.h>


namespace Leggiero
{
	namespace FileSystem
	{
		namespace Utility
		{
			//------------------------------------------------------------------------------
			const char kPathDelimiter = '\\';

			//------------------------------------------------------------------------------
			// Check existence of the directory and create if not exists
			void PrepareDirectoryPath(const std::string &path)
			{
				size_t pathLastSeparator = path.find_last_of('\\');
				if (pathLastSeparator != std::string::npos)
				{
					PrepareDirectoryPath(path.substr(0, pathLastSeparator));
				}

				std::wstring wPath(path.begin(), path.end());

				DWORD dwAttrib = GetFileAttributes(wPath.c_str());
				if (dwAttrib == INVALID_FILE_ATTRIBUTES || !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
				{
					CreateDirectory(wPath.c_str(), NULL);
				}
			}

			//------------------------------------------------------------------------------
			bool IsDirectory(const std::string &path)
			{
				std::wstring wPath(path.begin(), path.end());
				DWORD dwAttrib = GetFileAttributes(wPath.c_str());
				return ((dwAttrib & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
			}

			//------------------------------------------------------------------------------
			std::vector<GameDataString> ListSubDirectories(const std::string &path)
			{
				std::wstring wPath(path.begin(), path.end());

				DWORD dwAttrib = GetFileAttributes(wPath.c_str());
				if (dwAttrib == INVALID_FILE_ATTRIBUTES || !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
				{
					return std::vector<GameDataString>();
				}

				wPath += TEXT("\\*");

				WIN32_FIND_DATA ffd;
				HANDLE hFind = FindFirstFile(wPath.c_str(), &ffd);
				if (hFind == INVALID_HANDLE_VALUE)
				{
					return std::vector<GameDataString>();
				}

				std::vector<GameDataString> resultBuffer;
				do
				{
					if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
					{
						GameDataString currentDirName = Platform::Windows::Utility::WSTR_2_GameDataString(ffd.cFileName);
						if (currentDirName == "." || currentDirName == "..")
						{
							continue;
						}
						resultBuffer.push_back(currentDirName);
					}
				} while (FindNextFile(hFind, &ffd) != 0);
				FindClose(hFind);

				return resultBuffer;
			}

			//------------------------------------------------------------------------------
			std::vector<GameDataString> ListFiles(const std::string &path)
			{
				std::wstring wPath(path.begin(), path.end());

				DWORD dwAttrib = GetFileAttributes(wPath.c_str());
				if (dwAttrib == INVALID_FILE_ATTRIBUTES || !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
				{
					return std::vector<GameDataString>();
				}

				wPath += TEXT("\\*");

				WIN32_FIND_DATA ffd;
				HANDLE hFind = FindFirstFile(wPath.c_str(), &ffd);
				if (hFind == INVALID_HANDLE_VALUE)
				{
					return std::vector<GameDataString>();
				}

				std::vector<GameDataString> resultBuffer;
				do
				{
					if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
					{
						resultBuffer.push_back(Platform::Windows::Utility::WSTR_2_GameDataString(ffd.cFileName));
					}
				} while (FindNextFile(hFind, &ffd) != 0);
				FindClose(hFind);

				return resultBuffer;
			}
		}
	}
}
