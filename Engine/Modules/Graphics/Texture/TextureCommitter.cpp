////////////////////////////////////////////////////////////////////////////////
// Texture/TextureCommitter.cpp (Leggiero/Modules - Graphics)
//
// Implementation of Texture Committer
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "TextureCommitter.h"

// Standard Library
#include <vector>

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>

// Leggiero.Graphics
#include "../Common/GLFrameBufferBindContext.h"
#include "GLTextureResource.h"
#include "DynamicTextureResource.h"
#include "../Shader/GLShaderProgramResource.h"


namespace Leggiero
{
	namespace Graphics
	{
		//////////////////////////////////////////////////////////////////////////////// Internal Implementation for Texture Commit

		namespace _Internal
		{
			const char s_textureCommitVertexSharedSource[] = "attribute vec4 a_Position;\nattribute vec2 a_TextureUV0;\nvarying vec4 Position;\nvarying vec2 TextureUV0;\nvoid main()\n{\n\tgl_Position = a_Position;\n\tTextureUV0 = a_TextureUV0;\n}";
			const char s_textureCommitFragmentSharedSource[] = "varying mediump vec2 TextureUV0;\nuniform sampler2D u_Texture0;\nvoid main()\n{\n\tgl_FragColor = texture2D(u_Texture0, TextureUV0);\n}";

			// Vertex for Texture Commit
			struct TextureCommitTempVertex
			{
			public:
				float	position[4];
				float	textureUV[2];

			public:
				static void SetGLVertexAttribPointer(GLuint positionSlot, GLuint textureUVSlot, const TextureCommitTempVertex *pArrayStart)
				{
					glVertexAttribPointer(positionSlot, 4, GL_FLOAT, GL_FALSE, sizeof(TextureCommitTempVertex), &pArrayStart->position);
					glVertexAttribPointer(textureUVSlot, 2, GL_FLOAT, GL_FALSE, sizeof(TextureCommitTempVertex), &pArrayStart->textureUV);
				}
			};

			constexpr GLsizei kTargetTextureWidth = 4;
			constexpr GLsizei kTargetTextureHeight = 4;
		}


		//////////////////////////////////////////////////////////////////////////////// TextureCommitter

		//------------------------------------------------------------------------------
		TextureCommitter::TextureCommitter(GraphicResourceManagerComponent *ownerManager)
			: m_ownerResourceManager(ownerManager)
			, m_isTextureCommitShaderProgramInitilized(false)
		{
		}

		//------------------------------------------------------------------------------
		TextureCommitter::~TextureCommitter()
		{
		}

