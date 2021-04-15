////////////////////////////////////////////////////////////////////////////////
// FileSystemUtility_iOS.mm (Leggiero/Modules - FileSystem)
//
// iOS Platform Implementation of File System Utilities
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "FileSystemUtility.h"

// System Library
#import <Foundation/Foundation.h>


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
				
                id fileManager = [NSFileManager defaultManager];
                id currentPathString = [NSString stringWithUTF8String:path.c_str()];
                
                if ([fileManager fileExistsAtPath:currentPathString] == NO)
                {
                    [fileManager createDirectoryAtPath:currentPathString
                           withIntermediateDirectories:YES
                                            attributes:nil
                                                 error:nil];
                }
			}
			
			//------------------------------------------------------------------------------
			bool IsDirectory(const std::string &path)
			{
				if (path.empty())
                {
                    return false;
                }
				
                id fileManager = [NSFileManager defaultManager];
                id pathString = [NSString stringWithUTF8String:path.c_str()];
				
				BOOL isDir;
				if ([fileManager fileExistsAtPath:pathString isDirectory:&isDir] == NO)
                {
                    return false;
                }
				return (isDir == YES);
			}

			//------------------------------------------------------------------------------
			std::vector<GameDataString> ListSubDirectories(const std::string &path)
			{
				if (!IsDirectory(path))
				{
					return std::vector<GameDataString>();
				}
				
                id fileManager = [NSFileManager defaultManager];
				id pathString = [NSString stringWithUTF8String:path.c_str()];
				
				NSArray *directoryContents = [fileManager contentsOfDirectoryAtPath:pathString error:NULL];
				const size_t itemCount = static_cast<size_t>([directoryContents count]);
				std::vector<GameDataString> resultBuffer;
				resultBuffer.reserve(itemCount);
				for (int i = 0; i < itemCount; ++i)
				{
					NSString *currentName = (NSString *)[directoryContents objectAtIndex:i];
					NSArray *checkPathComponents = [NSArray arrayWithObjects:pathString, currentName, nil];
					NSString *currentPath = [NSString pathWithComponents:checkPathComponents];
					
					BOOL isDir;
					if ([fileManager fileExistsAtPath:currentPath isDirectory:&isDir] == YES)
					{
						if (isDir == YES)
						{
							resultBuffer.emplace_back([currentName UTF8String]);
						}
					}
				}
                return resultBuffer;
			}

			//------------------------------------------------------------------------------
			std::vector<GameDataString> ListFiles(const std::string &path)
			{
				if (!IsDirectory(path))
				{
					return std::vector<GameDataString>();
				}
				
                id fileManager = [NSFileManager defaultManager];
				id pathString = [NSString stringWithUTF8String:path.c_str()];
				
				NSArray *directoryContents = [fileManager contentsOfDirectoryAtPath:pathString error:NULL];
				const size_t itemCount = static_cast<size_t>([directoryContents count]);
				std::vector<GameDataString> resultBuffer;
				resultBuffer.reserve(itemCount);
				for (int i = 0; i < itemCount; ++i)
				{
					NSString *currentName = (NSString *)[directoryContents objectAtIndex:i];
					NSArray *checkPathComponents = [NSArray arrayWithObjects:pathString, currentName, nil];
					NSString *currentPath = [NSString pathWithComponents:checkPathComponents];
					
					BOOL isDir;
					if ([fileManager fileExistsAtPath:currentPath isDirectory:&isDir] == YES)
					{
						if (isDir == NO)
						{
							resultBuffer.emplace_back([currentName UTF8String]);
						}
					}
				}
                return resultBuffer;
			}
		}
	}
}
