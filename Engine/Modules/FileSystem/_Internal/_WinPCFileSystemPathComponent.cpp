////////////////////////////////////////////////////////////////////////////////
// _Internal/_WinPCFileSystemPathComponent.cpp (Leggiero/Modules - FileSystem)
//
// WinPC Platform File System Path Component Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "_WinPCFileSystemPathComponent.h"

// Standrad Library
#include <cstdio>
#include <fstream>

// External Library
#include <tinyxml2/tinyxml2.h>

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>


namespace Leggiero
{
	namespace FileSystem
	{
		//////////////////////////////////////////////////////////////////////////////// Internal Setting and Utility

		namespace _Internal
		{
			static const char kWinPCPathSettingFile[] = "WinPath.xml";
		}


		//////////////////////////////////////////////////////////////////////////////// WinPCFileSystemPathComponent

		//------------------------------------------------------------------------------
		WinPCFileSystemPathComponent::WinPCFileSystemPathComponent()
		{
		}

		//------------------------------------------------------------------------------
		WinPCFileSystemPathComponent::~WinPCFileSystemPathComponent()
		{
		}

		//------------------------------------------------------------------------------
		void WinPCFileSystemPathComponent::InitializeComponent(Engine::GameProcessAnchor *gameAnchor)
		{
			tinyxml2::XMLDocument pathSettingDoc;
			if (pathSettingDoc.LoadFile(_Internal::kWinPCPathSettingFile) != tinyxml2::XMLError::XML_SUCCESS)
			{
				return;
			}
			tinyxml2::XMLElement *rootElem = pathSettingDoc.RootElement();
			if (rootElem == NULL)
			{
				return;
			}

			// Initialize Path
			tinyxml2::XMLElement *mainDataPathElem = rootElem->FirstChildElement("MainData");
			if (mainDataPathElem != NULL)
			{
				m_mainDataRealPath = Utility::FilterDelimiter(mainDataPathElem->Attribute("internal"), true);
				m_externalMainDataRealPath = Utility::FilterDelimiter(mainDataPathElem->Attribute("external"), true);
			}

			tinyxml2::XMLElement *rawDataPathElem = rootElem->FirstChildElement("RawData");
			if (rawDataPathElem != NULL)
			{
				m_rawDataRealPath = Utility::FilterDelimiter(rawDataPathElem->Attribute("internal"), true);
				m_externalRawDataRealPath = Utility::FilterDelimiter(rawDataPathElem->Attribute("external"), true);
			}

			tinyxml2::XMLElement *cachePathElem = rootElem->FirstChildElement("Cache");
			if (cachePathElem != NULL)
			{
				m_cacheRealPath = Utility::FilterDelimiter(cachePathElem->Attribute("internal"), true);
				m_externalCacheRealPath = Utility::FilterDelimiter(cachePathElem->Attribute("external"), true);
			}

			tinyxml2::XMLElement *tempPathElem = rootElem->FirstChildElement("Temp");
			if (tempPathElem != NULL)
			{
				m_tempRealPath = Utility::FilterDelimiter(tempPathElem->Attribute("root"), true);
			}

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
			return new WinPCFileSystemPathComponent();
		}
	}
}
