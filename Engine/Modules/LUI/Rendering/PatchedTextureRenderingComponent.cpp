////////////////////////////////////////////////////////////////////////////////
// Rendering/PatchedTextureRenderingComponent.cpp (Leggiero/Modules - LegacyUI)
//
// Resizable Patched Texture Rendering Component Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "PatchedTextureRenderingComponent.h"

// Standard Library
#include <cmath>

// Leggiero.Graphics
#include <Graphics/Texture/GLTextureResource.h>

// Leggiero.LegacyUI
#include "../Common/UITransform.h"
#include "UIShaders.h"
#include "UIRenderer.h"
#include "UIRenderingUtility.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Rendering
		{
			//////////////////////////////////////////////////////////////////////////////// Utility

			namespace _UIPatchedTextureRenderingUtil
			{
				inline void PrepareRenderClippedRect(UISimpleRectangular &renderingRect, IUITransform &effectiveTransform,
					const Graphics::TextureRectSection &textureSection, const Graphics::GLByteARGB &color, const IUIClipping &clipping,
					bool isFlipHorizontal, bool isFlipVertical,
					std::vector<Graphics::TextureColoredVertex> &vertexBuffer, std::vector<GLushort> &indexBuffer)
				{
					RenderingUtility::PrepareRenderTextureRect(
						effectiveTransform.ApplyTransform(renderingRect.left, renderingRect.top),
						effectiveTransform.ApplyTransform(renderingRect.right, renderingRect.top),
						effectiveTransform.ApplyTransform(renderingRect.left, renderingRect.bottom),
						effectiveTransform.ApplyTransform(renderingRect.right, renderingRect.bottom),
						textureSection, color, clipping, isFlipHorizontal, isFlipVertical, false, vertexBuffer, indexBuffer);
				}
			}


			//////////////////////////////////////////////////////////////////////////////// Patch3TextureRenderingComponent

			//------------------------------------------------------------------------------
			Patch3TextureRenderingComponent::Patch3TextureRenderingComponent(UICoordinateType width, UICoordinateType height, bool isVertical,
				std::shared_ptr<Graphics::GLTextureResource> startTexture, const Graphics::TextureRectSection &startTextureSection,
				std::shared_ptr<Graphics::GLTextureResource> middleTexture, const Graphics::TextureRectSection &middleTextureSection,
				std::shared_ptr<Graphics::GLTextureResource> finishTexture, const Graphics::TextureRectSection &finishTextureSection,
				Graphics::GLByteARGB color)
				: m_width(width), m_height(height), m_isVertical(isVertical), m_color(color)
				, m_texture{ startTexture, middleTexture, finishTexture }, m_section{ startTextureSection, middleTextureSection, finishTextureSection }
			{
				m_vertexBuffer.reserve(12);
				m_indexBuffer.reserve(18);
				for (size_t i = 0; i < 3; ++i)
				{
					m_isFlipHorizontal[i] = false;
					m_isFlipVertical[i] = false;
				}
			}

			//------------------------------------------------------------------------------
			Patch3TextureRenderingComponent::~Patch3TextureRenderingComponent()
			{
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<IUIComponent> Patch3TextureRenderingComponent::Clone(const UIObject &ownerObject) const
			{
				return std::make_shared<Patch3TextureRenderingComponent>(m_width, m_height, m_isVertical,
					m_texture[0], m_section[0],
					m_texture[1], m_section[1],
					m_texture[2], m_section[2],
					m_color);
			}

			//------------------------------------------------------------------------------
			void Patch3TextureRenderingComponent::Render(const UIRenderer &renderer, IUITransform &effectiveTransform, const IUIClipping &effectiveClipping, float alpha)
			{
				if (alpha <= 0.0f)
				{
					return;
				}

				if (fabs(m_width) < kFloatEpsilon || fabs(m_height) < kFloatEpsilon)
				{
					// No area to render
					return;
				}

				Graphics::GLByteARGB effectiveColor(m_color);
				if (alpha < 1.0f)
				{
					effectiveColor.alpha = (GLubyte)((float)effectiveColor.alpha * alpha);
				}

				if (m_isVertical)
				{
					UICoordinateType startHeight = static_cast<UICoordinateType>((float)m_section[0].GetPixelHeight() * m_width / (float)m_section[0].GetPixelWidth());
					UICoordinateType finishHeight = static_cast<UICoordinateType>((float)m_section[2].GetPixelHeight() * m_width / (float)m_section[2].GetPixelWidth());
					UICoordinateType middleHeight = static_cast<UICoordinateType>(0.0f);
					if (m_height > 0.0f)
					{
						middleHeight = m_height - (startHeight + finishHeight);
						if (middleHeight < 0.0f)
						{
							UICoordinateType neededHeight = static_cast<UICoordinateType>(startHeight + finishHeight);
							UICoordinateType heightUnit = static_cast<UICoordinateType>(m_height / neededHeight);
							startHeight *= heightUnit;
							finishHeight *= heightUnit;
							middleHeight = static_cast<UICoordinateType>(0.0f);
						}
					}
					else
					{
						middleHeight = -m_height - (startHeight + finishHeight);
						if (middleHeight < 0.0f)
						{
							UICoordinateType neededHeight = static_cast<UICoordinateType>(startHeight + finishHeight);
							UICoordinateType heightUnit = static_cast<UICoordinateType>(-m_height / neededHeight);
							startHeight *= heightUnit;
							finishHeight *= heightUnit;
							middleHeight = static_cast<UICoordinateType>(0.0f);
						}
						startHeight = -startHeight;
						middleHeight = -middleHeight;
						finishHeight = -finishHeight;
					}

					UISimpleRectangular effectiveStartTextureRectArea = UISimpleRectangular::FromPositionSize(static_cast<UICoordinateType>(0.0f), static_cast<UICoordinateType>(0.0f), m_width, startHeight);
					_UIPatchedTextureRenderingUtil::PrepareRenderClippedRect(effectiveStartTextureRectArea, effectiveTransform, m_section[0], effectiveColor, effectiveClipping, m_isFlipHorizontal[0], m_isFlipVertical[0], m_vertexBuffer, m_indexBuffer);

					if (fabs(middleHeight) > 0.0f)
					{
						if (m_texture[0]->GetTextureInfo().name != m_texture[1]->GetTextureInfo().name)
						{
							_FlushRenderingBuffer(renderer, 0);
						}

						UISimpleRectangular effectiveMiddleTextureRectArea = UISimpleRectangular::FromPositionSize(static_cast<UICoordinateType>(0.0f), static_cast<UICoordinateType>(startHeight), m_width, middleHeight);
						_UIPatchedTextureRenderingUtil::PrepareRenderClippedRect(effectiveMiddleTextureRectArea, effectiveTransform, m_section[1], effectiveColor, effectiveClipping, m_isFlipHorizontal[1], m_isFlipVertical[1], m_vertexBuffer, m_indexBuffer);

						if (m_texture[1]->GetTextureInfo().name != m_texture[2]->GetTextureInfo().name)
						{
							_FlushRenderingBuffer(renderer, 1);
						}
					}
					else
					{
						if (m_texture[0]->GetTextureInfo().name != m_texture[2]->GetTextureInfo().name)
						{
							_FlushRenderingBuffer(renderer, 0);
						}
					}

					UISimpleRectangular effectiveFinishTextureRectArea = UISimpleRectangular::FromPositionSize(static_cast<UICoordinateType>(0.0f), static_cast<UICoordinateType>(startHeight + middleHeight), m_width, finishHeight);
					_UIPatchedTextureRenderingUtil::PrepareRenderClippedRect(effectiveFinishTextureRectArea, effectiveTransform, m_section[2], effectiveColor, effectiveClipping, m_isFlipHorizontal[2], m_isFlipVertical[2], m_vertexBuffer, m_indexBuffer);

					_FlushRenderingBuffer(renderer, 2);
				}
				else
				{
					UICoordinateType startWidth = static_cast<UICoordinateType>((float)m_section[0].GetPixelWidth() * m_height / (float)m_section[0].GetPixelHeight());
					UICoordinateType finishWidth = static_cast<UICoordinateType>((float)m_section[2].GetPixelWidth() * m_height / (float)m_section[2].GetPixelHeight());
					UICoordinateType middleWidth = static_cast<UICoordinateType>(0.0f);
					if (m_width > 0.0f)
					{
						middleWidth = m_width - (startWidth + finishWidth);
						if (middleWidth < 0.0f)
						{
							UICoordinateType neededWidth = static_cast<UICoordinateType>(startWidth + finishWidth);
							UICoordinateType widthUnit = static_cast<UICoordinateType>(m_width / neededWidth);
							startWidth *= widthUnit;
							finishWidth *= widthUnit;
							middleWidth = static_cast<UICoordinateType>(0.0f);
						}
					}
					else
					{
						middleWidth = -m_width - (startWidth + finishWidth);
						if (middleWidth < 0.0f)
						{
							UICoordinateType neededWidth = static_cast<UICoordinateType>(startWidth + finishWidth);
							UICoordinateType widthUnit = static_cast<UICoordinateType>(-m_width / neededWidth);
							startWidth *= widthUnit;
							finishWidth *= widthUnit;
							middleWidth = static_cast<UICoordinateType>(0.0f);
						}
						startWidth = -startWidth;
						middleWidth = -middleWidth;
						finishWidth = -finishWidth;
					}

					UISimpleRectangular effectiveStartTextureRectArea = UISimpleRectangular::FromPositionSize(static_cast<UICoordinateType>(0.0f), static_cast<UICoordinateType>(0.0f), startWidth, m_height);
					_UIPatchedTextureRenderingUtil::PrepareRenderClippedRect(effectiveStartTextureRectArea, effectiveTransform, m_section[0], effectiveColor, effectiveClipping, false, false, m_vertexBuffer, m_indexBuffer);

					if (fabs(middleWidth) > 0.0f)
					{
						if (m_texture[0]->GetTextureInfo().name != m_texture[1]->GetTextureInfo().name)
						{
							_FlushRenderingBuffer(renderer, 0);
						}

						UISimpleRectangular effectiveMiddleTextureRectArea = UISimpleRectangular::FromPositionSize(static_cast<UICoordinateType>(startWidth), static_cast<UICoordinateType>(0.0f), middleWidth, m_height);
						_UIPatchedTextureRenderingUtil::PrepareRenderClippedRect(effectiveMiddleTextureRectArea, effectiveTransform, m_section[1], effectiveColor, effectiveClipping, false, false, m_vertexBuffer, m_indexBuffer);

						if (m_texture[1]->GetTextureInfo().name != m_texture[2]->GetTextureInfo().name)
						{
							_FlushRenderingBuffer(renderer, 1);
						}
					}
					else
					{
						if (m_texture[0]->GetTextureInfo().name != m_texture[2]->GetTextureInfo().name)
						{
							_FlushRenderingBuffer(renderer, 0);
						}
					}

					UISimpleRectangular effectiveFinishTextureRectArea = UISimpleRectangular::FromPositionSize(static_cast<UICoordinateType>(startWidth + middleWidth), static_cast<UICoordinateType>(0.0f), finishWidth, m_height);
					_UIPatchedTextureRenderingUtil::PrepareRenderClippedRect(effectiveFinishTextureRectArea, effectiveTransform, m_section[2], effectiveColor, effectiveClipping, false, false, m_vertexBuffer, m_indexBuffer);

					_FlushRenderingBuffer(renderer, 2);
				}
			}

			//------------------------------------------------------------------------------
			void Patch3TextureRenderingComponent::SetSize(UICoordinateType width, UICoordinateType height)
			{
				m_width = width;
				m_height = height;
			}

			//------------------------------------------------------------------------------
			void Patch3TextureRenderingComponent::SetFlipState(size_t index, bool isFlipHorizontal, bool isFlipVertical)
			{
				m_isFlipHorizontal[index] = isFlipHorizontal;
				m_isFlipVertical[index] = isFlipVertical;
			}

			//------------------------------------------------------------------------------
			void Patch3TextureRenderingComponent::_FlushRenderingBuffer(const UIRenderer &renderer, size_t textureIndex)
			{
				if (m_indexBuffer.size() > 3)
				{
					std::shared_ptr<Shaders::UITextureColorShader> shader = renderer.GetTextureColorShader();
					shader->Use();

					glDisable(GL_DEPTH_TEST);
					glDisable(GL_CULL_FACE);

					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

					glUniform1i(shader->GetTextureLocation(), 0);
					m_texture[textureIndex]->Bind(GL_TEXTURE0);

					auto vertexAttribEnabledContext = shader->EnableUsingVertexAttribArray(true);
					Graphics::TextureColoredVertex::SetGLVertexAttribPointer(shader->GetPositionSlot(), shader->GetColorSlot(), shader->GetTextureUVSlot(), &m_vertexBuffer[0]);
					glDrawElements(GL_TRIANGLES, (GLsizei)m_indexBuffer.size(), GL_UNSIGNED_SHORT, &m_indexBuffer[0]);
				}
				m_vertexBuffer.clear();
				m_indexBuffer.clear();
			}


			//////////////////////////////////////////////////////////////////////////////// Patch3TextureRenderingComponent

			//------------------------------------------------------------------------------
			Patch9TextureRenderingComponent::Patch9TextureRenderingComponent(UICoordinateType width, UICoordinateType height,
				std::shared_ptr<Graphics::GLTextureResource> ltTexture, const Graphics::TextureRectSection &ltTextureSection,
				std::shared_ptr<Graphics::GLTextureResource> ctTexture, const Graphics::TextureRectSection &ctTextureSection,
				std::shared_ptr<Graphics::GLTextureResource> rtTexture, const Graphics::TextureRectSection &rtTextureSection,
				std::shared_ptr<Graphics::GLTextureResource> lmTexture, const Graphics::TextureRectSection &lmTextureSection,
				std::shared_ptr<Graphics::GLTextureResource> cmTexture, const Graphics::TextureRectSection &cmTextureSection,
				std::shared_ptr<Graphics::GLTextureResource> rmTexture, const Graphics::TextureRectSection &rmTextureSection,
				std::shared_ptr<Graphics::GLTextureResource> lbTexture, const Graphics::TextureRectSection &lbTextureSection,
				std::shared_ptr<Graphics::GLTextureResource> cbTexture, const Graphics::TextureRectSection &cbTextureSection,
				std::shared_ptr<Graphics::GLTextureResource> rbTexture, const Graphics::TextureRectSection &rbTextureSection,
				Graphics::GLByteARGB color,
				UICoordinateRatioType partScaleX, UICoordinateRatioType partScaleY)
				: m_width(width), m_height(height), m_color(color), m_partScaleX(partScaleX), m_partScaleY(partScaleY)
				, m_texture{ ltTexture, ctTexture, rtTexture, lmTexture, cmTexture, rmTexture, lbTexture, cbTexture, rbTexture }
				, m_section{ ltTextureSection, ctTextureSection, rtTextureSection, lmTextureSection, cmTextureSection, rmTextureSection, lbTextureSection, cbTextureSection, rbTextureSection }
			{
				m_vertexBuffer.reserve(12);
				m_indexBuffer.reserve(18);
				for (size_t i = 0; i < 9; ++i)
				{
					m_isFlipHorizontal[i] = false;
					m_isFlipVertical[i] = false;
				}
			}

			//------------------------------------------------------------------------------
			Patch9TextureRenderingComponent::~Patch9TextureRenderingComponent()
			{
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<IUIComponent> Patch9TextureRenderingComponent::Clone(const UIObject &ownerObject) const
			{
				std::shared_ptr<Patch9TextureRenderingComponent> clonedComponent = std::make_shared<Patch9TextureRenderingComponent>(m_width, m_height,
					m_texture[(size_t)SubPatchIndexType::kLT], m_section[(size_t)SubPatchIndexType::kLT],
					m_texture[(size_t)SubPatchIndexType::kCT], m_section[(size_t)SubPatchIndexType::kCT],
					m_texture[(size_t)SubPatchIndexType::kRT], m_section[(size_t)SubPatchIndexType::kRT],
					m_texture[(size_t)SubPatchIndexType::kLM], m_section[(size_t)SubPatchIndexType::kLM],
					m_texture[(size_t)SubPatchIndexType::kCM], m_section[(size_t)SubPatchIndexType::kCM],
					m_texture[(size_t)SubPatchIndexType::kRM], m_section[(size_t)SubPatchIndexType::kRM],
					m_texture[(size_t)SubPatchIndexType::kLB], m_section[(size_t)SubPatchIndexType::kLB],
					m_texture[(size_t)SubPatchIndexType::kCB], m_section[(size_t)SubPatchIndexType::kCB],
					m_texture[(size_t)SubPatchIndexType::kRB], m_section[(size_t)SubPatchIndexType::kRB],
					m_color);

				for (size_t i = 0; i < 9; ++i)
				{
					clonedComponent->m_isFlipHorizontal[i] = m_isFlipHorizontal[i];
					clonedComponent->m_isFlipVertical[i] = m_isFlipVertical[i];
				}

				return clonedComponent;
			}

			//------------------------------------------------------------------------------
			void Patch9TextureRenderingComponent::Render(const UIRenderer &renderer, IUITransform &effectiveTransform, const IUIClipping &effectiveClipping, float alpha)
			{
				if (alpha <= 0.0f)
				{
					return;
				}

				if (fabs(m_width) < kFloatEpsilon || fabs(m_height) < kFloatEpsilon)
				{
					// No area to render
					return;
				}

				Graphics::GLByteARGB effectiveColor(m_color);
				if (alpha < 1.0f)
				{
					effectiveColor.alpha = (GLubyte)((float)effectiveColor.alpha * alpha);
				}

				UICoordinateType patchSeamLeftX = static_cast<UICoordinateType>(0.0f);
				UICoordinateType patchSeamRightX = static_cast<UICoordinateType>(0.0f);
				if (m_width > 0.0f)
				{
					patchSeamLeftX = static_cast<UICoordinateType>(m_section[(size_t)SubPatchIndexType::kLM].GetPixelWidth() * m_partScaleX);
					patchSeamRightX = m_width - static_cast<UICoordinateType>(m_section[(size_t)SubPatchIndexType::kRM].GetPixelWidth() * m_partScaleX);
					if (patchSeamLeftX > patchSeamRightX)
					{
						patchSeamLeftX = m_width * patchSeamLeftX / (patchSeamLeftX + static_cast<UICoordinateType>(m_section[(size_t)SubPatchIndexType::kRM].GetPixelWidth() * m_partScaleX));
						patchSeamRightX = patchSeamLeftX;
					}
				}
				else
				{
					patchSeamLeftX = -static_cast<UICoordinateType>(m_section[(size_t)SubPatchIndexType::kLM].GetPixelWidth() * m_partScaleX);
					patchSeamRightX = m_width + static_cast<UICoordinateType>(m_section[(size_t)SubPatchIndexType::kRM].GetPixelWidth() * m_partScaleX);
					if (patchSeamLeftX < patchSeamRightX)
					{
						patchSeamLeftX = m_width * -patchSeamLeftX / (-patchSeamLeftX + static_cast<UICoordinateType>(m_section[(size_t)SubPatchIndexType::kRM].GetPixelWidth() * m_partScaleX));
						patchSeamRightX = patchSeamLeftX;
					}
				}

				UICoordinateType patchSeamTopY = static_cast<UICoordinateType>(0.0f);
				UICoordinateType patchSeamBottomY = static_cast<UICoordinateType>(0.0f);
				if (m_height > 0.0f)
				{
					patchSeamTopY = static_cast<UICoordinateType>(m_section[(size_t)SubPatchIndexType::kCT].GetPixelHeight() * m_partScaleY);
					patchSeamBottomY = m_height - static_cast<UICoordinateType>(m_section[(size_t)SubPatchIndexType::kCB].GetPixelHeight() * m_partScaleY);
					if (patchSeamTopY > patchSeamBottomY)
					{
						patchSeamTopY = m_height * patchSeamTopY / (patchSeamTopY + static_cast<UICoordinateType>(m_section[(size_t)SubPatchIndexType::kCB].GetPixelHeight() * m_partScaleY));
						patchSeamBottomY = patchSeamTopY;
					}
				}
				else
				{
					patchSeamTopY = -static_cast<UICoordinateType>(m_section[(size_t)SubPatchIndexType::kCT].GetPixelHeight() * m_partScaleY);
					patchSeamBottomY = m_height + static_cast<UICoordinateType>(m_section[(size_t)SubPatchIndexType::kCB].GetPixelHeight() * m_partScaleY);
					if (patchSeamTopY < patchSeamBottomY)
					{
						patchSeamTopY = m_height * -patchSeamTopY / (-patchSeamTopY + static_cast<UICoordinateType>(m_section[(size_t)SubPatchIndexType::kCB].GetPixelHeight() * m_partScaleY));
						patchSeamBottomY = patchSeamTopY;
					}
				}

				UISimpleRectangular effectiveLTRectArea(static_cast<UICoordinateType>(0.0f), static_cast<UICoordinateType>(0.0f), patchSeamLeftX, patchSeamTopY);
				_UIPatchedTextureRenderingUtil::PrepareRenderClippedRect(effectiveLTRectArea, effectiveTransform, m_section[(size_t)SubPatchIndexType::kLT], effectiveColor, effectiveClipping,
					m_isFlipHorizontal[(size_t)SubPatchIndexType::kLT], m_isFlipVertical[(size_t)SubPatchIndexType::kLT], m_vertexBuffer, m_indexBuffer);
				size_t lastQueuedIndex = (size_t)SubPatchIndexType::kLT;

				if (fabs(patchSeamLeftX - patchSeamRightX) > kFloatEpsilon)
				{
					if (m_texture[lastQueuedIndex]->GetTextureInfo().name != m_texture[(size_t)SubPatchIndexType::kCT]->GetTextureInfo().name)
					{
						_FlushRenderingBuffer(renderer, lastQueuedIndex);
					}
					UISimpleRectangular effectiveCTRectArea(patchSeamLeftX, static_cast<UICoordinateType>(0.0f), patchSeamRightX, patchSeamTopY);
					_UIPatchedTextureRenderingUtil::PrepareRenderClippedRect(effectiveCTRectArea, effectiveTransform, m_section[(size_t)SubPatchIndexType::kCT], effectiveColor, effectiveClipping,
						m_isFlipHorizontal[(size_t)SubPatchIndexType::kCT], m_isFlipVertical[(size_t)SubPatchIndexType::kCT], m_vertexBuffer, m_indexBuffer);
					lastQueuedIndex = (size_t)SubPatchIndexType::kCT;
				}

				if (m_texture[lastQueuedIndex]->GetTextureInfo().name != m_texture[(size_t)SubPatchIndexType::kRT]->GetTextureInfo().name)
				{
					_FlushRenderingBuffer(renderer, lastQueuedIndex);
				}
				UISimpleRectangular effectiveRTRectArea(patchSeamRightX, static_cast<UICoordinateType>(0.0f), m_width, patchSeamTopY);
				_UIPatchedTextureRenderingUtil::PrepareRenderClippedRect(effectiveRTRectArea, effectiveTransform, m_section[(size_t)SubPatchIndexType::kRT], effectiveColor, effectiveClipping,
					m_isFlipHorizontal[(size_t)SubPatchIndexType::kRT], m_isFlipVertical[(size_t)SubPatchIndexType::kRT], m_vertexBuffer, m_indexBuffer);
				lastQueuedIndex = (size_t)SubPatchIndexType::kRT;

				if (fabs(patchSeamTopY - patchSeamBottomY) > kFloatEpsilon)
				{
					if (m_texture[lastQueuedIndex]->GetTextureInfo().name != m_texture[(size_t)SubPatchIndexType::kLM]->GetTextureInfo().name)
					{
						_FlushRenderingBuffer(renderer, lastQueuedIndex);
					}
					UISimpleRectangular effectiveLMRectArea(static_cast<UICoordinateType>(0.0f), patchSeamTopY, patchSeamLeftX, patchSeamBottomY);
					_UIPatchedTextureRenderingUtil::PrepareRenderClippedRect(effectiveLMRectArea, effectiveTransform, m_section[(size_t)SubPatchIndexType::kLM], effectiveColor, effectiveClipping,
						m_isFlipHorizontal[(size_t)SubPatchIndexType::kLM], m_isFlipVertical[(size_t)SubPatchIndexType::kLM], m_vertexBuffer, m_indexBuffer);
					lastQueuedIndex = (size_t)SubPatchIndexType::kLM;

					if (fabs(patchSeamLeftX - patchSeamRightX) > kFloatEpsilon)
					{
						if (m_texture[lastQueuedIndex]->GetTextureInfo().name != m_texture[(size_t)SubPatchIndexType::kCM]->GetTextureInfo().name)
						{
							_FlushRenderingBuffer(renderer, lastQueuedIndex);
						}
						UISimpleRectangular effectiveCMRectArea(patchSeamLeftX, patchSeamTopY, patchSeamRightX, patchSeamBottomY);
						_UIPatchedTextureRenderingUtil::PrepareRenderClippedRect(effectiveCMRectArea, effectiveTransform, m_section[(size_t)SubPatchIndexType::kCM], effectiveColor, effectiveClipping,
							m_isFlipHorizontal[(size_t)SubPatchIndexType::kCM], m_isFlipVertical[(size_t)SubPatchIndexType::kCM], m_vertexBuffer, m_indexBuffer);
						lastQueuedIndex = (size_t)SubPatchIndexType::kCM;
					}

					if (m_texture[lastQueuedIndex]->GetTextureInfo().name != m_texture[(size_t)SubPatchIndexType::kRM]->GetTextureInfo().name)
					{
						_FlushRenderingBuffer(renderer, lastQueuedIndex);
					}
					UISimpleRectangular effectiveRMRectArea(patchSeamRightX, patchSeamTopY, m_width, patchSeamBottomY);
					_UIPatchedTextureRenderingUtil::PrepareRenderClippedRect(effectiveRMRectArea, effectiveTransform, m_section[(size_t)SubPatchIndexType::kRM], effectiveColor, effectiveClipping,
						m_isFlipHorizontal[(size_t)SubPatchIndexType::kRM], m_isFlipVertical[(size_t)SubPatchIndexType::kRM], m_vertexBuffer, m_indexBuffer);
					lastQueuedIndex = (size_t)SubPatchIndexType::kRM;
				}

				if (m_texture[lastQueuedIndex]->GetTextureInfo().name != m_texture[(size_t)SubPatchIndexType::kLB]->GetTextureInfo().name)
				{
					_FlushRenderingBuffer(renderer, lastQueuedIndex);
				}
				UISimpleRectangular effectiveLBRectArea(static_cast<UICoordinateType>(0.0f), patchSeamBottomY, patchSeamLeftX, m_height);
				_UIPatchedTextureRenderingUtil::PrepareRenderClippedRect(effectiveLBRectArea, effectiveTransform, m_section[(size_t)SubPatchIndexType::kLB], effectiveColor, effectiveClipping,
					m_isFlipHorizontal[(size_t)SubPatchIndexType::kLB], m_isFlipVertical[(size_t)SubPatchIndexType::kLB], m_vertexBuffer, m_indexBuffer);
				lastQueuedIndex = (size_t)SubPatchIndexType::kLB;

				if (fabs(patchSeamLeftX - patchSeamRightX) > kFloatEpsilon)
				{
					if (m_texture[lastQueuedIndex]->GetTextureInfo().name != m_texture[(size_t)SubPatchIndexType::kCB]->GetTextureInfo().name)
					{
						_FlushRenderingBuffer(renderer, lastQueuedIndex);
					}
					UISimpleRectangular effectiveCBRectArea(patchSeamLeftX, patchSeamBottomY, patchSeamRightX, m_height);
					_UIPatchedTextureRenderingUtil::PrepareRenderClippedRect(effectiveCBRectArea, effectiveTransform, m_section[(size_t)SubPatchIndexType::kCB], effectiveColor, effectiveClipping,
						m_isFlipHorizontal[(size_t)SubPatchIndexType::kCB], m_isFlipVertical[(size_t)SubPatchIndexType::kCB], m_vertexBuffer, m_indexBuffer);
					lastQueuedIndex = (size_t)SubPatchIndexType::kCB;
				}

				if (m_texture[lastQueuedIndex]->GetTextureInfo().name != m_texture[(size_t)SubPatchIndexType::kRB]->GetTextureInfo().name)
				{
					_FlushRenderingBuffer(renderer, lastQueuedIndex);
				}
				UISimpleRectangular effectiveRBRectArea(patchSeamRightX, patchSeamBottomY, m_width, m_height);
				_UIPatchedTextureRenderingUtil::PrepareRenderClippedRect(effectiveRBRectArea, effectiveTransform, m_section[(size_t)SubPatchIndexType::kRB], effectiveColor, effectiveClipping,
					m_isFlipHorizontal[(size_t)SubPatchIndexType::kRB], m_isFlipVertical[(size_t)SubPatchIndexType::kRB], m_vertexBuffer, m_indexBuffer);
				lastQueuedIndex = (size_t)SubPatchIndexType::kRB;

				_FlushRenderingBuffer(renderer, lastQueuedIndex);
			}

			//------------------------------------------------------------------------------
			void Patch9TextureRenderingComponent::SetSize(UICoordinateType width, UICoordinateType height)
			{
				m_width = width;
				m_height = height;
			}

			//------------------------------------------------------------------------------
			void Patch9TextureRenderingComponent::SetFlipState(SubPatchIndexType subPatch, bool isFlipHorizontal, bool isFlipVertical)
			{
				m_isFlipHorizontal[(size_t)subPatch] = isFlipHorizontal;
				m_isFlipVertical[(size_t)subPatch] = isFlipVertical;
			}

			//------------------------------------------------------------------------------
			void Patch9TextureRenderingComponent::_FlushRenderingBuffer(const UIRenderer &renderer, size_t textureIndex)
			{
				if (m_indexBuffer.size() > 3)
				{
					std::shared_ptr<Shaders::UITextureColorShader> shader = renderer.GetTextureColorShader();
					shader->Use();

					glDisable(GL_DEPTH_TEST);
					glDisable(GL_CULL_FACE);

					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

					glUniform1i(shader->GetTextureLocation(), 0);
					m_texture[textureIndex]->Bind(GL_TEXTURE0);

					auto vertexAttribEnabledContext = shader->EnableUsingVertexAttribArray(true);
					Graphics::TextureColoredVertex::SetGLVertexAttribPointer(shader->GetPositionSlot(), shader->GetColorSlot(), shader->GetTextureUVSlot(), &m_vertexBuffer[0]);
					glDrawElements(GL_TRIANGLES, (GLsizei)m_indexBuffer.size(), GL_UNSIGNED_SHORT, &m_indexBuffer[0]);
				}
				m_vertexBuffer.clear();
				m_indexBuffer.clear();
			}
		}
	}
}
