////////////////////////////////////////////////////////////////////////////////
// _Internal/_WinPCBundleFileResourceComponent.cpp (Leggiero/Modules - FileSystem)
//
// WinPC Platform Bundle FIle Resource Component Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "_WinPCBundleFileResourceComponent.h"

// Standrad Library
#include <cstdio>
#include <fstream>
#include <set>

// External Library
#include <tinyxml2/tinyxml2.h>

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>

// Leggiero.FileSystem
#include "../FileSystemUtility.h"


namespace Leggiero
{
	namespace FileSystem
	{
		//////////////////////////////////////////////////////////////////////////////// Internal Setting and Utility

		namespace _Internal
		{
			static const char kWinPCPathSettingFile[] = "WinPath.xml";
		}


		//////////////////////////////////////////////////////////////////////////////// WinPCBundleFileResourceComponent

		//------------------------------------------------------------------------------
		WinPCBundleFileResourceComponent::WinPCBundleFileResourceComponent()
		{
		}

		//------------------------------------------------------------------------------
		WinPCBundleFileResourceComponent::~WinPCBundleFileResourceComponent()
		{
		}

		//------------------------------------------------------------------------------
		void WinPCBundleFileResourceComponent::InitializeComponent(Engine::GameProcessAnchor *gameAnchor)
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

			// Initialize Bundle Path
			tinyxml2::XMLElement *bundlePathElem = rootElem->FirstChildElement("Bundle");
			if (bundlePathElem != NULL)
			{
				std::string bundleRoot = bundlePathElem->Attribute("root");
				m_baseBundleRealPath = Utility::CombinePath(bundleRoot, bundlePathElem->Attribute("base"));
				m_platformOverrideBundleRealPath = Utility::CombinePath(bundleRoot, bundlePathElem->Attribute("platform"));
			}
		}

		//------------------------------------------------------------------------------
		bool WinPCBundleFileResourceComponent::IsBundleFileExists(const std::string &virtualPath)
		{
			std::string platformResourcePath = Utility::CombinePath(m_platformOverrideBundleRealPath, virtualPath);
			if (FILE *file = fopen(platformResourcePath.c_str(), "rb"))
			{
				fclose(file);
				return true;
			}

			std::string baseResourcePath = Utility::CombinePath(m_baseBundleRealPath, virtualPath);
			if (FILE *file = fopen(baseResourcePath.c_str(), "rb"))
			{
				fclose(file);
				return true;
			}

			return false;
		}

		//------------------------------------------------------------------------------
		size_t WinPCBundleFileResourceComponent::GetBundleFileLength(const std::string &virtualPath)
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
		size_t WinPCBundleFileResourceComponent::ReadBundleFileData(const std::string &virtualPath, size_t offset, char *buffer, size_t bufferSize)
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
		std::streamoff WinPCBundleFileResourceComponent::ReadBundleFileData(const std::string &virtualPath, std::streamoff offset, std::ostream &buffer)
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
		bool WinPCBundleFileResourceComponent::IsDirectory(const std::string &virtualPath)
		{
			std::string effectiveRealPath = Utility::CombinePath(m_platformOverrideBundleRealPath, virtualPath);
			if (Utility::IsDirectory(effectiveRealPath))
			{
				return true;
			}
			else
			{
				if (FILE *file = fopen(effectiveRealPath.c_str(), "rb"))
				{
					fclose(file);
					return false;
				}
			}

			effectiveRealPath = Utility::CombinePath(m_baseBundleRealPath, virtualPath);
			if (Utility::IsDirectory(effectiveRealPath))
			{
				return true;
			}

			return false;
		}

		//------------------------------------------------------------------------------
		std::vector<std::string> WinPCBundleFileResourceComponent::ListSubDirectories(const std::string &virtualPath)
		{
			std::set<std::string> directorySet;

			std::string effectiveRealOverridePath = Utility::CombinePath(m_platformOverrideBundleRealPath, virtualPath);
			if (Utility::IsDirectory(effectiveRealOverridePath))
			{
				std::vector<std::string> overridedDirectoryList = Utility::ListSubDirectories(effectiveRealOverridePath);
				directorySet.insert(overridedDirectoryList.begin(), overridedDirectoryList.end());
			}
			else if (FILE *file = fopen(effectiveRealOverridePath.c_str(), "rb"))
			{
				// Overrided by file
				fclose(file);
				return std::vector<std::string>();
			}

			std::string effectiveRealBasePath = Utility::CombinePath(m_baseBundleRealPath, virtualPath);
			if (Utility::IsDirectory(effectiveRealBasePath))
			{
				std::vector<std::string> baseDirectoryList = Utility::ListSubDirectories(effectiveRealBasePath);
				directorySet.insert(baseDirectoryList.begin(), baseDirectoryList.end());
			}

			return std::vector<std::string>(directorySet.begin(), directorySet.end());
		}

		//------------------------------------------------------------------------------
		std::vector<std::string> WinPCBundleFileResourceComponent::ListFiles(const std::string &virtualPath)
		{
			std::set<std::string> fileSet;

			std::string effectiveRealOverridePath = Utility::CombinePath(m_platformOverrideBundleRealPath, virtualPath);
			if (Utility::IsDirectory(effectiveRealOverridePath))
			{
				std::vector<std::string> overridedFileList = Utility::ListFiles(effectiveRealOverridePath);
				fileSet.insert(overridedFileList.begin(), overridedFileList.end());
			}
			else if (FILE *file = fopen(effectiveRealOverridePath.c_str(), "rb"))
			{
				// Overrided by file
				fclose(file);
				return std::vector<std::string>();
			}

			std::string effectiveRealBasePath = Utility::CombinePath(m_baseBundleRealPath, virtualPath);
			if (Utility::IsDirectory(effectiveRealBasePath))
			{
				std::vector<std::string> baseFileList = Utility::ListFiles(effectiveRealBasePath);
				fileSet.insert(baseFileList.begin(), baseFileList.end());
			}

			return std::vector<std::string>(fileSet.begin(), fileSet.end());
		}

		//------------------------------------------------------------------------------
		std::string WinPCBundleFileResourceComponent::_GetBundleResourceFilePath(const std::string &virtualPath) const
		{
			std::string platformResourcePath = Utility::CombinePath(m_platformOverrideBundleRealPath, virtualPath);
			if (FILE *file = fopen(platformResourcePath.c_str(), "rb"))
			{
				fclose(file);
				return platformResourcePath;
			}

			std::string baseResourcePath = Utility::CombinePath(m_baseBundleRealPath, virtualPath);
			return baseResourcePath;
		}


		//////////////////////////////////////////////////////////////////////////////// BundleFileResourceComponent - Creation Function

		//------------------------------------------------------------------------------
		BundleFileResourceComponent *BundleFileResourceComponent::CreateComponentObject()
		{
			return new WinPCBundleFileResourceComponent();
		}
	}
}
