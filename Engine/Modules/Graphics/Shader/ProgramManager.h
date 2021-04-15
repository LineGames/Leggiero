////////////////////////////////////////////////////////////////////////////////
// Shader/ProgramManager.h (Leggiero/Modules - Graphics)
//
// Program Resource Manager Class (member of Resource Manager Component)
// All Load and Creation Function should be called in the thread w/ GL Context
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__SHADER__PROGRAM_MANAGER_H
#define __LM_GRAPHICS__SHADER__PROGRAM_MANAGER_H


// Leggiero.Basics
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>
#include <string>

// External Library
#include <GLES3.h>

// Leggiero.Utility
#include <Utility/Sugar/NonCopyable.h>


namespace Leggiero
{
	namespace Graphics
	{
		// Forward Declarations
		class GraphicResourceManagerComponent;
		class GLShaderProgramResource;


		// Program Manager
		class ProgramManager
			: private Utility::SyntacticSugar::NonCopyable
		{
		public:
			ProgramManager(GraphicResourceManagerComponent *ownerManager);
			virtual ~ProgramManager();

		public:
			std::shared_ptr<GLShaderProgramResource> LoadGLProgramFromInMemoryStrings(std::string vsSource, std::string fsSource);
			std::shared_ptr<GLShaderProgramResource> LoadGLProgramFromInMemoryCStrings(const char *vsSource, const char *fsSource, size_t vsSourceLength = 0, size_t fsSourceLength = 0, bool isPersistenceMemory = true);
			std::shared_ptr<GLShaderProgramResource> LoadGLProgramFromFiles(std::string vsFilePath, std::string fsFilePath);
			std::shared_ptr<GLShaderProgramResource> LoadGLProgramFromBundle(std::string vsVirtualPath, std::string fsVirtualPath);

		protected:
			GraphicResourceManagerComponent *m_ownerResourceManager;
		};
	}
}

#endif
