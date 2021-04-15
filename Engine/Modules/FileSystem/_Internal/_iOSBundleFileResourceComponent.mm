////////////////////////////////////////////////////////////////////////////////
// _Internal/_iOSBundleFileResourceComponent.mm (Leggiero/Modules - FileSystem)
//
// iOS Platform Bundle FIle Resource Component Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "_iOSBundleFileResourceComponent.h"

// Standrad Library
#include <cstdio>
#include <fstream>
#include <set>

// System Library
#import <Foundation/Foundation.h>

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>

// Leggiero.FileSystem
#include "../FileSystemUtility.h"


namespace Leggiero
{
	namespace FileSystem
	{
		//////////////////////////////////////////////////////////////////////////////// IOSBundleFileResourceComponent

		//------------------------------------------------------------------------------
        IOSBundleFileResourceComponent::IOSBundleFileResourceComponent()
		{
		}

		//------------------------------------------------------------------------------
        IOSBundleFileResourceComponent::~IOSBundleFileResourceComponent()
		{
		}
    
		//------------------------------------------------------------------------------
		bool IOSBundleFileResourceComponent::IsBundleFileExists(const std::string &virtualPath)
		{
            std::string resourcePath = _GetBundleResourceFilePath(virtualPath);
            return !resourcePath.empty();
		}

		//------------------------------------------------------------------------------
		size_t IOSBundleFileResourceComponent::GetBundleFileLength(const std::string &virtualPath)
		{
			std::string resourcePath = _GetBundleResourceFilePath(virtualPath);
			if (FILE *file = fopen(resourcePath.c_str(), "rb"))
			{
				fseek(file, 0, SEEK_END);
				size_t size = ftell(file);
				fclose(file);
				return size;
			}
			return 0;
		}

		//------------------------------------------------------------------------------
		size_t IOSBundleFileResourceComponent::ReadBundleFileData(const std::string &virtualPath, size_t offset, char *buffer, size_t bufferSize)
		{
			if (buffer == NULL || bufferSize <= 0)
			{
				// Invalid Argument
				return 0;
			}

			std::string resourcePath = _GetBundleResourceFilePath(virtualPath);
			FILE *file = fopen(resourcePath.c_str(), "rb");
			if (file == NULL)
			{
				// FIle not exists or cannot open file
				return 0;
			}

			{
				auto fileCloseFunc = [file] { fclose(file); };
				FINALLY_OF_BLOCK(_FILE_CLOSE, fileCloseFunc);

				if (offset > 0)
				{
					if (fseek(file, (long)offset, SEEK_SET) != 0)
					{
						// Invalid offset
						return 0;
					}
				}

				return fread(buffer, 1, bufferSize, file);
			}
		}

		//------------------------------------------------------------------------------
		std::streamoff IOSBundleFileResourceComponent::ReadBundleFileData(const std::string &virtualPath, std::streamoff offset, std::ostream &buffer)
		{
			std::ifstream fs(_GetBundleResourceFilePath(virtualPath), std::ios::binary);
			if (!fs.good())
			{
				return 0;
			}

			if (offset > 0)
			{
				fs.seekg(offset, std::ios_base::beg);
				if (!fs.good())
				{
					return 0;
				}
			}
			std::ostream::pos_type beforePos = buffer.tellp();
			buffer << fs.rdbuf();
			fs.close();

			return buffer.tellp() - beforePos;
		}
		
