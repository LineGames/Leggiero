////////////////////////////////////////////////////////////////////////////////
// _Internal/_iOSFileSystemPathComponent.mm (Leggiero/Modules - FileSystem)
//
// iOS Platform File System Path Component Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "_iOSFileSystemPathComponent.h"

// Standrad Library
#include <cstdio>
#include <fstream>

// System Library
#import <Foundation/Foundation.h>

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>


namespace Leggiero
{
	namespace FileSystem
	{
		//////////////////////////////////////////////////////////////////////////////// WinPCFileSystemPathComponent

		//------------------------------------------------------------------------------
        IOSFileSystemPathComponent::IOSFileSystemPathComponent()
		{
		}

		//------------------------------------------------------------------------------
        IOSFileSystemPathComponent::~IOSFileSystemPathComponent()
		{
		}

		//------------------------------------------------------------------------------
		void IOSFileSystemPathComponent::InitializeComponent(Engine::GameProcessAnchor *gameAnchor)
		{
            id dirPaths;
            id currentDir;
            
            // Main
            dirPaths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
            currentDir = [dirPaths objectAtIndex:0];
            m_mainDataRealPath = Utility::CombinePath([currentDir UTF8String], "m");
            m_externalMainDataRealPath = Utility::CombinePath([currentDir UTF8String], "em");
            
            // Raw
            dirPaths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
            currentDir = [dirPaths objectAtIndex:0];
            m_rawDataRealPath = Utility::CombinePath([currentDir UTF8String], "r");
            m_externalRawDataRealPath = Utility::CombinePath([currentDir UTF8String], "er");
            
            // Cache
            dirPaths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
            currentDir = [dirPaths objectAtIndex:0];
            m_rawDataRealPath = Utility::CombinePath([currentDir UTF8String], "c");
            m_externalRawDataRealPath = Utility::CombinePath([currentDir UTF8String], "ec");
            
            // Temp
            m_tempRealPath = Utility::CombinePath([NSTemporaryDirectory() UTF8String], "t");

			// Create path directory if not exist
			Utility::PrepareDirectoryPath(m_mainDataRealPath);
			Utility::PrepareDirectoryPath(m_rawDataRealPath);
			Utility::PrepareDirectoryPath(m_cacheRealPath);
			Utility::PrepareDirectoryPath(m_tempRealPath);

			Utility::PrepareDirectoryPath(m_externalMainDataRealPath);
			Utility::PrepareDirectoryPath(m_externalRawDataRealPath);
			Utility::PrepareDirectoryPath(m_externalCacheRealPath);
		}


		//////////////////////////////////////////////////////////////////////////////// FileSystemPathComponent - Creation Function

		//------------------------------------------------------------------------------
		FileSystemPathComponent *FileSystemPathComponent::CreateComponentObject()
		{
			return new IOSFileSystemPathComponent();
		}
	}
}
