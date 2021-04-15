////////////////////////////////////////////////////////////////////////////////
// Shader/ProgramManager.cpp (Leggiero/Modules - Graphics)
//
// Implementation of Program Manager
// This is for divide implementation of GraphicResourceManagerComponent to
// to avoid to be gigantic monolithic manager component.
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "ProgramManager.h"

// Leggiero.Graphics
#include "../GraphicResourceManagerComponent.h"
#include "GLShaderProgramResource.h"


namespace Leggiero
{
	namespace Graphics
	{
		//////////////////////////////////////////////////////////////////////////////// ProgramManager

		//------------------------------------------------------------------------------
		ProgramManager::ProgramManager(GraphicResourceManagerComponent *ownerManager)
			: m_ownerResourceManager(ownerManager)
		{
		}

		//------------------------------------------------------------------------------
		ProgramManager::~ProgramManager()
		{
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<GLShaderProgramResource> ProgramManager::LoadGLProgramFromInMemoryStrings(std::string vsSource, std::string fsSource)
		{
			std::shared_ptr<GLShaderProgramResource> createdResource = LoadGLProgramResourceFromInMemoryStrings(m_ownerResourceManager, vsSource, fsSource);
			if (createdResource)
			{
				m_ownerResourceManager->_RegisterResource(createdResource);
			}
			return createdResource;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<GLShaderProgramResource> ProgramManager::LoadGLProgramFromInMemoryCStrings(const char *vsSource, const char *fsSource, size_t vsSourceLength, size_t fsSourceLength, bool isPersistenceMemory)
		{
			std::shared_ptr<GLShaderProgramResource> createdResource = LoadGLProgramResourceFromInMemoryCStrings(m_ownerResourceManager, vsSource, fsSource, vsSourceLength, fsSourceLength, isPersistenceMemory);
			if (createdResource)
			{
				m_ownerResourceManager->_RegisterResource(createdResource);
			}
			return createdResource;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<GLShaderProgramResource> ProgramManager::LoadGLProgramFromFiles(std::string vsFilePath, std::string fsFilePath)
		{
			std::shared_ptr<GLShaderProgramResource> createdResource = LoadGLProgramResourceFromFiles(m_ownerResourceManager, vsFilePath, fsFilePath);
			if (createdResource)
			{
				m_ownerResourceManager->_RegisterResource(createdResource);
			}
			return createdResource;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<GLShaderProgramResource> ProgramManager::LoadGLProgramFromBundle(std::string vsVirtualPath, std::string fsVirtualPath)
		{
			if (m_ownerResourceManager->m_bundleFileSystemComponent == nullptr)
			{
				// Just fail when bundle file system not exists
				return nullptr;
			}

			std::shared_ptr<GLShaderProgramResource> createdResource = LoadGLProgramResourceFromBundle(m_ownerResourceManager, m_ownerResourceManager->m_bundleFileSystemComponent, vsVirtualPath, fsVirtualPath);
			if (createdResource)
			{
				m_ownerResourceManager->_RegisterResource(createdResource);
			}
			return createdResource;
		}
	}
}
