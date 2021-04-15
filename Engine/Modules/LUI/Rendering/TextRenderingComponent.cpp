////////////////////////////////////////////////////////////////////////////////
// Rendering/TextRenderingComponent.cpp (Leggiero/Modules - LegacyUI)
//
// Implementation of text rendering component
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "TextRenderingComponent.h"

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


			//////////////////////////////////////////////////////////////////////////////// TextRenderingComponent

			//------------------------------------------------------------------------------
			TextRenderingComponent::TextRenderingComponent(std::shared_ptr<Font::CachedGlyphTypesetting> typesetting,
				bool isRenderShadow, const Graphics::GLByteARGB &shadowColor, const UIVector2D &shadowOffset, Graphics::GLByteARGB globalMultiplyColor)
				: m_typesetting(typesetting)
				, m_isRenderShadow(isRenderShadow), m_shadowColor(shadowColor), m_shadowOffset(shadowOffset)
				, m_globalMultiplyColor(globalMultiplyColor)
			{
				if (typesetting)
				{
					size_t characterCount = typesetting->characters.size();
					m_vertexBuffer.reserve(characterCount * 4 + 8);
					m_indexBuffer.reserve(characterCount * 6 + 12);
				}
			}

			//------------------------------------------------------------------------------
			TextRenderingComponent::~TextRenderingComponent()
			{
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<IUIComponent> TextRenderingComponent::Clone(const UIObject &ownerObject) const
			{
				return std::make_shared<TextRenderingComponent>(m_typesetting, m_isRenderShadow, m_shadowColor, m_shadowOffset, m_globalMultiplyColor);
			}

			//------------------------------------------------------------------------------
			void TextRenderingComponent::Render(const UIRenderer &renderer, IUITransform &effectiveTransform, const IUIClipping &effectiveClipping, float alpha)
			{
				if (alpha <= 0.0f)
				{
					return;
				}

				std::shared_ptr<Font::CachedGlyphTypesetting> typesettingCopy(m_typesetting);
				if (!typesettingCopy)
				{
					return;
				}

				// Make Batch by Texture
				std::unordered_map<std::shared_ptr<Graphics::GLTextureResource>, std::vector<std::shared_ptr<Font::CachedGlyphTypesetting::CharacterType> > > textureBatchTable;
				for (const std::shared_ptr<Font::CachedGlyphTypesetting::CharacterType> &currentCharacter : typesettingCopy->characters)
				{
					if (!currentCharacter->glyphInformation->HasImage())
					{
						continue;
					}
					std::shared_ptr<Graphics::GLTextureResource> characterTexture(currentCharacter->glyphInformation->GetFontImage()->GetTexture());
					if (!characterTexture)
					{
						continue;
					}

					std::unordered_map<std::shared_ptr<Graphics::GLTextureResource>, std::vector<std::shared_ptr<Font::CachedGlyphTypesetting::CharacterType> > >::iterator batchFindIt = textureBatchTable.find(characterTexture);
					if (batchFindIt == textureBatchTable.end())
					{
						std::vector<std::shared_ptr<Font::CachedGlyphTypesetting::CharacterType> > batchList;
						batchList.push_back(currentCharacter);
						textureBatchTable[characterTexture] = batchList;
					}
					else
					{
						batchFindIt->second.push_back(currentCharacter);
					}
				}

				// Do Character Rendering
				UICoordinateType yBaselineOffset = static_cast<UICoordinateType>(m_typesetting->firstLineLogicalHeight * 0.75f - typesettingCopy->baselineOffset);
				Graphics::GLByteARGB effectiveShadowColor(m_shadowColor);
				if (alpha < 1.0f)
				{
					effectiveShadowColor.alpha = static_cast<GLubyte>((float)effectiveShadowColor.alpha * alpha);
				}
				if (m_isRenderShadow && (m_shadowColor.alpha) > 0 && (static_cast<float>(m_shadowOffset.SquareLength()) > kFloatEpsilon))
				{
					// Shadow Rendering
					for (std::unordered_map<std::shared_ptr<Graphics::GLTextureResource>, std::vector<std::shared_ptr<Font::CachedGlyphTypesetting::CharacterType> > >::iterator it = textureBatchTable.begin(); it != textureBatchTable.end(); ++it)
					{
						for (std::shared_ptr<Font::CachedGlyphTypesetting::CharacterType> &currentCharacter : it->second)
						{
							UISimpleRectangular effectiveCharacterRectArea = UISimpleRectangular::FromPositionSize(currentCharacter->effectiveCharacterRect.left + m_shadowOffset.x, currentCharacter->effectiveCharacterRect.top + yBaselineOffset + m_shadowOffset.y, currentCharacter->effectiveCharacterRect.GetWidth(), currentCharacter->effectiveCharacterRect.GetHeight());
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
				for (std::unordered_map<std::shared_ptr<Graphics::GLTextureResource>, std::vector<std::shared_ptr<Font::CachedGlyphTypesetting::CharacterType> > >::iterator it = textureBatchTable.begin(); it != textureBatchTable.end(); ++it)
				{
					for (std::shared_ptr<Font::CachedGlyphTypesetting::CharacterType> &currentCharacter : it->second)
					{
						UISimpleRectangular effectiveCharacterRectArea = UISimpleRectangular::FromPositionSize(currentCharacter->effectiveCharacterRect.left, currentCharacter->effectiveCharacterRect.top + yBaselineOffset, currentCharacter->effectiveCharacterRect.GetWidth(), currentCharacter->effectiveCharacterRect.GetHeight());
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
			UIElementSize TextRenderingComponent::GetVisualSize()
			{
				if (!m_typesetting)
				{
					return UIElementSize();
				}

				return UIElementSize(m_typesetting->boundingBox.GetWidth(), m_typesetting->logicalHeight);
			}

			//------------------------------------------------------------------------------
			void TextRenderingComponent::SetTypesetting(std::shared_ptr<Font::CachedGlyphTypesetting> typesetting)
			{
				m_typesetting = typesetting;
			}
		}
	}
}