		//------------------------------------------------------------------------------
		bool IOSBundleFileResourceComponent::IsDirectory(const std::string &virtualPath)
		{
			NSString *bundleResourcePath = [[NSBundle mainBundle] resourcePath];
			
			std::string sanitizedVirtualPath = Utility::FilterDelimiter(virtualPath);
			
			// Search in Overriding Bundle
            std::string overridingBundleDirectoryPath;
            if (sanitizedVirtualPath.empty())
            {
                overridingBundleDirectoryPath = "iOS";
            }
            else
            {
                overridingBundleDirectoryPath = "iOS/" + sanitizedVirtualPath;
            }
			
			NSArray *checkOverridingPathComponents = [NSArray arrayWithObjects:bundleResourcePath, [NSString stringWithUTF8String:overridingBundleDirectoryPath.c_str()], nil];
			NSString *checkOverridingPathString = [NSString pathWithComponents:checkOverridingPathComponents];
            
            id fileManager = [NSFileManager defaultManager];
			
			BOOL isDir;
			if ([fileManager fileExistsAtPath:checkOverridingPathString isDirectory:&isDir] == YES)
			{
				return (isDir == YES);
			}
			
            // Search in Base Bundle
            std::string baseBundleDirectoryPath;
            if (sanitizedVirtualPath.empty())
            {
                baseBundleDirectoryPath = "Base";
            }
            else
            {
                baseBundleDirectoryPath = "Base/" + sanitizedVirtualPath;
            }
            
            NSArray *checkBasePathComponents = [NSArray arrayWithObjects:bundleResourcePath, [NSString stringWithUTF8String:baseBundleDirectoryPath.c_str()], nil];
			NSString *checkBasePath = [NSString pathWithComponents:checkBasePathComponents];
			
			if ([fileManager fileExistsAtPath:checkBasePath isDirectory:&isDir] == YES)
			{
				return (isDir == YES);
			}
			
			return false;
		}

		//------------------------------------------------------------------------------
		std::vector<std::string> IOSBundleFileResourceComponent::ListSubDirectories(const std::string &virtualPath)
		{
			std::set<std::string> directorySet;
			
			NSString *bundleResourcePath = [[NSBundle mainBundle] resourcePath];
			
			std::string sanitizedVirtualPath = Utility::FilterDelimiter(virtualPath);
			
			// Search in Overriding Bundle
            std::string overridingBundleDirectoryPath;
            if (sanitizedVirtualPath.empty())
            {
                overridingBundleDirectoryPath = "iOS";
            }
            else
            {
                overridingBundleDirectoryPath = "iOS/" + sanitizedVirtualPath;
            }
			
			NSArray *checkOverridingPathComponents = [NSArray arrayWithObjects:bundleResourcePath, [NSString stringWithUTF8String:overridingBundleDirectoryPath.c_str()], nil];
			NSString *checkOverridingPathString = [NSString pathWithComponents:checkOverridingPathComponents];
            
            id fileManager = [NSFileManager defaultManager];
			
			BOOL isDir;
			if ([fileManager fileExistsAtPath:checkOverridingPathString isDirectory:&isDir] == YES)
			{
				if (isDir == NO)
				{
					// Overrided by file
					return std::vector<std::string>();
				}
				
				NSArray *directoryContents = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:checkOverridingPathString error:NULL];
				const size_t itemCount = static_cast<size_t>([directoryContents count]);
				for (int i = 0; i < itemCount; ++i)
				{
					NSString *currentName = (NSString *)[directoryContents objectAtIndex:i];
					NSArray *checkPathComponents = [NSArray arrayWithObjects:checkOverridingPathString, currentName, nil];
					NSString *currentPath = [NSString pathWithComponents:checkPathComponents];
					
					BOOL isItemDir;
					if ([fileManager fileExistsAtPath:currentPath isDirectory:&isItemDir] == YES)
					{
						if (isItemDir == YES)
						{
							std::string currentNameCppString([currentName UTF8String]);
							directorySet.insert(currentNameCppString);
						}
					}
				}
			}
			
			// Search in Base Bundle
            std::string baseBundleDirectoryPath;
            if (sanitizedVirtualPath.empty())
            {
                baseBundleDirectoryPath = "Base";
            }
            else
            {
                baseBundleDirectoryPath = "Base/" + sanitizedVirtualPath;
            }
            
