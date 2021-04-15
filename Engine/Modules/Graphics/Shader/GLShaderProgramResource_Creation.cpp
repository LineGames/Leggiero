////////////////////////////////////////////////////////////////////////////////
// Shader/GLShaderProgramResource.cpp (Leggiero/Modules - Graphics)
//
// Implementation of Creation Functions of Shader Program Resource
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "GLShaderProgramResource.h"

// Standard Library
#include <cstring>
#include <fstream>
#include <sstream>

// Leggiero.FileSystem
#include <FileSystem/BundleFileResourceComponent.h>


namespace Leggiero
{
	namespace Graphics
	{
		namespace _Internal
		{
			static const std::string _kEmptyStringForGLProgramResource("");
		}


		//////////////////////////////////////////////////////////////////////////////// Shader Program Creation Functions

		//------------------------------------------------------------------------------
		// Create GL Shader Program Resource from In-Memory std::string source
		std::shared_ptr<GLShaderProgramResource> LoadGLProgramResourceFromInMemoryStrings(GraphicResourceManagerComponent *ownerManager, const std::string &vsSource, const std::string &fsSource)
		{
			return std::make_shared<GLShaderProgramResource>(ownerManager, vsSource, nullptr, fsSource, nullptr);
		}

		//------------------------------------------------------------------------------
		// Create GL Shader Program Resource from In-Memory C string source
		std::shared_ptr<GLShaderProgramResource> LoadGLProgramResourceFromInMemoryCStrings(GraphicResourceManagerComponent *ownerManager, const char *vsSource, const char *fsSource, size_t vsSourceLength, size_t fsSourceLength, bool isPersistenceMemory)
		{
			std::shared_ptr<GLShaderProgramResource> createdResource;

			const char *vsSourceEffectiveCString = nullptr;
			const char *fsSourceEffectiveCString = nullptr;
			if (isPersistenceMemory)
			{
				if (vsSourceLength == 0)
				{
					vsSourceEffectiveCString = vsSource;
				}
				if (fsSourceLength == 0)
				{
					fsSourceEffectiveCString = fsSource;
				}
			}

			// Branch directly to avoid overhead from object construction
			if (vsSource == vsSourceEffectiveCString && fsSource == fsSourceEffectiveCString)
			{
				createdResource = std::make_shared<GLShaderProgramResource>(ownerManager, _Internal::_kEmptyStringForGLProgramResource, vsSourceEffectiveCString, _Internal::_kEmptyStringForGLProgramResource, fsSourceEffectiveCString);
			}
			else if (vsSource == vsSourceEffectiveCString)
			{
				if (fsSourceLength == 0)
				{
					createdResource = std::make_shared<GLShaderProgramResource>(ownerManager, _Internal::_kEmptyStringForGLProgramResource, vsSourceEffectiveCString, std::string(fsSource), nullptr);
				}
				else
				{
					createdResource = std::make_shared<GLShaderProgramResource>(ownerManager, _Internal::_kEmptyStringForGLProgramResource, vsSourceEffectiveCString, std::string(fsSource, fsSourceLength), nullptr);
				}
			}
			else if (fsSource == fsSourceEffectiveCString)
			{
				if (vsSourceLength == 0)
				{
					createdResource = std::make_shared<GLShaderProgramResource>(ownerManager, std::string(vsSource), nullptr, _Internal::_kEmptyStringForGLProgramResource, fsSourceEffectiveCString);
				}
				else
				{
					createdResource = std::make_shared<GLShaderProgramResource>(ownerManager, std::string(vsSource, vsSourceLength), nullptr, _Internal::_kEmptyStringForGLProgramResource, fsSourceEffectiveCString);
				}
			}
			else
			{
				if (vsSourceLength == 0 && fsSourceLength == 0)
				{
					createdResource = std::make_shared<GLShaderProgramResource>(ownerManager, std::string(vsSource), nullptr, std::string(fsSource), nullptr);
				}
				else if (vsSourceLength == 0)
				{
					createdResource = std::make_shared<GLShaderProgramResource>(ownerManager, std::string(vsSource), nullptr, std::string(fsSource, fsSourceLength), nullptr);
				}
				else if (fsSourceLength == 0)
				{
					createdResource = std::make_shared<GLShaderProgramResource>(ownerManager, std::string(vsSource, vsSourceLength), nullptr, std::string(fsSource), nullptr);
				}
				else
				{
					createdResource = std::make_shared<GLShaderProgramResource>(ownerManager, std::string(vsSource, vsSourceLength), nullptr, std::string(fsSource, fsSourceLength), nullptr);
				}
			}

			return createdResource;
		}

		//------------------------------------------------------------------------------
		// Create GL Shader Program Resource from source files
		std::shared_ptr<GLShaderProgramResource> LoadGLProgramResourceFromFiles(GraphicResourceManagerComponent *ownerManager, const std::string &vsFilePath, const std::string &fsFilePath)
		{
			std::stringstream vsBufferStream;
			if (!vsFilePath.empty())
			{
				std::ifstream inputFileStream(vsFilePath, std::ios::binary);
				if (inputFileStream.good())
				{
					vsBufferStream << inputFileStream.rdbuf();
				}
				inputFileStream.close();
			}

			std::stringstream fsBufferStream;
			if (!fsFilePath.empty())
			{
				std::ifstream inputFileStream(fsFilePath, std::ios::binary);
				if (inputFileStream.good())
				{
					fsBufferStream << inputFileStream.rdbuf();
				}
				inputFileStream.close();
			}

			return std::make_shared<GLShaderProgramResource>(ownerManager, vsBufferStream.str(), nullptr, fsBufferStream.str(), nullptr);
		}

		//------------------------------------------------------------------------------
		// Create GL Shader Program Resource from source in app bundle
		std::shared_ptr<GLShaderProgramResource> LoadGLProgramResourceFromBundle(GraphicResourceManagerComponent *ownerManager, FileSystem::BundleFileResourceComponent *bundle, const std::string &vsVirtualPath, const std::string &fsVirtualPath)
		{
			// Real File Access Optimization
			if (bundle->NPO_IsEnable_BundleFileRealPath())
			{
				return LoadGLProgramResourceFromFiles(ownerManager,
					vsVirtualPath.empty() ? _Internal::_kEmptyStringForGLProgramResource : bundle->NPO_GetBundleFileRealPath(vsVirtualPath),
					fsVirtualPath.empty() ? _Internal::_kEmptyStringForGLProgramResource : bundle->NPO_GetBundleFileRealPath(fsVirtualPath));
			}

			// Load Resource
			std::stringstream vsBufferStream;
			if (!vsVirtualPath.empty())
			{
				bundle->ReadBundleFileData(vsVirtualPath, 0, vsBufferStream);
			}

			std::stringstream fsBufferStream;
			if (!fsVirtualPath.empty())
			{
				bundle->ReadBundleFileData(fsVirtualPath, 0, fsBufferStream);
			}

			return std::make_shared<GLShaderProgramResource>(ownerManager, vsBufferStream.str(), nullptr, fsBufferStream.str(), nullptr);
		}
	}
}
