////////////////////////////////////////////////////////////////////////////////
// Element/UIElementCachedVariantText.cpp (Leggiero/Modules - LegacyUI)
//
// Cached variant UI text element implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIElementCachedVariantText.h"

// Standard Library
#include <unordered_set>

// Leggiero.Graphics
#include <Graphics/Texture/TextureSection.h>
#include <Graphics/Texture/RuntimeTextureAtlasEntry.h>

// Leggiero.Font
#include <Font/GlyphManagerComponent.h>
#include <Font/Common/CachedGlyph.h>
#include <Font/Common/MultiPageFontSetting.h>

// Leggiero.LegacyUI
#include "../Component/UILayoutComponent.h"
#include "../Component/UISizeSubComponent.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Element
		{
			//////////////////////////////////////////////////////////////////////////////// UIElementCachedVariantText

			//------------------------------------------------------------------------------
			UIElementCachedVariantText::GlyphGeneratorType UIElementCachedVariantText::GlyphGeneratorFuncGen(Font::GlyphManagerComponent *fontManager,
				std::shared_ptr<Font::MultiPageFontSetting> fontSettting, Font::GlyphSizeType size,
				Graphics::GLByteARGB color, float strokeWidth, Graphics::GLByteARGB strokeColor,
				bool isFitGrid, Font::TextureType textureType)
			{
				if (fontManager == nullptr || !fontSettting || size <= (Font::GlyphSizeType)kFloatEpsilon
					|| (color.alpha == 0 && (strokeColor.alpha == 0 || strokeWidth < kFloatEpsilon)))
				{
					return ([](Font::GlyphCodePointType codepoint)
						{
							return Rendering::CachedTextRenderingComponent::DrawingCharacter(nullptr);
						});
				}

				return ([fontManager, fontSettting, size, color, strokeWidth, strokeColor, isFitGrid, textureType](Font::GlyphCodePointType codepoint)
					{
						bool isStroked = false;
						if (strokeWidth > 0.0f && strokeColor.alpha > 0)
						{
							isStroked = true;
						}

						Graphics::GLByteARGB effectiveDrawColor = Graphics::GLByteARGB::kWhite;
						Graphics::GLByteARGB effectiveStrokeColor = strokeColor;
						Graphics::GLByteARGB multiplyColor = color;
						if (color.alpha == 255)
						{
							if (isStroked)
							{
								if (color == strokeColor)
								{
									effectiveStrokeColor = Graphics::GLByteARGB::kWhite;
								}
								else if (strokeColor.red == 0 && strokeColor.green == 0 && strokeColor.blue == 0)
								{
									// Keep setting
								}
								else if (color.red == 0 && color.green == 0 && color.blue == 0)
								{
									multiplyColor = strokeColor;
									effectiveStrokeColor = Graphics::GLByteARGB::kWhite;
									effectiveDrawColor = color;
								}
								else
								{
									effectiveDrawColor = color;
									effectiveStrokeColor = strokeColor;
									multiplyColor = Graphics::GLByteARGB::kWhite;
								}
							}
						}
						else
						{
							effectiveDrawColor = color;
							multiplyColor = Graphics::GLByteARGB::kWhite;
						}

						// Render Glyph
						Font::MultiPageFontSetting::FontSetting applyingSetting = fontSettting->GetSettingForGlyph(codepoint);
						if (!applyingSetting.IsValid())
						{
							applyingSetting = fontSettting->GetTofuSetting();
							if (!applyingSetting.IsValid())
							{
								return Rendering::CachedTextRenderingComponent::DrawingCharacter(nullptr);
							}
						}

						Font::GlyphSizeType effectiveSize = (Font::GlyphSizeType)(size * applyingSetting.scaleMultiply);
						Font::GlyphStyleType effectiveStyle = Font::GlyphStyleType::kNone;
						if (applyingSetting.isForceBold)
						{
							effectiveStyle |= Font::GlyphStyleType::kBold;
						}
						if (applyingSetting.isForceItalic)
						{
							effectiveStyle |= Font::GlyphStyleType::kItalic;
						}

						std::shared_ptr<Font::CachedGlyph> renderedGlyph =
							fontManager->LoadGlyph(applyingSetting.face, codepoint, effectiveSize, applyingSetting.horizontalRatioMultiply, effectiveStyle, effectiveDrawColor, textureType, strokeWidth, effectiveStrokeColor);
						if (renderedGlyph)
						{
							Rendering::CachedTextRenderingComponent::DrawingCharacter resultGlyph(renderedGlyph, UIVector2D::kZero, multiplyColor);
							if (applyingSetting.isForceMonospacing)
							{
								resultGlyph.advance = (UICoordinateType)(size * applyingSetting.monospacingWidthRatio);
								resultGlyph.additionalOffset.x += (UICoordinateType)((resultGlyph.advance - (UICoordinateType)renderedGlyph->GetAdvanceX()) * 0.5f);
							}
							else
							{
								resultGlyph.advance = (UICoordinateType)renderedGlyph->GetAdvanceX();
							}
							return resultGlyph;
						}
						return Rendering::CachedTextRenderingComponent::DrawingCharacter(nullptr);
					});
			}

			//------------------------------------------------------------------------------
			UIElementCachedVariantText::UIElementCachedVariantText(std::shared_ptr<UIManager> ownerManager, GlyphGeneratorType glyphGeneratorFunc, UICoordinateType logicalHeight, const GameDataString &initialString,
				bool isRenderShadow, const Graphics::GLByteARGB &shadowColor, const UIVector2D &shadowOffset,
				const Graphics::GLByteARGB &multiplyColor,
				const GameDataString &initialCachingCharacters)
				: UIObject(ownerManager)
				, m_glyphGeneratorFunc(glyphGeneratorFunc)
				, m_isDirty(true), m_toChangeString(initialString)
			{
				m_textRenderingComponent = std::make_shared<Rendering::CachedTextRenderingComponent>(m_cachedTextDrawing, logicalHeight, isRenderShadow, shadowColor, shadowOffset, multiplyColor);
				RegisterComponent(m_textRenderingComponent);

				GameDataString::const_iterator characterIt = initialCachingCharacters.cbegin();
				while (characterIt != initialCachingCharacters.cend())
				{
					uint32_t codepoint = 0;
					try
					{
						codepoint = utf8::next(characterIt, initialCachingCharacters.cend());
					}
					catch (utf8::not_enough_room ex)
					{
						// String End
						break;
					}
					m_cacheWaitQueue.push_back(codepoint);
				}
			}

			//------------------------------------------------------------------------------
			UIElementCachedVariantText::~UIElementCachedVariantText()
			{
			}

			//------------------------------------------------------------------------------
			void UIElementCachedVariantText::Prepare()
			{
				_ResolveCacheRegister();

				if (!m_isDirty)
				{
					return;
				}
				_DoTypesetting();
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UIObject> UIElementCachedVariantText::Clone() const
			{
				std::shared_ptr<UIManager> managerCopy(m_ownerManager.lock());
				if (!managerCopy)
				{
					return nullptr;
				}

				std::shared_ptr<UIElementCachedVariantText> clonedObject(new UIElementCachedVariantText(managerCopy, m_glyphGeneratorFunc, GetLogicalHeight(), m_toChangeString,
					m_textRenderingComponent->HasShadow(), m_textRenderingComponent->GetShadowColor(), m_textRenderingComponent->GetShadowOffset(), m_textRenderingComponent->GetGlobalMultiplyColor()));

				// Clone UIObject Data
				clonedObject->m_isVisible = m_isVisible;
				clonedObject->m_isUpdating = m_isUpdating;
				clonedObject->m_isEnable = m_isEnable;
				clonedObject->m_isParentEnable = m_isParentEnable;

				clonedObject->ChangeLayoutComponent(std::dynamic_pointer_cast<UILayoutComponent>(m_layoutComponent->Clone(*clonedObject)));

				// Copy Object and Tree
				std::unordered_set<UIComponentType, Utility::SyntacticSugar::EnumClassHash> copiedComponentSet;
				copiedComponentSet.insert(UIComponentType::kRendering);
				while (copiedComponentSet.size() < m_components.size())
				{
					bool isCopiedLoop = false;
					for (std::unordered_map<UIComponentType, std::shared_ptr<IUIComponent>, Utility::SyntacticSugar::EnumClassHash>::const_iterator it = m_components.begin(); it != m_components.end(); ++it)
					{
						if (copiedComponentSet.find(it->first) != copiedComponentSet.end())
						{
							continue;
						}

						const std::vector<UIComponentType> &dependencyList = it->second->GetDependentComponentTypes();
						bool isUnResolvedDependencyExists = false;
						for (const UIComponentType &dependingComponentType : dependencyList)
						{
							if (copiedComponentSet.find(dependingComponentType) == copiedComponentSet.end())
							{
								isUnResolvedDependencyExists = true;
								break;
							}
						}
						if (isUnResolvedDependencyExists)
						{
							continue;
						}

						isCopiedLoop = true;
						copiedComponentSet.insert(it->first);
						std::shared_ptr<IUIComponent> clonedComponent(it->second->Clone(*clonedObject));
						if (clonedComponent)
						{
							clonedObject->m_components[it->first] = clonedComponent;
						}
					}
					if (!isCopiedLoop)
					{
						// Cyclic Component Dependency Exists
						break;
					}
				}

				clonedObject->m_preChildren.reserve(m_preChildren.size());
				for (const std::shared_ptr<UIObject> &currentChild : m_preChildren)
				{
					std::shared_ptr<UIObject> clonedChild(currentChild->Clone());
					if (clonedChild)
					{
						clonedChild->UpdateParent(*clonedObject);
						clonedObject->m_preChildren.push_back((clonedChild));
					}
				}
				clonedObject->m_postChildren.reserve(m_postChildren.size());
				for (const std::shared_ptr<UIObject> &currentChild : m_postChildren)
				{
					std::shared_ptr<UIObject> clonedChild(currentChild->Clone());
					if (clonedChild)
					{
						clonedChild->UpdateParent(*clonedObject);
						clonedObject->m_postChildren.push_back((clonedChild));
					}
				}

				return clonedObject;
			}

			//------------------------------------------------------------------------------
			void UIElementCachedVariantText::GraphicPrepare()
			{
				_ResolveCacheRegister();

				if (!m_isDirty)
				{
					return;
				}
				_DoTypesetting();
			}

			//------------------------------------------------------------------------------
			void UIElementCachedVariantText::FrameGraphicPrepare(const UIRenderer &renderer, UIVector2D offsetPosition, const IUIClipping &effectiveClipping)
			{
				Prepare();
			}

			//------------------------------------------------------------------------------
			UIElementSize UIElementCachedVariantText::CalculateSize()
			{
				return m_textRenderingComponent->GetVisualSize();
			}

			//------------------------------------------------------------------------------
			UICoordinateType UIElementCachedVariantText::CalculateWidth()
			{
				return CalculateSize().width;
			}

			//------------------------------------------------------------------------------
			UICoordinateType UIElementCachedVariantText::CalculateHeight()
			{
				return CalculateSize().height;
			}

			//------------------------------------------------------------------------------
			void UIElementCachedVariantText::SetString(const GameDataString &stringToSet)
			{
				if (m_isDirty)
				{
					m_toChangeString = stringToSet;
				}
				else
				{
					if (m_currentString == stringToSet)
					{
						return;
					}
					m_isDirty = true;
					m_toChangeString = stringToSet;
				}
			}

			//------------------------------------------------------------------------------
			void UIElementCachedVariantText::SetString(GameDataString &&stringToSet)
			{
				if (m_isDirty)
				{
					m_toChangeString = stringToSet;
				}
				else
				{
					if (m_currentString == stringToSet)
					{
						return;
					}
					m_isDirty = true;
					m_toChangeString = stringToSet;
				}
			}

			//------------------------------------------------------------------------------
			void UIElementCachedVariantText::SetMultiplyColor(Graphics::GLByteARGB multiplyColor)
			{
				m_textRenderingComponent->SetGlobalMultiplyColor(multiplyColor);
			}

			//------------------------------------------------------------------------------
			const Graphics::GLByteARGB UIElementCachedVariantText::GetMultiplyColor() const
			{
				return m_textRenderingComponent->GetGlobalMultiplyColor();
			}

			//------------------------------------------------------------------------------
			void UIElementCachedVariantText::SetLogicalHeight(UICoordinateType logicalHeight)
			{
				m_textRenderingComponent->SetLogicalHeight(logicalHeight);
			}

			//------------------------------------------------------------------------------
			UICoordinateType UIElementCachedVariantText::GetLogicalHeight() const
			{
				return m_textRenderingComponent->GetLogicalHeight();
			}

			//------------------------------------------------------------------------------
			void UIElementCachedVariantText::TurnShadow(bool isTurnOn)
			{
				if (!m_textRenderingComponent)
				{
					return;
				}
				m_textRenderingComponent->TurnShadow(isTurnOn);
			}

			//------------------------------------------------------------------------------
			void UIElementCachedVariantText::SetShadow(const Graphics::GLByteARGB &shadowColor, const UIVector2D &shadowOffset)
			{
				if (!m_textRenderingComponent)
				{
					return;
				}
				m_textRenderingComponent->SetShadow(shadowColor, shadowOffset);
			}

			//------------------------------------------------------------------------------
			void UIElementCachedVariantText::_ResolveCacheRegister()
			{
				for (const Font::GlyphCodePointType &codepoint : m_cacheWaitQueue)
				{
					if (m_glyphCache.find(codepoint) != m_glyphCache.end())
					{
						continue;
					}
					m_glyphCache.insert(std::pair<Font::GlyphCodePointType, Rendering::CachedTextRenderingComponent::DrawingCharacter>(codepoint, m_glyphGeneratorFunc(codepoint)));
				}
				m_cacheWaitQueue.clear();
			}

			//------------------------------------------------------------------------------
			void UIElementCachedVariantText::_DoTypesetting()
			{
				if (m_toChangeString.length() == 0)
				{
					m_currentString = m_toChangeString;
					m_cachedTextDrawing.clear();
					m_textRenderingComponent->SetText(m_cachedTextDrawing, m_textRenderingComponent->GetLogicalHeight());
					m_isDirty = false;
					return;
				}

				std::vector<Font::GlyphCodePointType> codepointString; codepointString.reserve(m_toChangeString.length());
				std::unordered_set<Font::GlyphCodePointType> needToBakeCodepointSet;
				GameDataString::iterator characterIt = m_toChangeString.begin();
				while (characterIt != m_toChangeString.end())
				{
					uint32_t codepoint = 0;
					try
					{
						codepoint = utf8::next(characterIt, m_toChangeString.end());
					}
					catch (utf8::not_enough_room ex)
					{
						// String End
						break;
					}
					codepointString.push_back(codepoint);
					if (m_glyphCache.find(codepoint) == m_glyphCache.end())
					{
						if (needToBakeCodepointSet.find(codepoint) == needToBakeCodepointSet.end())
						{
							needToBakeCodepointSet.insert(codepoint);
						}
					}
				}
				if (!needToBakeCodepointSet.empty())
				{
					for (Font::GlyphCodePointType codepoint : needToBakeCodepointSet)
					{
						m_cacheWaitQueue.push_back(codepoint);
					}
					_ResolveCacheRegister();
				}

				m_cachedTextDrawing.clear();
				UIVector2D currentOrigin(UIVector2D::kZero);
				for (Font::GlyphCodePointType codepoint : codepointString)
				{
					std::unordered_map<Font::GlyphCodePointType, Rendering::CachedTextRenderingComponent::DrawingCharacter>::iterator cacheFindIt = m_glyphCache.find(codepoint);
					if (cacheFindIt == m_glyphCache.end())
					{
						continue;
					}

					Rendering::CachedTextRenderingComponent::DrawingCharacter currentDrawingCharacter(cacheFindIt->second);
					currentDrawingCharacter.characterOrigin = currentOrigin;
					currentDrawingCharacter.characterOrigin.x += currentDrawingCharacter.glyphInformation->GetOffsetX() + currentDrawingCharacter.additionalOffset.x;
					currentDrawingCharacter.characterOrigin.y -= currentDrawingCharacter.glyphInformation->GetOffsetY() - currentDrawingCharacter.additionalOffset.y;
					currentOrigin.x += currentDrawingCharacter.advance;
					m_cachedTextDrawing.push_back(currentDrawingCharacter);
				}

				m_currentString = m_toChangeString;
				m_textRenderingComponent->SetText(m_cachedTextDrawing, m_textRenderingComponent->GetLogicalHeight());
				m_isDirty = false;
			}
		}
	}
}