            NSArray *checkBasePathComponents = [NSArray arrayWithObjects:bundleResourcePath, [NSString stringWithUTF8String:baseBundleDirectoryPath.c_str()], nil];
			NSString *checkBasePath = [NSString pathWithComponents:checkBasePathComponents];
			
			if ([fileManager fileExistsAtPath:checkBasePath isDirectory:&isDir] == YES)
			{
				if (isDir == YES)
				{
					NSArray *directoryContents = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:checkBasePath error:NULL];
					const size_t itemCount = static_cast<size_t>([directoryContents count]);
					for (int i = 0; i < itemCount; ++i)
					{
						NSString *currentName = (NSString *)[directoryContents objectAtIndex:i];
						NSArray *checkPathComponents = [NSArray arrayWithObjects:checkBasePath, currentName, nil];
						NSString *currentPath = [NSString pathWithComponents:checkPathComponents];
						
						BOOL isItemDir;
						if ([fileManager fileExistsAtPath:currentPath isDirectory:&isItemDir] == YES)
						{
							if (isItemDir == YES)
							{
								std::string currentNameCppString([currentName UTF8String]);
								directorySet.insert(currentNameCppString);
							}
						}
					}
				}
			}

			return std::vector<std::string>(directorySet.begin(), directorySet.end());
		}

		//------------------------------------------------------------------------------
		std::vector<std::string> IOSBundleFileResourceComponent::ListFiles(const std::string &virtualPath)
		{
			std::set<std::string> fileSet;
			
			NSString *bundleResourcePath = [[NSBundle mainBundle] resourcePath];
			
			std::string sanitizedVirtualPath = Utility::FilterDelimiter(virtualPath);
			
			// Search in Overriding Bundle
            std::string overridingBundleDirectoryPath;
            if (sanitizedVirtualPath.empty())
            {
                overridingBundleDirectoryPath = "iOS";
            }
            else
            {
                overridingBundleDirectoryPath = "iOS/" + sanitizedVirtualPath;
            }
			
			NSArray *checkOverridingPathComponents = [NSArray arrayWithObjects:bundleResourcePath, [NSString stringWithUTF8String:overridingBundleDirectoryPath.c_str()], nil];
			NSString *checkOverridingPathString = [NSString pathWithComponents:checkOverridingPathComponents];
            
            id fileManager = [NSFileManager defaultManager];
			
			BOOL isDir;
			if ([fileManager fileExistsAtPath:checkOverridingPathString isDirectory:&isDir] == YES)
			{
				if (isDir == NO)
				{
					// Overrided by file
					return std::vector<std::string>();
				}
				
				NSArray *directoryContents = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:checkOverridingPathString error:NULL];
				const size_t itemCount = static_cast<size_t>([directoryContents count]);
				for (int i = 0; i < itemCount; ++i)
				{
					NSString *currentName = (NSString *)[directoryContents objectAtIndex:i];
					NSArray *checkPathComponents = [NSArray arrayWithObjects:checkOverridingPathString, currentName, nil];
					NSString *currentPath = [NSString pathWithComponents:checkPathComponents];
					
					BOOL isItemDir;
					if ([fileManager fileExistsAtPath:currentPath isDirectory:&isItemDir] == YES)
					{
						if (isItemDir == NO)
						{
							std::string currentNameCppString([currentName UTF8String]);
							fileSet.insert(currentNameCppString);
						}
					}
				}
			}
			
			// Search in Base Bundle
            std::string baseBundleDirectoryPath;
            if (sanitizedVirtualPath.empty())
            {
                baseBundleDirectoryPath = "Base";
            }
            else
            {
                baseBundleDirectoryPath = "Base/" + sanitizedVirtualPath;
            }
            
            NSArray *checkBasePathComponents = [NSArray arrayWithObjects:bundleResourcePath, [NSString stringWithUTF8String:baseBundleDirectoryPath.c_str()], nil];
			NSString *checkBasePath = [NSString pathWithComponents:checkBasePathComponents];
			
			if ([fileManager fileExistsAtPath:checkBasePath isDirectory:&isDir] == YES)
			{
				if (isDir == YES)
				{
					NSArray *directoryContents = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:checkBasePath error:NULL];
					const size_t itemCount = static_cast<size_t>([directoryContents count]);
					for (int i = 0; i < itemCount; ++i)
					{
						NSString *currentName = (NSString *)[directoryContents objectAtIndex:i];
						NSArray *checkPathComponents = [NSArray arrayWithObjects:checkBasePath, currentName, nil];
						NSString *currentPath = [NSString pathWithComponents:checkPathComponents];
						
						BOOL isItemDir;
						if ([fileManager fileExistsAtPath:currentPath isDirectory:&isItemDir] == YES)
						{
							if (isItemDir == NO)
							{
								std::string currentNameCppString([currentName UTF8String]);
								fileSet.insert(currentNameCppString);
							}
						}
					}
				}
			}

			return std::vector<std::string>(fileSet.begin(), fileSet.end());
		}

		//------------------------------------------------------------------------------
		std::string IOSBundleFileResourceComponent::_GetBundleResourceFilePath(const std::string &virtualPath) const
		{
            std::string sanitizedVirtualPath = Utility::FilterDelimiter(virtualPath);
            
            // Get Directory
            std::string fileParentVirtualPath;
            std::string fileNameWithExt(sanitizedVirtualPath);
            size_t lastFolderSplit = sanitizedVirtualPath.find_last_of('/');
            if (lastFolderSplit != std::string::npos)
            {
                fileParentVirtualPath = sanitizedVirtualPath.substr(0, lastFolderSplit);
                fileNameWithExt = sanitizedVirtualPath.substr(lastFolderSplit + 1);
            }
            
            // Get File Name and Extension
            std::string fileName(fileNameWithExt);
            std::string fileExt;
            size_t lastDot = fileNameWithExt.find_last_of('.');
            if (lastDot != std::string::npos)
            {
                fileName = fileNameWithExt.substr(0, lastDot);
                fileExt = fileNameWithExt.substr(lastDot + 1);
            }
            
            NSString *fileNameString = [NSString stringWithUTF8String:fileName.c_str()];
            NSString *fileExtString = nil;
            if (!fileExt.empty())
            {
                fileExtString = [NSString stringWithUTF8String:fileExt.c_str()];
            }
            
            // Search in Overriding Bundle
            std::string overridingBundleDirectoryPath;
            if (fileParentVirtualPath.empty())
            {
                overridingBundleDirectoryPath = "iOS";
            }
            else
            {
                overridingBundleDirectoryPath = "iOS/" + fileParentVirtualPath;
            }
            
            id bundleResourcePath = [[NSBundle mainBundle] pathForResource:fileNameString ofType:fileExtString inDirectory:[NSString stringWithUTF8String:overridingBundleDirectoryPath.c_str()]];
            if (bundleResourcePath != nil)
            {
                return [bundleResourcePath UTF8String];
            }
            
            // Search in Base Bundle
            std::string baseBundleDirectoryPath;
            if (fileParentVirtualPath.empty())
            {
                baseBundleDirectoryPath = "Base";
            }
            else
            {
                baseBundleDirectoryPath = "Base/" + fileParentVirtualPath;
            }
            
            bundleResourcePath = [[NSBundle mainBundle] pathForResource:fileNameString ofType:fileExtString inDirectory:[NSString stringWithUTF8String:baseBundleDirectoryPath.c_str()]];
            if (bundleResourcePath != nil)
            {
                return [bundleResourcePath UTF8String];
            }
            
            // File Not Found
            return "";
		}


		//////////////////////////////////////////////////////////////////////////////// BundleFileResourceComponent - Creation Function

		//------------------------------------------------------------------------------
		BundleFileResourceComponent *BundleFileResourceComponent::CreateComponentObject()
		{
			return new IOSBundleFileResourceComponent();
		}
	}
}