		//------------------------------------------------------------------------------
		void TextureCommitter::CommitTexture(const std::shared_ptr<GLTextureResource> &targetTexture, GLsizei imageWidth, GLsizei imageHeight)
		{
			if (!targetTexture->GetTextureInfo().IsValid())
			{
				// Invalid Texture
			}

			if (imageWidth == 0)
			{
				imageWidth = targetTexture->GetTextureInfo().contentWidth;
			}
			if (imageHeight == 0)
			{
				imageHeight = targetTexture->GetTextureInfo().contentHeight;
			}
			if (imageWidth == 0 || imageHeight == 0)
			{
				// Null Image
				return;
			}

			if (!m_isTextureCommitShaderProgramInitilized.load())
			{
				_CheckAndPrepareTextureCommitShaderProgram();
			}
			if (!m_textureCommitShaderProgram)
			{
				// Commit Shader Not Available
				return;
			}
			if (!m_textureCommitShaderProgram->IsValid())
			{
				_CheckAndRestoreTextureCommitResources();
				if (!m_textureCommitShaderProgram || !m_textureCommitShaderProgram->IsValid())
				{
					// Commit Shader Not Available
					return;
				}
			}

			std::shared_ptr<DynamicTextureResource> commitTarget(_ClaimTexutreCommitTarget());
			if (!commitTarget)
			{
				// Commit Target Not Available
				return;
			}

			std::vector<_Internal::TextureCommitTempVertex> textureVertexBuffer;
			textureVertexBuffer.reserve(6);
			_Internal::TextureCommitTempVertex tempVertex;

			float leftU = 0.0f;
			float rightU = (float)imageWidth / (float)targetTexture->GetTextureInfo().width;
			float topV = 0.0f;
			float bottomV = (float)imageHeight / (float)targetTexture->GetTextureInfo().height;

			{
				TextureCommitter *pCapturedThis = this;
				auto commitTargetReleaseFunc = [commitTarget, pCapturedThis]() mutable { pCapturedThis->_ReleaseTexutreCommitTarget(commitTarget); };
				FINALLY_OF_BLOCK(_RELEASE_COMMIT_TARGET, commitTargetReleaseFunc);

				// Make frame buffer and bind
				// Cannot cache frame buffer itself because it should used for each own thread
				GLint prevBinded;
				glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevBinded);

				GLint prevViewport[4];
				glGetIntegerv(GL_VIEWPORT, prevViewport);

				GLFrameBufferBindContext frameBufferRevertContext((GLuint)prevBinded, (GLint)prevViewport[0], (GLint)prevViewport[1], (GLsizei)prevViewport[2], (GLsizei)prevViewport[3], true);

				GLuint createdFrameBufferName = 0;
				glGenFramebuffers(1, &createdFrameBufferName);
				{
					auto framebufferDeleteFunc = [createdFrameBufferName]() mutable {
						GLuint tempValue = createdFrameBufferName;
						glDeleteFramebuffers(1, &tempValue);
					};
					FINALLY_OF_BLOCK(_DELETE_FRAMEBUFFER, framebufferDeleteFunc);

					glBindFramebuffer(GL_FRAMEBUFFER, createdFrameBufferName);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, commitTarget->GetTextureInfo().name, 0);

					if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
					{
						glViewport(0, 0, _Internal::kTargetTextureWidth, _Internal::kTargetTextureHeight);

						m_textureCommitShaderProgram->Use();

						glEnableVertexAttribArray(m_textureCommitShaderPositionSlot);
						glEnableVertexAttribArray(m_textureCommitShaderTextureUVSlot);

						glDisable(GL_DEPTH_TEST);
						glDisable(GL_CULL_FACE);
						glDisable(GL_BLEND);

						glUniform1i(m_textureCommitShaderTextureLocation, 0);

						tempVertex = { { 0.0f, 0.0f, 0.0f, 1.0f },{ leftU, topV } }; textureVertexBuffer.push_back(tempVertex);
						tempVertex = { { 0.0f, 1.0f, 0.0f, 1.0f },{ leftU, bottomV } }; textureVertexBuffer.push_back(tempVertex);
						tempVertex = { { 1.0f, 1.0f, 0.0f, 1.0f },{ rightU, bottomV } }; textureVertexBuffer.push_back(tempVertex);

						tempVertex = { { 1.0f, 1.0f, 0.0f, 1.0f },{ rightU, bottomV } }; textureVertexBuffer.push_back(tempVertex);
						tempVertex = { { 1.0f, 0.0f, 0.0f, 1.0f },{ rightU, topV } }; textureVertexBuffer.push_back(tempVertex);
						tempVertex = { { 0.0f, 0.0f, 0.0f, 1.0f },{ leftU, topV } }; textureVertexBuffer.push_back(tempVertex);

						targetTexture->Bind(GL_TEXTURE0);

						_Internal::TextureCommitTempVertex::SetGLVertexAttribPointer(m_textureCommitShaderPositionSlot, m_textureCommitShaderTextureUVSlot, &textureVertexBuffer[0]);
						glDrawArrays(GL_TRIANGLES, 0, (GLsizei)textureVertexBuffer.size());

						glDisableVertexAttribArray(m_textureCommitShaderPositionSlot);
						glDisableVertexAttribArray(m_textureCommitShaderTextureUVSlot);

                        #ifdef _LEGGIERO_IOS
                            glFlush();
                        #else
                            glFinish();
                        #endif
					}
				}
			}
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<DynamicTextureResource> TextureCommitter::_ClaimTexutreCommitTarget()
		{
			// Try to use cached targets
			std::shared_ptr<DynamicTextureResource> resultTarget;
			while (m_cachedTextureCommitTargets.try_dequeue(resultTarget))
			{
				if (resultTarget->IsValid())
				{
					return resultTarget;
				}
				else
				{
					resultTarget->Restore();
					if (resultTarget->IsValid())
					{
						return resultTarget;
					}
				}
			}

			// Create New
			return CreateDynamicTextureResource(m_ownerResourceManager, _Internal::kTargetTextureWidth, _Internal::kTargetTextureHeight, PredefinedDynamicTextureNotifier::kNonNotifyingDynamicTextureRestoreNotifier, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, 0, true);
		}

		//------------------------------------------------------------------------------
		void TextureCommitter::_ReleaseTexutreCommitTarget(std::shared_ptr<DynamicTextureResource> target)
		{
			if (!target)
			{
				return;
			}
			if (!target->IsValid())
			{
				return;
			}

			m_cachedTextureCommitTargets.enqueue(target);
		}

