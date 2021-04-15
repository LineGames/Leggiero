////////////////////////////////////////////////////////////////////////////////
// Rendering/CachedTextRenderingComponent.cpp (Leggiero/Modules - LegacyUI)
//
// Cached text rendering component implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "CachedTextRenderingComponent.h"

// Standard Library
#include <unordered_map>

// Leggiero.Graphics
#include <Graphics/Texture/GLTextureResource.h>
#include <Graphics/Texture/RuntimeTextureAtlasEntry.h>

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

			namespace _UITextRenderingUtil
			{
				inline void PrepareRenderClippedRect(UISimpleRectangular &renderingRect, IUITransform &effectiveTransform,
					const Graphics::TextureRectSection &textureSection, const Graphics::GLByteARGB &color, const IUIClipping &effectiveClipping, std::vector<Graphics::TextureColoredVertex> &vertexBuffer, std::vector<GLushort> &indexBuffer)
				{
					RenderingUtility::PrepareRenderTextureRect(
						effectiveTransform.ApplyTransform(renderingRect.left, renderingRect.top),
						effectiveTransform.ApplyTransform(renderingRect.right, renderingRect.top),
						effectiveTransform.ApplyTransform(renderingRect.left, renderingRect.bottom),
						effectiveTransform.ApplyTransform(renderingRect.right, renderingRect.bottom),
						textureSection, color, effectiveClipping, false, false, true, vertexBuffer, indexBuffer);
				}
			}


			//////////////////////////////////////////////////////////////////////////////// CachedTextRenderingComponent::DrawingCharacter

			//------------------------------------------------------------------------------
			CachedTextRenderingComponent::DrawingCharacter::DrawingCharacter(std::shared_ptr<Font::CachedGlyph> glyph, UIVector2D givenOrigin, Graphics::GLByteARGB color)
				: glyphInformation(glyph), characterOrigin(givenOrigin), multiplyColor(color)
				, pixelWidth(0), pixelHeight(0)
				, additionalOffset(UIVector2D::kZero)

			{
				if (glyph)
				{
					if (glyph->HasImage())
					{
						pixelWidth = glyph->GetFontImage()->GetTextureSection().GetPixelWidth();
						pixelHeight = glyph->GetFontImage()->GetTextureSection().GetPixelHeight();
					}
				}
			}


			//////////////////////////////////////////////////////////////////////////////// CachedTextRenderingComponent

			//------------------------------------------------------------------------------
			CachedTextRenderingComponent::CachedTextRenderingComponent(const std::vector<DrawingCharacter> &cachedText, UICoordinateType logicalHeight,
				bool isRenderShadow, const Graphics::GLByteARGB &shadowColor, const UIVector2D &shadowOffset,
				Graphics::GLByteARGB globalMultiplyColor)
				: m_cachedText(cachedText), m_logicalHeight(logicalHeight)
				, m_isRenderShadow(isRenderShadow), m_shadowColor(shadowColor), m_shadowOffset(shadowOffset), m_globalMultiplyColor(globalMultiplyColor)
				, m_cahcedSize(static_cast<UICoordinateType>(0.0f), logicalHeight)
			{
				size_t characterCount = cachedText.size();
				if (characterCount > 0)
				{
					m_vertexBuffer.reserve(characterCount * 4 + 8);
					m_indexBuffer.reserve(characterCount * 6 + 12);

					_CalculateSize();
				}
			}

			//------------------------------------------------------------------------------
			CachedTextRenderingComponent::~CachedTextRenderingComponent()
			{
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<IUIComponent> CachedTextRenderingComponent::Clone(const UIObject &ownerObject) const
			{
				return std::make_shared<CachedTextRenderingComponent>(m_cachedText, m_logicalHeight, m_isRenderShadow, m_shadowColor, m_shadowOffset, m_globalMultiplyColor);
			}

			//------------------------------------------------------------------------------
			void CachedTextRenderingComponent::Render(const UIRenderer &renderer, IUITransform &effectiveTransform, const IUIClipping &effectiveClipping, float alpha)
			{
				if (alpha <= 0.0f)
				{
					return;
				}

				size_t characterCount = m_cachedText.size();
				if (characterCount == 0)
				{
					return;
				}

				// Make Batch by Texture
				std::unordered_map<std::shared_ptr<Graphics::GLTextureResource>, std::vector<const DrawingCharacter *> > textureBatchTable;
				for (const DrawingCharacter &currentCharacter : m_cachedText)
				{
					if (!currentCharacter.glyphInformation)
					{
						continue;
					}
					if (!currentCharacter.glyphInformation->HasImage())
					{
						continue;
					}
					std::shared_ptr<Graphics::GLTextureResource> characterTexture(currentCharacter.glyphInformation->GetFontImage()->GetTexture());
					if (!characterTexture)
					{
						continue;
					}

					std::unordered_map<std::shared_ptr<Graphics::GLTextureResource>, std::vector<const DrawingCharacter *> >::iterator batchFindIt = textureBatchTable.find(characterTexture);
					if (batchFindIt == textureBatchTable.end())
					{
						std::vector<const DrawingCharacter *> batchList;
						batchList.push_back(&currentCharacter);
						textureBatchTable[characterTexture] = batchList;
					}
					else
					{
						batchFindIt->second.push_back(&currentCharacter);
					}
				}

				// Do Character Rendering
				UICoordinateType yBaselineOffset = (UICoordinateType)(m_logicalHeight * 0.75f);
				Graphics::GLByteARGB effectiveShadowColor(m_shadowColor);
				if (alpha < 1.0f)
				{
					effectiveShadowColor.alpha = static_cast<GLubyte>((float)effectiveShadowColor.alpha * alpha);
				}
				if (m_isRenderShadow && (m_shadowColor.alpha > 0) && (static_cast<float>(m_shadowOffset.SquareLength()) > kFloatEpsilon))
				{
					// Shadow Rendering
					for (std::unordered_map<std::shared_ptr<Graphics::GLTextureResource>, std::vector<const DrawingCharacter *> >::iterator it = textureBatchTable.begin(); it != textureBatchTable.end(); ++it)
					{
						for (const DrawingCharacter *currentCharacter : it->second)
						{
							UISimpleRectangular effectiveCharacterRectArea = UISimpleRectangular::FromPositionSize(currentCharacter->characterOrigin.x + m_shadowOffset.x, currentCharacter->characterOrigin.y + yBaselineOffset + m_shadowOffset.y,
								(UICoordinateType)currentCharacter->pixelWidth, (UICoordinateType)currentCharacter->pixelHeight);
							_UITextRenderingUtil::PrepareRenderClippedRect(effectiveCharacterRectArea, effectiveTransform, currentCharacter->glyphInformation->GetFontImage()->GetTextureSection(), effectiveShadowColor, effectiveClipping, m_vertexBuffer, m_indexBuffer);
						}

						if (m_indexBuffer.size() > 3)
						{
							std::shared_ptr<Shaders::UITextureColorShader> shader = renderer.GetTextureColorShader();
							shader->Use();

							glDisable(GL_DEPTH_TEST);
							glDisable(GL_CULL_FACE);

							glEnable(GL_BLEND);
							glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

							glUniform1i(shader->GetTextureLocation(), 0);
							it->first->Bind(GL_TEXTURE0);

							auto vertexAttribEnabledContext = shader->EnableUsingVertexAttribArray(true);
							Graphics::TextureColoredVertex::SetGLVertexAttribPointer(shader->GetPositionSlot(), shader->GetColorSlot(), shader->GetTextureUVSlot(), &m_vertexBuffer[0]);
							glDrawElements(GL_TRIANGLES, (GLsizei)m_indexBuffer.size(), GL_UNSIGNED_SHORT, &m_indexBuffer[0]);
						}
						m_vertexBuffer.clear();
						m_indexBuffer.clear();
					}
				}

				// Real Font Rendering
				Graphics::GLByteARGB globalMultiplyColor(m_globalMultiplyColor);
				if (alpha < 1.0f)
				{
					globalMultiplyColor.alpha = static_cast<GLubyte>((float)globalMultiplyColor.alpha * alpha);
				}
				for (std::unordered_map<std::shared_ptr<Graphics::GLTextureResource>, std::vector<const DrawingCharacter *> >::iterator it = textureBatchTable.begin(); it != textureBatchTable.end(); ++it)
				{
					for (const DrawingCharacter *currentCharacter : it->second)
					{
						UISimpleRectangular effectiveCharacterRectArea = UISimpleRectangular::FromPositionSize(currentCharacter->characterOrigin.x, currentCharacter->characterOrigin.y + yBaselineOffset, (UICoordinateType)currentCharacter->pixelWidth, (UICoordinateType)currentCharacter->pixelHeight);
						Graphics::GLByteARGB effectiveColor(currentCharacter->multiplyColor);
						effectiveColor.alpha = static_cast<GLubyte>((int)effectiveColor.alpha * (int)globalMultiplyColor.alpha / 255);
						effectiveColor.red = static_cast<GLubyte>((int)effectiveColor.red * (int)globalMultiplyColor.red / 255);
						effectiveColor.green = static_cast<GLubyte>((int)effectiveColor.green * (int)globalMultiplyColor.green / 255);
						effectiveColor.blue = static_cast<GLubyte>((int)effectiveColor.blue * (int)globalMultiplyColor.blue / 255);
						_UITextRenderingUtil::PrepareRenderClippedRect(effectiveCharacterRectArea, effectiveTransform, currentCharacter->glyphInformation->GetFontImage()->GetTextureSection(), effectiveColor, effectiveClipping, m_vertexBuffer, m_indexBuffer);
					}

					if (m_indexBuffer.size() > 3)
					{
						std::shared_ptr<Shaders::UITextureColorShader> shader = renderer.GetTextureColorShader();
						shader->Use();

						glDisable(GL_DEPTH_TEST);
						glDisable(GL_CULL_FACE);

						glEnable(GL_BLEND);
						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

						glUniform1i(shader->GetTextureLocation(), 0);
						it->first->Bind(GL_TEXTURE0);

						auto vertexAttribEnabledContext = shader->EnableUsingVertexAttribArray(true);
						Graphics::TextureColoredVertex::SetGLVertexAttribPointer(shader->GetPositionSlot(), shader->GetColorSlot(), shader->GetTextureUVSlot(), &m_vertexBuffer[0]);
						glDrawElements(GL_TRIANGLES, (GLsizei)m_indexBuffer.size(), GL_UNSIGNED_SHORT, &m_indexBuffer[0]);
					}
					m_vertexBuffer.clear();
					m_indexBuffer.clear();
				}
			}

			//------------------------------------------------------------------------------
			UIElementSize CachedTextRenderingComponent::GetVisualSize()
			{
				return m_cahcedSize;
			}

			//------------------------------------------------------------------------------
			void CachedTextRenderingComponent::_CalculateSize()
			{
				m_cahcedSize.height = m_logicalHeight;

				UICoordinateType maxRenderedRight = static_cast<UICoordinateType>(0.0f);
				UICoordinateType currentOriginX = static_cast<UICoordinateType>(0.0f);
				for (const DrawingCharacter &currentCharacter : m_cachedText)
				{
					currentOriginX += currentCharacter.advance;
					if (currentOriginX > maxRenderedRight)
					{
						maxRenderedRight = currentOriginX;
					}
				}
				m_cahcedSize.width = maxRenderedRight;
			}

			//------------------------------------------------------------------------------
			void CachedTextRenderingComponent::SetText(const std::vector<DrawingCharacter> &cachedText, UICoordinateType logicalHeight)
			{
				m_logicalHeight = logicalHeight;

				m_cachedText.clear();
				m_cachedText.reserve(cachedText.size());
				m_cachedText.insert(m_cachedText.end(), cachedText.begin(), cachedText.end());

				size_t characterCount = cachedText.size();
				if (characterCount > 0)
				{
					m_vertexBuffer.reserve(characterCount * 4 + 8);
					m_indexBuffer.reserve(characterCount * 6 + 12);
				}

				_CalculateSize();
			}
		}
	}
}
