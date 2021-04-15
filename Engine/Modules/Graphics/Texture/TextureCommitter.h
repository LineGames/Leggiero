////////////////////////////////////////////////////////////////////////////////
// Texture/TextureCommitter.h (Leggiero/Modules - Graphics)
//
// Texture Committer Class (member of Resource Manager Component)
// Use Texture in same context after load to avoid thread race condition
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__TEXTURE__TEXTURE_COMMITTER_H
#define __LM_GRAPHICS__TEXTURE__TEXTURE_COMMITTER_H


// Leggiero.Basics
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <atomic>
#include <memory>

// External Library
#include <GLES3.h>
#include <concurrentqueue/concurrentqueue.h>

// Leggiero.Utility
#include <Utility/Sugar/NonCopyable.h>
#include <Utility/Threading/ManagedThreadPrimitives.h>


namespace Leggiero
{
	namespace Graphics
	{
		// Forward Declarations
		class GraphicResourceManagerComponent;
		class GLTextureResource;
		class DynamicTextureResource;
		class GLShaderProgramResource;


		// Texture Committer
		class TextureCommitter
			: private Utility::SyntacticSugar::NonCopyable
		{
			friend class GraphicResourceManagerComponent;

		public:
			TextureCommitter(GraphicResourceManagerComponent *ownerManager);
			virtual ~TextureCommitter();

		public:
			void CommitTexture(const std::shared_ptr<GLTextureResource> &targetTexture, GLsizei imageWidth = 0, GLsizei imageHeight = 0);

		protected:
			std::shared_ptr<DynamicTextureResource> _ClaimTexutreCommitTarget();
			void _ReleaseTexutreCommitTarget(std::shared_ptr<DynamicTextureResource> target);

			void _PrepareTextureCommitTarget(int count);

			void _FinalizeAllTextureCommitResources();
			void _CheckAndRestoreTextureCommitResources();

			void _InitializeTextureCommitShaderProgram();
			void _CheckAndPrepareTextureCommitShaderProgram();

		protected:
			GraphicResourceManagerComponent *m_ownerResourceManager;

			moodycamel::ConcurrentQueue<std::shared_ptr<DynamicTextureResource> >	m_cachedTextureCommitTargets;

			std::atomic_bool							m_isTextureCommitShaderProgramInitilized;
			Utility::Threading::SafePthreadLock			m_textureCommitShaderProgramInitializeLock;
			std::shared_ptr<GLShaderProgramResource>	m_textureCommitShaderProgram;
			GLint										m_textureCommitShaderPositionSlot;
			GLint										m_textureCommitShaderTextureUVSlot;
			GLint										m_textureCommitShaderTextureLocation;
		};
	}
}

#endif