		//------------------------------------------------------------------------------
		void TextureCommitter::_PrepareTextureCommitTarget(int count)
		{
			int existingCacheApprox = static_cast<int>(m_cachedTextureCommitTargets.size_approx());
			int effectiveCount = count - existingCacheApprox;
			if (effectiveCount <= 0)
			{
				// Enough cache
				return;
			}

			std::vector<std::shared_ptr<DynamicTextureResource> > committingTextureCache;
			committingTextureCache.reserve(effectiveCount);
			for (int i = 0; i < effectiveCount; ++i)
			{
				std::shared_ptr<DynamicTextureResource> cachingTexture(_ClaimTexutreCommitTarget());
				if (cachingTexture)
				{
					committingTextureCache.push_back(cachingTexture);
				}
			}

			for (std::shared_ptr<DynamicTextureResource> &currentTexture : committingTextureCache)
			{
				_ReleaseTexutreCommitTarget(currentTexture);
			}
		}

		//------------------------------------------------------------------------------
		void TextureCommitter::_FinalizeAllTextureCommitResources()
		{
			std::shared_ptr<DynamicTextureResource> dequeuedItem;
			while (m_cachedTextureCommitTargets.try_dequeue(dequeuedItem))
			{
				// Just dequeue and throw away
			}

			m_isTextureCommitShaderProgramInitilized.store(false);
			m_textureCommitShaderProgram.reset();
		}

		//------------------------------------------------------------------------------
		void TextureCommitter::_CheckAndRestoreTextureCommitResources()
		{
			// Restore Shader Program
			if (m_textureCommitShaderProgram && !m_textureCommitShaderProgram->IsValid())
			{
				m_textureCommitShaderProgram->Restore();
				if (m_textureCommitShaderProgram->IsValid())
				{
					m_textureCommitShaderPositionSlot = m_textureCommitShaderProgram->GetAttribLocation("a_Position");
					m_textureCommitShaderTextureUVSlot = m_textureCommitShaderProgram->GetAttribLocation("a_TextureUV0");
					m_textureCommitShaderTextureLocation = m_textureCommitShaderProgram->GetUniformLocation("u_Texture0");
				}
				else
				{
					m_textureCommitShaderProgram.reset();
					m_isTextureCommitShaderProgramInitilized.store(false);
				}
			}
			if (!m_textureCommitShaderProgram)
			{
				_CheckAndPrepareTextureCommitShaderProgram();
			}

			// Restore Targets
			std::vector<std::shared_ptr<DynamicTextureResource> > restoredItems;
			restoredItems.reserve(m_cachedTextureCommitTargets.size_approx() + 2);
			std::shared_ptr<DynamicTextureResource> dequeuedItem;
			while (m_cachedTextureCommitTargets.try_dequeue(dequeuedItem))
			{
				if (dequeuedItem->IsValid())
				{
					restoredItems.push_back(dequeuedItem);
				}
				else
				{
					dequeuedItem->Restore();
					if (dequeuedItem->IsValid())
					{
						restoredItems.push_back(dequeuedItem);
					}
				}
			}

			for (std::shared_ptr<DynamicTextureResource> &currentRestored : restoredItems)
			{
				m_cachedTextureCommitTargets.enqueue(currentRestored);
			}
			restoredItems.clear();
		}

		//------------------------------------------------------------------------------
		void TextureCommitter::_InitializeTextureCommitShaderProgram()
		{
			m_textureCommitShaderProgram = LoadGLProgramResourceFromInMemoryCStrings(m_ownerResourceManager,
				_Internal::s_textureCommitVertexSharedSource, _Internal::s_textureCommitFragmentSharedSource, std::strlen(_Internal::s_textureCommitVertexSharedSource), std::strlen(_Internal::s_textureCommitFragmentSharedSource), true);
			if (!m_textureCommitShaderProgram)
			{
				return;
			}

			m_textureCommitShaderPositionSlot = m_textureCommitShaderProgram->GetAttribLocation("a_Position");
			m_textureCommitShaderTextureUVSlot = m_textureCommitShaderProgram->GetAttribLocation("a_TextureUV0");

			m_textureCommitShaderTextureLocation = m_textureCommitShaderProgram->GetUniformLocation("u_Texture0");

			m_isTextureCommitShaderProgramInitilized.store(true);
		}

		//------------------------------------------------------------------------------
		void TextureCommitter::_CheckAndPrepareTextureCommitShaderProgram()
		{
			auto lock = m_textureCommitShaderProgramInitializeLock.Lock();	// Try to lock, but go even it failed

			if (!m_isTextureCommitShaderProgramInitilized.load())
			{
				_InitializeTextureCommitShaderProgram();
			}
		}
	}
}
