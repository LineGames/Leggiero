////////////////////////////////////////////////////////////////////////////////
// Element/UIElementBlurredBarrier.cpp (Leggiero/Modules - LegacyUI)
//
// UI Blurred Barrier Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIElementBlurredBarrier.h"

// Standard Library
#include <cmath>
#include <cstring>

// External Library
#include <GLES3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Leggiero.Utility
#include <Utility/Math/SimpleMath.h>

// Leggiero.Graphics
#include <Graphics/GraphicResourceManagerComponent.h>
#include <Graphics/Texture/TextureManager.h>
#include <Graphics/Common/GraphicsReferenceState.h>
#include <Graphics/Texture/TextureHelper.h>
#include <Graphics/Texture/TextureSection.h>
#include <Graphics/Texture/DynamicTextureResource.h>
#include <Graphics/Texture/RenderTargetTextureResource.h>
#include <Graphics/Shader/CommonGLVertexType.h>

// Leggiero.LegacyUI
#include "../Component/UILayoutComponent.h"
#include "../Component/UISizeSubComponent.h"
#include "../Rendering/UIShaders.h"
#include "../Rendering/TextureRenderingComponent.h"
#include "../Rendering/ShapeSetRenderingComponent.h"
#include "../Rendering/UIShapeRect.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Element
		{
			//////////////////////////////////////////////////////////////////////////////// Constants

			namespace UIElementBlurredBarrierConstants
			{
				constexpr int kBlurMarginWidth = 7;
			}


			//////////////////////////////////////////////////////////////////////////////// UIElementBlurredBarrier

			//------------------------------------------------------------------------------
			UIElementBlurredBarrier::UIElementBlurredBarrier(std::shared_ptr<UIManager> ownerManager, Graphics::GraphicResourceManagerComponent *pGraphicResourceManager, Graphics::GLByteARGB alternativeColor,
				bool isSkipDefaultBlur, int blurIterations, std::shared_ptr<Shaders::UITextureBlurShader> moreBlurShader, float downsampleRatio, std::shared_ptr<Shaders::UITextureColorShader> downsampleShader)
				: UIObject(ownerManager)
				, m_pGraphicResourceManager(pGraphicResourceManager), m_alternativeColor(alternativeColor), m_isSkipDefaultBlur(isSkipDefaultBlur)
				, m_isGraphicPrepareTried(false)
				, m_isDynamic(true), m_isDirty(true), m_isBlurIterationChanged(true)
				, m_downsampleRatio(downsampleRatio), m_downsampleShader(downsampleShader), m_isDownsampling(downsampleRatio > 1.0f && downsampleShader)
				, m_blurIterations(blurIterations), m_moreBlurShader(moreBlurShader), m_isMoreBlurEnabled(moreBlurShader)
				, m_width(static_cast<UICoordinateType>(0.0f)), m_height(static_cast<UICoordinateType>(0.0f))
			{
				m_requiredWidth = static_cast<UICoordinateType>(Graphics::ReferenceState::GetReferenceViewportWidth());
				m_requiredHeight = static_cast<UICoordinateType>(Graphics::ReferenceState::GetReferenceViewportHeight());

				UICoordinateType maxTextureSize = static_cast<UICoordinateType>(Graphics::ReferenceState::GetMaximumTextureDimension());
				if (m_requiredWidth > maxTextureSize || m_requiredHeight > maxTextureSize)
				{
					m_isEnableTextureCapture = false;
					m_alternativeRectShape = std::make_shared<UIShapeRect>((UICoordinateType)1.0f, (UICoordinateType)1.0f, alternativeColor);
					RegisterComponent(std::make_shared<Rendering::ShapeSetRenderingComponent>(std::initializer_list<std::shared_ptr<IUIRenderingShape> >{ m_alternativeRectShape }));
					return;
				}

				m_isEnableTextureCapture = true;

				if (!m_isDownsampling)
				{
					m_downsampleRatio = 1.0f;
				}

				if (m_blurIterations < 0)
				{
					m_blurIterations = 0;
				}

				if (m_isSkipDefaultBlur)
				{
					m_renderingComponent = std::make_shared<Rendering::TextureRenderingComponent>(nullptr);
					m_renderingComponent->SetFlip(false, true);
					RegisterComponent(m_renderingComponent);
				}
				else
				{
					std::shared_ptr<Rendering::BlurredTextureRenderingComponent> concreteRenderingComponent(std::make_shared<Rendering::BlurredTextureRenderingComponent>(nullptr));
					m_renderingComponent = concreteRenderingComponent;
					concreteRenderingComponent->SetFlip(false, true);
					RegisterComponent(concreteRenderingComponent);

					concreteRenderingComponent->SetLinearBlurMultiplier(1.0f / downsampleRatio, 1.0f / downsampleRatio);
				}
			}

			//------------------------------------------------------------------------------
			UIElementBlurredBarrier::UIElementBlurredBarrier(std::shared_ptr<UIManager> ownerManager, Graphics::GraphicResourceManagerComponent *pGraphicResourceManager, UICoordinateType maxWidth, UICoordinateType maxHeight, Graphics::GLByteARGB alternativeColor,
				bool isSkipDefaultBlur, int blurIterations, std::shared_ptr<Shaders::UITextureBlurShader> moreBlurShader, float downsampleRatio, std::shared_ptr<Shaders::UITextureColorShader> downsampleShader)
				: UIObject(ownerManager)
				, m_pGraphicResourceManager(pGraphicResourceManager), m_alternativeColor(alternativeColor), m_isSkipDefaultBlur(isSkipDefaultBlur)
				, m_requiredWidth(maxWidth), m_requiredHeight(maxHeight)
				, m_isGraphicPrepareTried(false)
				, m_isDynamic(true), m_isDirty(true), m_isBlurIterationChanged(true)
				, m_downsampleRatio(downsampleRatio), m_downsampleShader(downsampleShader), m_isDownsampling(downsampleRatio > 1.0f && downsampleShader)
				, m_blurIterations(blurIterations), m_moreBlurShader(moreBlurShader), m_isMoreBlurEnabled(moreBlurShader)
				, m_width(static_cast<UICoordinateType>(0.0f)), m_height(static_cast<UICoordinateType>(0.0f))
			{
				UICoordinateType maxTextureSize = static_cast<UICoordinateType>(Graphics::ReferenceState::GetMaximumTextureDimension());
				if (m_requiredWidth > maxTextureSize || m_requiredHeight > maxTextureSize)
				{
					m_isEnableTextureCapture = false;
					return;
				}

				m_isEnableTextureCapture = true;

				if (!m_isDownsampling)
				{
					m_downsampleRatio = 1.0f;
				}

				if (m_blurIterations < 0)
				{
					m_blurIterations = 0;
				}

				if (m_isSkipDefaultBlur)
				{
					m_renderingComponent = std::make_shared<Rendering::TextureRenderingComponent>(nullptr);
					m_renderingComponent->SetFlip(false, true);
					RegisterComponent(m_renderingComponent);
				}
				else
				{
					std::shared_ptr<Rendering::BlurredTextureRenderingComponent> concreteRenderingComponent(std::make_shared<Rendering::BlurredTextureRenderingComponent>(nullptr));
					m_renderingComponent = concreteRenderingComponent;
					concreteRenderingComponent->SetFlip(false, true);
					RegisterComponent(concreteRenderingComponent);

					concreteRenderingComponent->SetLinearBlurMultiplier(1.0f / downsampleRatio, 1.0f / downsampleRatio);
				}
			}

			//------------------------------------------------------------------------------
			UIElementBlurredBarrier::~UIElementBlurredBarrier()
			{
			}

			//------------------------------------------------------------------------------
			void UIElementBlurredBarrier::Prepare()
			{
				if (!m_isEnableTextureCapture)
				{
					return;
				}
				_PrepareTexture();
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UIObject> UIElementBlurredBarrier::Clone() const
			{
				std::shared_ptr<UIObject> clonedObject(UIObject::Clone());
				std::shared_ptr<UIElementBlurredBarrier> concreteCloned(std::dynamic_pointer_cast<UIElementBlurredBarrier>(clonedObject));
				if (!concreteCloned)
				{
					return clonedObject;
				}
				if (!concreteCloned->m_isEnableTextureCapture)
				{
					return clonedObject;
				}

				concreteCloned->m_renderingComponent = concreteCloned->GetComponentObject<Rendering::TextureRenderingComponent>(UIComponentType::kRendering);
				concreteCloned->m_renderingComponent->SetTexture(nullptr);

				concreteCloned->m_isDynamic = m_isDynamic;

				concreteCloned->m_isDownsampling = m_isDownsampling;
				concreteCloned->m_downsampleRatio = m_downsampleRatio;
				concreteCloned->m_downsampleShader = m_downsampleShader;

				concreteCloned->m_isMoreBlurEnabled = m_isMoreBlurEnabled;
				concreteCloned->m_blurIterations = m_blurIterations;
				concreteCloned->m_moreBlurShader = m_moreBlurShader;

				return concreteCloned;
			}

			void UIElementBlurredBarrier::ChangeMoreBlurIteration(int iterations)
			{
				int oldBlurIteration = m_blurIterations;
				if (iterations < 0)
				{
					m_blurIterations = 0;
				}
				else
				{
					m_blurIterations = iterations;
				}
				if (oldBlurIteration != m_blurIterations)
				{
					m_isBlurIterationChanged = true;
				}
			}

			//------------------------------------------------------------------------------
			void UIElementBlurredBarrier::GraphicPrepare()
			{
				if (!m_isEnableTextureCapture)
				{
					return;
				}
				_PrepareTexture();
			}

			//------------------------------------------------------------------------------
			void UIElementBlurredBarrier::FrameGraphicPrepare(const UIRenderer &renderer, UIVector2D offsetPosition, const IUIClipping &effectiveClipping)
			{
				if (!m_isEnableTextureCapture)
				{
					m_alternativeRectShape->SetSize(m_width, m_height);
					return;
				}

				if (!m_texture)
				{
					if (!_PrepareTexture())
					{
						return;
					}
				}

				if (m_width == 0 || m_height == 0.0f)
				{
					return;
				}

				if (!m_isDynamic && !m_isDirty)
				{
					if (m_isBlurIterationChanged)
					{
						m_isBlurIterationChanged = false;
						if (m_blurIterations == 0)
						{
							m_renderingComponent->SetTexture(m_texture, *m_textureSection);
							m_renderingComponent->SetSize(m_textureSampledWidth, m_textureSampledHeight);
							return;
						}
					}
					if ((m_isDownsampling && m_downSampledTexture) || (m_isMoreBlurEnabled && m_blurIterationTwoPassTexture))
					{
						if (m_isBlurIterationChanged || m_downSampledTexture->IsFreshRestored())
						{
							m_downSampledTexture->SetFreshRestoreFlag(false);
							if (!_BakeProcessedTexture())
							{
								m_renderingComponent->SetTexture(m_texture, *m_textureSection);
								m_renderingComponent->SetSize(m_textureSampledWidth, m_textureSampledHeight);
							}
						}
					}
					return;
				}

				UISimpleRectangular effectiveRectArea = UISimpleRectangular::FromPositionSize(offsetPosition.x, offsetPosition.y, m_width, m_height);
				//note: to optimize, slice rect area using clipping

				GLint startX = (GLint)floor(effectiveRectArea.left);
				GLint startY = (GLint)floor(effectiveRectArea.top);
				m_textureSampledWidth = effectiveRectArea.Width();
				m_textureSampledHeight = effectiveRectArea.Height();
				GLsizei pixelWidth = (GLsizei)ceil(m_textureSampledWidth);
				GLsizei pixelHeight = (GLsizei)ceil(m_textureSampledHeight);

				m_texture->CopyFromFrameBuffer(startX, startY, (GLint)0, (GLint)0, pixelWidth, pixelHeight);
				if (!m_isDynamic)
				{
					// manually fill clamp area to blur
					// for dynamic, don't do that because it is too expensive
					GLsizei xPatch = 0;
					GLsizei yPatch = 0;
					if (pixelWidth < m_texture->GetTextureInfo().width)
					{
						GLsizei fillCount = m_texture->GetTextureInfo().width - pixelWidth;
						if (fillCount > (GLsizei)UIElementBlurredBarrierConstants::kBlurMarginWidth)
						{
							fillCount = (GLsizei)UIElementBlurredBarrierConstants::kBlurMarginWidth;
						}
						xPatch = fillCount;

						char *rowBuffer = (char *)malloc((size_t)((pixelHeight) * 4)); // 4 due to GL_PACK_ALIGNMENT
						glReadPixels(startX + pixelWidth - 1, startY, 1, pixelHeight, GL_RGB, GL_UNSIGNED_BYTE, rowBuffer);
						size_t lineSize = xPatch * 3;
						if (lineSize % 4 > 0)
						{
							lineSize += 4 - (lineSize % 4);
						}
						char *uploadBuffer = (char *)malloc((size_t)(lineSize * pixelHeight));
						for (GLsizei i = 0; i < pixelHeight; ++i)
						{
							char *lineBuffer = uploadBuffer + lineSize * i;
							for (GLsizei j = 0; j < fillCount; ++j)
							{
								*(lineBuffer + j * 3) = rowBuffer[i * 4];
								*(lineBuffer + j * 3 + 1) = rowBuffer[i * 4 + 1];
								*(lineBuffer + j * 3 + 2) = rowBuffer[i * 4 + 2];
							}
						}
						m_texture->UploadSubImage((GLint)pixelWidth, (GLint)0, uploadBuffer, fillCount, pixelHeight);
						free(uploadBuffer);
						free(rowBuffer);
					}
					if (pixelHeight < m_texture->GetTextureInfo().height)
					{
						GLsizei fillCount = m_texture->GetTextureInfo().height - pixelHeight;
						if (fillCount > (GLsizei)UIElementBlurredBarrierConstants::kBlurMarginWidth)
						{
							fillCount = (GLsizei)UIElementBlurredBarrierConstants::kBlurMarginWidth;
						}
						yPatch = fillCount;

						char *rowBuffer = (char *)malloc((size_t)((pixelWidth + 1) * 3));
						glReadPixels(startX, startY + pixelHeight - 1, pixelWidth, 1, GL_RGB, GL_UNSIGNED_BYTE, rowBuffer);
						size_t fillRowWidth = pixelWidth + xPatch;
						size_t lineSize = fillRowWidth * 3;
						if (lineSize % 4 > 0)
						{
							lineSize += 4 - (lineSize % 4);
						}
						char *uploadBuffer = (char *)malloc((size_t)(lineSize * fillCount));
						for (GLsizei i = 0; i < pixelWidth; ++i)
						{
							for (GLsizei j = 0; j < fillCount; ++j)
							{
								*(uploadBuffer + j * lineSize + i * 3) = rowBuffer[i * 3];
								*(uploadBuffer + j * lineSize + i * 3 + 1) = rowBuffer[i * 3 + 1];
								*(uploadBuffer + j * lineSize + i * 3 + 2) = rowBuffer[i * 3 + 2];
							}
						}
						for (GLsizei i = 0; i < xPatch; ++i)
						{
							for (GLsizei j = 0; j < fillCount; ++j)
							{
								*(uploadBuffer + j * lineSize + (pixelWidth + i) * 3) = rowBuffer[(pixelWidth - 1) * 3];
								*(uploadBuffer + j * lineSize + (pixelWidth + i) * 3 + 1) = rowBuffer[(pixelWidth - 1) * 3 + 1];
								*(uploadBuffer + j * lineSize + (pixelWidth + i) * 3 + 2) = rowBuffer[(pixelWidth - 1) * 3 + 2];
							}
						}
						m_texture->UploadSubImage((GLint)0, (GLint)pixelHeight, uploadBuffer, (GLsizei)fillRowWidth, fillCount);
						free(uploadBuffer);
						free(rowBuffer);
					}
				}
				m_textureSection = std::make_shared<Graphics::TextureRectSection>(0, 0, (int)pixelWidth, (int)pixelHeight, m_texture);

				if ((m_isDownsampling && m_downSampledTexture) || (m_isMoreBlurEnabled && m_blurIterationTwoPassTexture))
				{
					if (!_BakeProcessedTexture())
					{
						m_renderingComponent->SetTexture(m_texture, *m_textureSection);
						m_renderingComponent->SetSize(m_textureSampledWidth, m_textureSampledHeight);
					}
				}
				else
				{
					m_renderingComponent->SetTexture(m_texture, *m_textureSection);
					m_renderingComponent->SetSize(m_textureSampledWidth, m_textureSampledHeight);
				}

				m_isDirty = false;
			}

			//------------------------------------------------------------------------------
			void UIElementBlurredBarrier::SetColor(Graphics::GLByteARGB color)
			{
				if (!m_isEnableTextureCapture)
				{
					return;
				}
				m_renderingComponent->SetColor(color);
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UIObject> UIElementBlurredBarrier::_AllocateCloningObject() const
			{
				std::shared_ptr<UIManager> managerCopy(m_ownerManager.lock());
				if (!managerCopy)
				{
					return nullptr;
				}
				return std::shared_ptr<UIObject>(dynamic_cast<UIObject *>(new UIElementBlurredBarrier(managerCopy, m_pGraphicResourceManager, m_requiredWidth, m_requiredHeight, m_alternativeColor, m_isSkipDefaultBlur)));
			}

			//------------------------------------------------------------------------------
			bool UIElementBlurredBarrier::_PrepareTexture()
			{
				if (m_isGraphicPrepareTried)
				{
					return false;
				}
				m_isGraphicPrepareTried = true;
				if (m_texture)
				{
					return true;
				}
				
				GLsizei textureWidth = static_cast<GLsizei>(Utility::Math::SmallestGreaterEqualPow2((int)ceil(m_requiredWidth)));
				GLsizei textureHeight = static_cast<GLsizei>(Utility::Math::SmallestGreaterEqualPow2((int)ceil(m_requiredHeight)));

				m_texture = m_pGraphicResourceManager->GetTextureManager().CreateDynamicTexture(textureWidth, textureHeight,
					std::bind(&UIElementBlurredBarrier::_OnTextureRestore, std::weak_ptr<UIElementBlurredBarrier>(std::dynamic_pointer_cast<UIElementBlurredBarrier>(shared_from_this())), std::placeholders::_1),
					GL_RGB, GL_UNSIGNED_BYTE, GL_LINEAR, GL_LINEAR);
				m_textureSection.reset();
				if (!m_texture)
				{
					m_isDirty = true;
					return false;
				}

				GLsizei effectiveSubTextureWidth = textureWidth;
				GLsizei effectiveSubTextureHeight = textureHeight;

				if (m_isDownsampling)
				{
					effectiveSubTextureWidth = (GLsizei)Utility::Math::SmallestGreaterEqualPow2((int)ceil(m_requiredWidth / m_downsampleRatio));
					effectiveSubTextureHeight = (GLsizei)Utility::Math::SmallestGreaterEqualPow2((int)ceil(m_requiredHeight / m_downsampleRatio));

					m_downSampledTexture = m_pGraphicResourceManager->GetTextureManager().CreateRenderTargetTexture(effectiveSubTextureWidth, effectiveSubTextureHeight, false, false, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
					if (!m_downSampledTexture)
					{
						m_downSampledTexture.reset();
						m_texture.reset();
						m_isDirty = true;
						return false;
					}
				}

				if (m_isMoreBlurEnabled)
				{
					if (!m_downSampledTexture)
					{
						m_downSampledTexture = m_pGraphicResourceManager->GetTextureManager().CreateRenderTargetTexture(effectiveSubTextureWidth, effectiveSubTextureHeight, false, false, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
					}
					m_blurIterationTwoPassTexture = m_pGraphicResourceManager->GetTextureManager().CreateRenderTargetTexture(effectiveSubTextureWidth, effectiveSubTextureHeight, false, false, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
					if (!m_downSampledTexture || !m_blurIterationTwoPassTexture)
					{
						m_downSampledTexture.reset();
						m_blurIterationTwoPassTexture.reset();
						m_texture.reset();
						m_isDirty = true;
						return false;
					}
				}

				m_isDirty = true;
				return true;
			}

			//------------------------------------------------------------------------------
			bool UIElementBlurredBarrier::_BakeProcessedTexture()
			{
				if (!m_isDownsampling && !m_isMoreBlurEnabled)
				{
					return false;
				}
				if (!m_isDownsampling && m_blurIterations <= 0)
				{
					return false;
				}

				std::shared_ptr<Graphics::DynamicTextureResource>		texture(m_texture);
				std::shared_ptr<Graphics::TextureRectSection>	textureSection(m_textureSection);
				std::shared_ptr<Graphics::RenderTargetTextureResource>	downSampledTexture(m_downSampledTexture);
				std::shared_ptr<Graphics::RenderTargetTextureResource>	twoPassTexture(m_blurIterationTwoPassTexture);
				if (!texture || !textureSection || (m_isDownsampling && !downSampledTexture) || (m_isMoreBlurEnabled && !twoPassTexture))
				{
					return false;
				}

				float realWidth = (float)textureSection->GetPixelWidth();
				float realHeight = (float)textureSection->GetPixelHeight();
				if (m_isDownsampling)
				{
					realWidth /= m_downsampleRatio;
					realHeight /= m_downsampleRatio;
				}

				GLsizei pixelWidth = (GLsizei)ceil(realWidth);
				GLsizei pixelHeight = (GLsizei)ceil(realHeight);

				Graphics::TextureRectSection effectiveResultSection(0, 0, (int)pixelWidth, (int)pixelHeight, downSampledTexture);
				Graphics::GLTextureResource *pCurrentSource = texture.get();

				// Common Matrix
				glm::mat4 projMat = glm::ortho(0.0f, (float)downSampledTexture->GetTextureInfo().width, 0.0f, -(float)downSampledTexture->GetTextureInfo().height, -1.0f, 1.0f);

				glm::vec3 eyePos; eyePos[0] = 0.0f; eyePos[1] = 0.0f; eyePos[2] = -1.0f;
				glm::vec3 eyeSee; eyeSee[0] = eyePos[0]; eyeSee[1] = eyePos[1]; eyeSee[2] = 0.0f;
				glm::vec3 eyeUp; eyeUp[0] = 0.0f; eyeUp[1] = -1.0f; eyeUp[2] = 0.0f;
				glm::mat4 viewMat = glm::lookAt(eyePos, eyeSee, eyeUp);

				glm::mat4 modelMat = glm::mat4(1.0f);

				float noHSLModifyVector[5] = { 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };

				// Common Data
				std::vector<Graphics::TextureColoredVertex> textureVertexBuffer;
				textureVertexBuffer.reserve(25);
				Graphics::TextureColoredVertex tempVertex;
				float leftX = 0.0f;
				float rightX = 0.0f;
				float topY = 0.0f;
				float bottomY = 0.0f;
				float leftU = 0.0f;
				float rightU = 0.0f;
				float topV = 0.0f;
				float bottomV = 0.0f;

				// Do Down Sampling
				if (m_isDownsampling)
				{
					m_downsampleShader->Use();
					auto vertexAttribEnabledContext = m_downsampleShader->EnableUsingVertexAttribArray(true);

					glDisable(GL_DEPTH_TEST);
					glDisable(GL_CULL_FACE);
					glDisable(GL_BLEND);

					glUniformMatrix4fv(m_downsampleShader->GetViewMatrixLocation(), 1, GL_FALSE, glm::value_ptr(viewMat));
					glUniformMatrix4fv(m_downsampleShader->GetModelMatrixLocation(), 1, GL_FALSE, glm::value_ptr(modelMat));
					glUniformMatrix4fv(m_downsampleShader->GetProjectionMatrixLocation(), 1, GL_FALSE, glm::value_ptr(projMat));
					glUniform1fv(m_downsampleShader->GetHSLModificationLocation(), 5, noHSLModifyVector);

					glUniform1i(m_downsampleShader->GetTextureLocation(), 0);

					texture->Bind(GL_TEXTURE0);

					rightX = realWidth;
					bottomY = realHeight;

					rightU = (float)textureSection->GetTexelRight();
					bottomV = (float)textureSection->GetTexelBottom();

					tempVertex = { { leftX, topY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ leftU, topV } }; textureVertexBuffer.push_back(tempVertex);
					tempVertex = { { leftX, bottomY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ leftU, bottomV } }; textureVertexBuffer.push_back(tempVertex);
					tempVertex = { { rightX, bottomY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ rightU, bottomV } }; textureVertexBuffer.push_back(tempVertex);

					tempVertex = { { rightX, bottomY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ rightU, bottomV } }; textureVertexBuffer.push_back(tempVertex);
					tempVertex = { { rightX, topY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ rightU, topV } }; textureVertexBuffer.push_back(tempVertex);
					tempVertex = { { leftX, topY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ leftU, topV } }; textureVertexBuffer.push_back(tempVertex);

					bool isXPatchExists = false;
					bool isYPatchExists = false;
					if ((GLsizei)pixelWidth < downSampledTexture->GetTextureInfo().width)
					{
						float superRight = rightX + (float)UIElementBlurredBarrierConstants::kBlurMarginWidth;
						float reverseLeftU = rightU - (float)textureSection->GetTexelOnePixelWidth();
						isXPatchExists = true;

						tempVertex = { { superRight, topY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ reverseLeftU, topV } }; textureVertexBuffer.push_back(tempVertex);
						tempVertex = { { superRight, bottomY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ reverseLeftU, bottomV } }; textureVertexBuffer.push_back(tempVertex);
						tempVertex = { { rightX, bottomY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ rightU, bottomV } }; textureVertexBuffer.push_back(tempVertex);

						tempVertex = { { rightX, bottomY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ rightU, bottomV } }; textureVertexBuffer.push_back(tempVertex);
						tempVertex = { { rightX, topY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ rightU, topV } }; textureVertexBuffer.push_back(tempVertex);
						tempVertex = { { superRight, topY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ reverseLeftU, topV } }; textureVertexBuffer.push_back(tempVertex);
					}
					if ((GLsizei)pixelHeight < downSampledTexture->GetTextureInfo().height)
					{
						float superBottom = bottomY + (float)UIElementBlurredBarrierConstants::kBlurMarginWidth;
						float reverseTopV = bottomV - (float)textureSection->GetTexelOnePixelHeight();
						isYPatchExists = true;

						tempVertex = { { leftX, superBottom, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ leftU, reverseTopV } }; textureVertexBuffer.push_back(tempVertex);
						tempVertex = { { leftX, bottomY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ leftU, bottomV } }; textureVertexBuffer.push_back(tempVertex);
						tempVertex = { { rightX, bottomY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ rightU, bottomV } }; textureVertexBuffer.push_back(tempVertex);

						tempVertex = { { rightX, bottomY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ rightU, bottomV } }; textureVertexBuffer.push_back(tempVertex);
						tempVertex = { { rightX, superBottom, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ rightU, reverseTopV } }; textureVertexBuffer.push_back(tempVertex);
						tempVertex = { { leftX, superBottom, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ leftU, reverseTopV } }; textureVertexBuffer.push_back(tempVertex);
					}
					if (isXPatchExists && isYPatchExists)
					{
						float superRight = rightX + (float)UIElementBlurredBarrierConstants::kBlurMarginWidth;
						float reverseLeftU = rightU - (float)textureSection->GetTexelOnePixelWidth();
						float superBottom = bottomY + (float)UIElementBlurredBarrierConstants::kBlurMarginWidth;
						float reverseTopV = bottomV - (float)textureSection->GetTexelOnePixelHeight();

						tempVertex = { { superRight, superBottom, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ reverseLeftU, reverseTopV } }; textureVertexBuffer.push_back(tempVertex);
						tempVertex = { { superRight, bottomY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ reverseLeftU, bottomV } }; textureVertexBuffer.push_back(tempVertex);
						tempVertex = { { rightX, bottomY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ rightU, bottomV } }; textureVertexBuffer.push_back(tempVertex);

						tempVertex = { { rightX, bottomY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ rightU, bottomV } }; textureVertexBuffer.push_back(tempVertex);
						tempVertex = { { rightX, superBottom, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ rightU, reverseTopV } }; textureVertexBuffer.push_back(tempVertex);
						tempVertex = { { superRight, superBottom, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ reverseLeftU, reverseTopV } }; textureVertexBuffer.push_back(tempVertex);
					}

					auto textureRenderingContext = downSampledTexture->BindToFrameBufferTarget(true);

					glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
					glClear(GL_COLOR_BUFFER_BIT);

					Graphics::TextureColoredVertex::SetGLVertexAttribPointer(m_downsampleShader->GetPositionSlot(), m_downsampleShader->GetColorSlot(), m_downsampleShader->GetTextureUVSlot(), &textureVertexBuffer[0]);
					glDrawArrays(GL_TRIANGLES, 0, (GLsizei)textureVertexBuffer.size());
					textureVertexBuffer.clear();

					pCurrentSource = downSampledTexture.get();
				}

				// Do More Blurring
				if (m_isMoreBlurEnabled && m_blurIterations > 0)
				{
					m_moreBlurShader->Use();
					auto vertexAttribEnabledContext = m_moreBlurShader->EnableUsingVertexAttribArray(true);

					glDisable(GL_DEPTH_TEST);
					glDisable(GL_CULL_FACE);
					glDisable(GL_BLEND);

					glUniformMatrix4fv(m_moreBlurShader->GetViewMatrixLocation(), 1, GL_FALSE, glm::value_ptr(viewMat));
					glUniformMatrix4fv(m_moreBlurShader->GetModelMatrixLocation(), 1, GL_FALSE, glm::value_ptr(modelMat));
					glUniformMatrix4fv(m_moreBlurShader->GetProjectionMatrixLocation(), 1, GL_FALSE, glm::value_ptr(projMat));
					glUniform1fv(m_moreBlurShader->GetHSLModificationLocation(), 5, noHSLModifyVector);

					glUniform1i(m_moreBlurShader->GetTextureLocation(), 0);

					float texelWidthParameter = 1.0f / downSampledTexture->GetTextureInfo().width;
					float texelHeightParameter = 1.0f / downSampledTexture->GetTextureInfo().height;

					rightX = (float)pixelWidth;
					bottomY = (float)pixelHeight;

					rightU = (float)effectiveResultSection.GetTexelRight();
					bottomV = (float)effectiveResultSection.GetTexelBottom();

					tempVertex = { { leftX, topY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ leftU, topV } }; textureVertexBuffer.push_back(tempVertex);
					tempVertex = { { leftX, bottomY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ leftU, bottomV } }; textureVertexBuffer.push_back(tempVertex);
					tempVertex = { { rightX, bottomY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ rightU, bottomV } }; textureVertexBuffer.push_back(tempVertex);

					tempVertex = { { rightX, bottomY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ rightU, bottomV } }; textureVertexBuffer.push_back(tempVertex);
					tempVertex = { { rightX, topY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ rightU, topV } }; textureVertexBuffer.push_back(tempVertex);
					tempVertex = { { leftX, topY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ leftU, topV } }; textureVertexBuffer.push_back(tempVertex);

					bool isXPatchExists = false;
					bool isYPatchExists = false;
					if ((GLsizei)pixelWidth < downSampledTexture->GetTextureInfo().width)
					{
						float superRight = rightX + (float)UIElementBlurredBarrierConstants::kBlurMarginWidth;
						float reverseLeftU = rightU - (float)effectiveResultSection.GetTexelOnePixelWidth();
						isXPatchExists = true;

						tempVertex = { { superRight, topY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ reverseLeftU, topV } }; textureVertexBuffer.push_back(tempVertex);
						tempVertex = { { superRight, bottomY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ reverseLeftU, bottomV } }; textureVertexBuffer.push_back(tempVertex);
						tempVertex = { { rightX, bottomY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ rightU, bottomV } }; textureVertexBuffer.push_back(tempVertex);

						tempVertex = { { rightX, bottomY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ rightU, bottomV } }; textureVertexBuffer.push_back(tempVertex);
						tempVertex = { { rightX, topY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ rightU, topV } }; textureVertexBuffer.push_back(tempVertex);
						tempVertex = { { superRight, topY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ reverseLeftU, topV } }; textureVertexBuffer.push_back(tempVertex);
					}
					if ((GLsizei)pixelHeight < downSampledTexture->GetTextureInfo().height)
					{
						float superBottom = bottomY + (float)UIElementBlurredBarrierConstants::kBlurMarginWidth;
						float reverseTopV = bottomV - (float)effectiveResultSection.GetTexelOnePixelHeight();
						isYPatchExists = true;

						tempVertex = { { leftX, superBottom, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ leftU, reverseTopV } }; textureVertexBuffer.push_back(tempVertex);
						tempVertex = { { leftX, bottomY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ leftU, bottomV } }; textureVertexBuffer.push_back(tempVertex);
						tempVertex = { { rightX, bottomY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ rightU, bottomV } }; textureVertexBuffer.push_back(tempVertex);

						tempVertex = { { rightX, bottomY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ rightU, bottomV } }; textureVertexBuffer.push_back(tempVertex);
						tempVertex = { { rightX, superBottom, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ rightU, reverseTopV } }; textureVertexBuffer.push_back(tempVertex);
						tempVertex = { { leftX, superBottom, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ leftU, reverseTopV } }; textureVertexBuffer.push_back(tempVertex);
					}
					if (isXPatchExists && isYPatchExists)
					{
						float superRight = rightX + (float)UIElementBlurredBarrierConstants::kBlurMarginWidth;
						float reverseLeftU = rightU - (float)effectiveResultSection.GetTexelOnePixelWidth();
						float superBottom = bottomY + (float)UIElementBlurredBarrierConstants::kBlurMarginWidth;
						float reverseTopV = bottomV - (float)effectiveResultSection.GetTexelOnePixelHeight();

						tempVertex = { { superRight, superBottom, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ reverseLeftU, reverseTopV } }; textureVertexBuffer.push_back(tempVertex);
						tempVertex = { { superRight, bottomY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ reverseLeftU, bottomV } }; textureVertexBuffer.push_back(tempVertex);
						tempVertex = { { rightX, bottomY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ rightU, bottomV } }; textureVertexBuffer.push_back(tempVertex);

						tempVertex = { { rightX, bottomY, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ rightU, bottomV } }; textureVertexBuffer.push_back(tempVertex);
						tempVertex = { { rightX, superBottom, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ rightU, reverseTopV } }; textureVertexBuffer.push_back(tempVertex);
						tempVertex = { { superRight, superBottom, 0.0f, 1.0f },{ 255, 255, 255, 255 },{ reverseLeftU, reverseTopV } }; textureVertexBuffer.push_back(tempVertex);
					}

					for (int i = 0; i < m_blurIterations; ++i)
					{
						// Horizontal Blur to twoPassTexture
						{
							glUniform1f(m_moreBlurShader->GetTexelWidthOffsetLocation(), texelWidthParameter);
							glUniform1f(m_moreBlurShader->GetTexelHeightOffsetLocation(), 0.0f);

							pCurrentSource->Bind(GL_TEXTURE0);

							auto textureRenderingContext = twoPassTexture->BindToFrameBufferTarget(true);

							glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
							glClear(GL_COLOR_BUFFER_BIT);

							Graphics::TextureColoredVertex::SetGLVertexAttribPointer(m_moreBlurShader->GetPositionSlot(), m_moreBlurShader->GetColorSlot(), m_moreBlurShader->GetTextureUVSlot(), &textureVertexBuffer[0]);
							glDrawArrays(GL_TRIANGLES, 0, (GLsizei)textureVertexBuffer.size());
						}
						pCurrentSource = downSampledTexture.get();

						// Vertical Blur to downSampledTexture
						{
							glUniform1f(m_moreBlurShader->GetTexelWidthOffsetLocation(), 0.0f);
							glUniform1f(m_moreBlurShader->GetTexelHeightOffsetLocation(), texelHeightParameter);

							twoPassTexture->Bind(GL_TEXTURE0);

							auto textureRenderingContext = downSampledTexture->BindToFrameBufferTarget(true);

							glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
							glClear(GL_COLOR_BUFFER_BIT);

							Graphics::TextureColoredVertex::SetGLVertexAttribPointer(m_moreBlurShader->GetPositionSlot(), m_moreBlurShader->GetColorSlot(), m_moreBlurShader->GetTextureUVSlot(), &textureVertexBuffer[0]);
							glDrawArrays(GL_TRIANGLES, 0, (GLsizei)textureVertexBuffer.size());
						}
					}
				}

				// Set Result Texture
				m_renderingComponent->SetTexture(downSampledTexture, effectiveResultSection);
				m_renderingComponent->SetSize(m_textureSampledWidth, m_textureSampledHeight);

				m_isBlurIterationChanged = false;

				return true;
			}

			//------------------------------------------------------------------------------
			void UIElementBlurredBarrier::_OnTextureRestore(std::weak_ptr<UIElementBlurredBarrier> weakThis, std::shared_ptr<Graphics::DynamicTextureResource> texture)
			{
				std::shared_ptr<UIElementBlurredBarrier> copiedThis(weakThis.lock());
				copiedThis->m_textureSection.reset();
				copiedThis->m_isDirty = true;
			}
		}
	}
}
