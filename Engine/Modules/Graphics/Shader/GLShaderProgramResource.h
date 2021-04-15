////////////////////////////////////////////////////////////////////////////////
// Shader/GLShaderProgramResource.h (Leggiero/Modules - Graphics)
//
// OpenGL Shader Program Resource
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__SHADER__GL_SHADER_PROGRAM_RESOURCE_H
#define __LM_GRAPHICS__SHADER__GL_SHADER_PROGRAM_RESOURCE_H


// Leggiero.Basics
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <functional>
#include <memory>

// External Library
#include <GLES3.h>

// Leggiero.Utility
#include <Utility/Sugar/EventNotifier.h>
#include <Utility/Sugar/NonCopyable.h>

// Leggiero.Graphics
#include "../Common/IGLGraphicResource.h"


namespace Leggiero
{
	// Forward Declaration
	namespace Utility
	{
		namespace Object
		{
			class PointerHolder;
		}
	}


	// Forward Declaration of File System Component
	namespace FileSystem
	{
		class BundleFileResourceComponent;
	}


	namespace Graphics
	{
		// Forward Declarations
		class GraphicResourceManagerComponent;
		class GLShaderProgramResource;
		class IGLProgramStateEventObserver;


		// Shader Program Creation Functions
		std::shared_ptr<GLShaderProgramResource> LoadGLProgramResourceFromInMemoryStrings(GraphicResourceManagerComponent *ownerManager, const std::string &vsSource, const std::string &fsSource);
		std::shared_ptr<GLShaderProgramResource> LoadGLProgramResourceFromInMemoryCStrings(GraphicResourceManagerComponent *ownerManager, const char *vsSource, const char *fsSource, size_t vsSourceLength = 0, size_t fsSourceLength = 0, bool isPersistenceMemory = true);
		std::shared_ptr<GLShaderProgramResource> LoadGLProgramResourceFromFiles(GraphicResourceManagerComponent *ownerManager, const std::string &vsFilePath, const std::string &fsFilePath);
		std::shared_ptr<GLShaderProgramResource> LoadGLProgramResourceFromBundle(GraphicResourceManagerComponent *ownerManager, FileSystem::BundleFileResourceComponent *bundle, const std::string &vsVirtualPath, const std::string &fsVirtualPath);


		// OpenGL ES Shader Program Resource
		class GLShaderProgramResource
			: public IGLGraphicResource
			, public std::enable_shared_from_this<GLShaderProgramResource>
			, private Utility::SyntacticSugar::NonCopyable
		{
		public:	// Constants
			static constexpr GLint kLocation_INVALID = static_cast<GLint>(-2);

		protected:
			friend std::shared_ptr<GLShaderProgramResource> LoadGLProgramResourceFromInMemoryStrings(GraphicResourceManagerComponent *, const std::string &, const std::string &);
			friend std::shared_ptr<GLShaderProgramResource> LoadGLProgramResourceFromInMemoryCStrings(GraphicResourceManagerComponent *, const char *, const char *, size_t, size_t, bool);
			friend std::shared_ptr<GLShaderProgramResource> LoadGLProgramResourceFromFiles(GraphicResourceManagerComponent *, const std::string &, const std::string &);
			friend std::shared_ptr<GLShaderProgramResource> LoadGLProgramResourceFromBundle(GraphicResourceManagerComponent *, FileSystem::BundleFileResourceComponent *, const std::string &, const std::string &);

		public:
			// Direct calling the constructor is NOT recommended. Please use creation functions.
			// The constructor remains public due to std::make_shared call.
			GLShaderProgramResource(
				GraphicResourceManagerComponent *ownerManager,
				const std::string &vertexShaderSourceString, const char *vertexShaderSourceCString,
				const std::string &fragmentShaderSourceString, const char *fragmentShaderSourceCString);

		public:
			virtual ~GLShaderProgramResource();

		public:
			GLuint GetGLProgram() const { return m_glProgram; }

		public:
			bool Use();
			GLint GetAttribLocation(const GLchar *name);
			GLint GetUniformLocation(const GLchar *name);

		public:
			bool IsVertexShaderCompileSuccess() const { return m_isVertexShaderCompileSuccess; }
			const char *GetVertexShaderCompileInfoLog() const { return m_vertexShaderInfoLog; }
			bool IsFragmentShaderCompileSuccess() const { return m_isFragmentShaderCompileSuccess; }
			const char *GetFragmentShaderCompileInfoLog() const { return m_fragmentShaderInfoLog; }
			bool IsProgramLinkSuccess() const { return m_isProgramLinkSuccess; }
			const char *GetProgramLinkInfoLog() const { return m_programInfoLog; }

		public:
			void RegisterStateEventObserver(IGLProgramStateEventObserver *observer);
			void UnRegisterStateEventObserver(IGLProgramStateEventObserver *observer);

		public:	// IGLGraphicResource
			virtual bool IsValid() override;
			virtual bool Restore() override;

		protected:
			std::function<void()> _GenerateResourceTrashFunc();

			void _ClearCompileStatus();
			bool _Compile();

		protected:
			bool m_isInitialized;
			bool m_isInvalidated;

			GLuint m_glProgram;
			GLuint m_glVertexShader;
			GLuint m_glFragmentShader;

			std::weak_ptr<Utility::Object::PointerHolder> m_managerHolder;

			bool m_isUsingVertexShader;
			std::string m_vertexShaderSourceString;
			const char *m_vertexShaderSourceCString;

			bool m_isUsingFragmentShader;
			std::string m_fragmentShaderSourceString;
			const char *m_fragmentShaderSourceCString;

		protected:
			bool m_isVertexShaderCompileSuccess;
			char *m_vertexShaderInfoLog;
			bool m_isFragmentShaderCompileSuccess;
			char *m_fragmentShaderInfoLog;
			bool m_isProgramLinkSuccess;
			char *m_programInfoLog;

		protected:
			Utility::DesignPattern::EventNotifier<IGLProgramStateEventObserver *> m_stateEventNotifier;
		};
	}
}

#endif
