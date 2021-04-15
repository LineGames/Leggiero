////////////////////////////////////////////////////////////////////////////////
// Prefab/UITextInputBoxPrefab.cpp (Leggiero/Modules - LegacyUI)
//
// Text Input Box UI Object Prefab Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UITextInputBoxPrefab.h"

// Standard Library
#include <functional>

// External Library
#include <fmt/format.h>
#include <tinyxml2/tinyxml2.h>

// Leggiero.LegacyUI
#include "../UIObject.h"
#include "../Component/UILayoutComponent.h"
#include "../Component/UISizeSubComponent.h"
#include "../Element/UIElementTextInputBox.h"
#include "../Description/UIDescriptionManager.h"
#include "../Description/UIDescriptionExpression.h"
#include "../Description/UIDescriptionFont.h"
#include "../Description/UIPrefabDescription_Reader.h"
#include "UIPrefabPlaceProcessor.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			namespace Prefab
			{
				//////////////////////////////////////////////////////////////////////////////// TextInputBoxPrefab

				//------------------------------------------------------------------------------
				std::shared_ptr<TextInputBoxPrefab> TextInputBoxPrefab::ReadFromFromXMLElement(ParsingXMLElementType *elem)
				{
					const char *fontClassAttribute = elem->Attribute("fontClass");
					if (fontClassAttribute == nullptr)
					{
						// No Font
						return nullptr;
					}

					const char *widthAttribute = elem->Attribute("width");
					const char *heightAttribute = elem->Attribute("height");
					if (widthAttribute == nullptr || heightAttribute == nullptr)
					{
						// No Touch Size
						return nullptr;
					}

					std::shared_ptr<TextInputBoxPrefab> createdPrefab(std::make_shared<TextInputBoxPrefab>(TypeUtility::ParseVariablePath(fontClassAttribute)));

					// Size
					createdPrefab->m_widthExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(widthAttribute, UIDescriptionValueType::kFloatingPoint));
					createdPrefab->m_heightExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(heightAttribute, UIDescriptionValueType::kFloatingPoint));

					// Clipping
					const char *clippingLeft = elem->Attribute("clipLeft");
					if (clippingLeft != nullptr)
					{
						createdPrefab->m_clippingRectLeftExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(clippingLeft, UIDescriptionValueType::kFloatingPoint));
					}
					const char *clippingTop = elem->Attribute("clipTop");
					if (clippingTop != nullptr)
					{
						createdPrefab->m_clippingRectTopExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(clippingTop, UIDescriptionValueType::kFloatingPoint));
					}
					const char *clippingWidth = elem->Attribute("clipWidth");
					if (clippingWidth != nullptr)
					{
						createdPrefab->m_clippingRectWidthExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(clippingWidth, UIDescriptionValueType::kFloatingPoint));
					}
					const char *clippingHeight = elem->Attribute("clipHeight");
					if (clippingHeight != nullptr)
					{
						createdPrefab->m_clippingRectHeightExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(clippingHeight, UIDescriptionValueType::kFloatingPoint));
					}

					// Text
					const char *lineWidthAttribute = elem->Attribute("lineWidth");
					if (lineWidthAttribute != nullptr)
					{
						createdPrefab->m_textLineWidthExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(lineWidthAttribute, UIDescriptionValueType::kFloatingPoint));

						const char *lineSpaceAttribute = elem->Attribute("lineSpace");
						if (lineSpaceAttribute != nullptr)
						{
							createdPrefab->m_textLineSpaceRatioExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(lineSpaceAttribute, UIDescriptionValueType::kFloatingPoint));
						}

						const char *textAlignAttribute = elem->Attribute("textAlign");
						if (textAlignAttribute != nullptr)
						{
							createdPrefab->m_textBlockTextAlign = Font::ParseTextAlignType(textAlignAttribute);
						}
						const char *lastLineTextAlignAttribute = elem->Attribute("lastLineAlign");
						if (lastLineTextAlignAttribute != nullptr)
						{
							createdPrefab->m_textBlockTextLastLineAlign = Font::ParseTextAlignType(lastLineTextAlignAttribute);
						}
					}

					const char *sizeAttribute = elem->Attribute("size");
					if (sizeAttribute != nullptr)
					{
						createdPrefab->m_textSizeExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(sizeAttribute, UIDescriptionValueType::kFloatingPoint));
					}

					const char *textColorAttribute = elem->Attribute("textColor");
					if (textColorAttribute != nullptr)
					{
						createdPrefab->m_textColorExpression = Expression::CachingExpression<ColorARGBValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(textColorAttribute, UIDescriptionValueType::kColor));
					}
					const char *colorAttribute = elem->Attribute("color");
					if (colorAttribute != nullptr)
					{
						createdPrefab->m_textMultiplyColorExpression = Expression::CachingExpression<ColorARGBValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(colorAttribute, UIDescriptionValueType::kColor));
					}

					const char *strokeWidthAttribute = elem->Attribute("strokeWidth");
					if (strokeWidthAttribute != nullptr)
					{
						createdPrefab->m_textStrokeWidthExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(strokeWidthAttribute, UIDescriptionValueType::kFloatingPoint));
					}
					const char *strokeColorAttribute = elem->Attribute("strokeColor");
					if (strokeColorAttribute != nullptr)
					{
						createdPrefab->m_textStrokeColorExpression = Expression::CachingExpression<ColorARGBValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(strokeColorAttribute, UIDescriptionValueType::kColor));
					}

					{
						const char *widthModifyAddAttribute = elem->Attribute("textWidthModifyAdd");
						if (widthModifyAddAttribute != nullptr)
						{
							createdPrefab->m_textSizeWidthModifyAddExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(widthModifyAddAttribute, UIDescriptionValueType::kFloatingPoint));
						}
						const char *heightModifyAddAttribute = elem->Attribute("textHeightModifyAdd");
						if (heightModifyAddAttribute != nullptr)
						{
							createdPrefab->m_textSizeHeightModifyAddExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(heightModifyAddAttribute, UIDescriptionValueType::kFloatingPoint));
						}

						const char *widthModifyMultiplyAttribute = elem->Attribute("textWidthModifyMultiply");
						if (widthModifyMultiplyAttribute != nullptr)
						{
							createdPrefab->m_textSizeWidthModifyMultiplyExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(widthModifyMultiplyAttribute, UIDescriptionValueType::kFloatingPoint));
						}
						const char *heightModifyMultiplyAttribute = elem->Attribute("textHeightModifyMultiply");
						if (heightModifyMultiplyAttribute != nullptr)
						{
							createdPrefab->m_textSizeHeightModifyMultiplyExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(heightModifyMultiplyAttribute, UIDescriptionValueType::kFloatingPoint));
						}
					}

					const char *parentReferenceRatioXAttribute = elem->Attribute("textParentRefRatioX");
					if (parentReferenceRatioXAttribute != nullptr)
					{
						createdPrefab->m_textLayoutParentReferenceRatioXExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(parentReferenceRatioXAttribute, UIDescriptionValueType::kFloatingPoint));
					}
					const char *parentReferenceRatioYAttribute = elem->Attribute("textParentRefRatioY");
					if (parentReferenceRatioYAttribute != nullptr)
					{
						createdPrefab->m_textLayoutParentReferenceRatioYExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(parentReferenceRatioYAttribute, UIDescriptionValueType::kFloatingPoint));
					}
					const char *parentOffsetXAttribute = elem->Attribute("textParentOffsetX");
					if (parentOffsetXAttribute != nullptr)
					{
						createdPrefab->m_textLayoutParentOffsetXExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(parentOffsetXAttribute, UIDescriptionValueType::kFloatingPoint));
					}
					const char *parentOffsetYAttribute = elem->Attribute("textParentOffsetY");
					if (parentOffsetYAttribute != nullptr)
					{
						createdPrefab->m_textLayoutParentOffsetYExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(parentOffsetYAttribute, UIDescriptionValueType::kFloatingPoint));
					}

					const char *anchorReferenceRatioXAttribute = elem->Attribute("textAnchorRefRatioX");
					if (anchorReferenceRatioXAttribute != nullptr)
					{
						createdPrefab->m_textLayoutAnchorReferenceRatioXExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(anchorReferenceRatioXAttribute, UIDescriptionValueType::kFloatingPoint));
					}
					const char *anchorReferenceRatioYAttribute = elem->Attribute("textAnchorRefRatioY");
					if (anchorReferenceRatioYAttribute != nullptr)
					{
						createdPrefab->m_textLayoutAnchorReferenceRatioYExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(anchorReferenceRatioYAttribute, UIDescriptionValueType::kFloatingPoint));
					}
					const char *anchorOffsetXAttribute = elem->Attribute("textAnchorOffsetX");
					if (anchorOffsetXAttribute != nullptr)
					{
						createdPrefab->m_textLayoutAnchorOffsetXExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(anchorOffsetXAttribute, UIDescriptionValueType::kFloatingPoint));
					}
					const char *anchorOffsetYAttribute = elem->Attribute("textAnchorOffsetY");
					if (anchorOffsetYAttribute != nullptr)
					{
						createdPrefab->m_textLayoutAnchorOffsetYExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(anchorOffsetYAttribute, UIDescriptionValueType::kFloatingPoint));
					}

					// Dialog
					createdPrefab->m_dialogTitleString.ReadFromFromXMLElement(elem, "dlgTitle_");
					createdPrefab->m_dialogMessageString.ReadFromFromXMLElement(elem, "dlgMsg_");

					if (!createdPrefab->m_dialogAcceptButtonString.ReadFromFromXMLElement(elem, "dlgBtnOK_"))
					{
						createdPrefab->m_dialogAcceptButtonString.rawString = "OK";
					}

					if (!createdPrefab->m_dialogCancelButtonString.ReadFromFromXMLElement(elem, "dlgBtnCancel_"))
					{
						createdPrefab->m_dialogCancelButtonString.rawString = "Cancel";
					}

					// Common
					createdPrefab->_ReadObjectCommonDataFromXMLElement(elem);

					return createdPrefab;
				}

				//------------------------------------------------------------------------------
				std::shared_ptr<UIObject> TextInputBoxPrefab::_CreateUIObject(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, std::unordered_map<VariableNameType, std::weak_ptr<UIObject> > &idTable, ComponentPostProcessCreatingContext &postProcessContext, IUIPrefabPlaceProcessor *placeProcessor)
				{
					ObjectEnvironmentPair foundClass(processingContext.Find(m_textClassPath));
					if (!std::get<0>(foundClass))
					{
						// Class Not Found
						return nullptr;
					}
					std::shared_ptr<FontClass> fontClassDescription(std::dynamic_pointer_cast<FontClass>(std::get<0>(foundClass)));
					if (!fontClassDescription)
					{
						// Invalid Class Description
						return nullptr;
					}

					std::shared_ptr<Font::MultiPageFontSetting> fontSetting(fontClassDescription->GetFontSetting(std::get<1>(foundClass)));
					if (!fontSetting)
					{
						// Invalid Font Class
						return nullptr;
					}

					UICoordinateType width = m_widthExpression->Evaluate(processingContext);
					UICoordinateType height = m_heightExpression->Evaluate(processingContext);

					bool isAutoClipping = true;
					UICoordinateType clippingLeft = static_cast<UICoordinateType>(0.0f);
					UICoordinateType clippingTop = static_cast<UICoordinateType>(0.0f);
					UICoordinateType clippingWidth = static_cast<UICoordinateType>(width);
					UICoordinateType clippingHeight = static_cast<UICoordinateType>(height);
					if (m_clippingRectLeftExpression || m_clippingRectTopExpression || m_clippingRectWidthExpression || m_clippingRectHeightExpression)
					{
						isAutoClipping = false;
						clippingLeft = static_cast<UICoordinateType>(m_clippingRectLeftExpression ? m_clippingRectLeftExpression->Evaluate(processingContext) : 0.0f);
						clippingTop = static_cast<UICoordinateType>(m_clippingRectTopExpression ? m_clippingRectTopExpression->Evaluate(processingContext) : 0.0f);
						clippingWidth = static_cast<UICoordinateType>(m_clippingRectWidthExpression ? m_clippingRectWidthExpression->Evaluate(processingContext) : width - clippingLeft);
						clippingHeight = static_cast<UICoordinateType>(m_clippingRectHeightExpression ? m_clippingRectHeightExpression->Evaluate(processingContext) : height - clippingTop);
					}

					const GameDataString dialogTitle(m_dialogTitleString.GetStringValue(processingContext, placeProcessor));
					const GameDataString dialogMessage(m_dialogMessageString.GetStringValue(processingContext, placeProcessor));
					const GameDataString dialogAcceptButton(m_dialogAcceptButtonString.GetStringValue(processingContext, placeProcessor));
					const GameDataString dialogCancelButton(m_dialogCancelButtonString.GetStringValue(processingContext, placeProcessor));

					const GameDataString initialString = "";

					Font::GlyphSizeType fontSize = static_cast<Font::GlyphSizeType>(floor(m_textSizeExpression ? m_textSizeExpression->Evaluate(processingContext)
						: fontClassDescription->GetDefaultSize(std::get<1>(foundClass))));
					Graphics::GLByteARGB fontColor(m_textColorExpression ? m_textColorExpression->Evaluate(processingContext)
						: fontClassDescription->GetDefaultColor(std::get<1>(foundClass)));
					float strokeWidth = static_cast<float>(m_textStrokeWidthExpression ? m_textStrokeWidthExpression->Evaluate(processingContext)
						: fontClassDescription->GetDefaultStrokeWidth(std::get<1>(foundClass)));
					Graphics::GLByteARGB strokeColor(Graphics::GLByteARGB::kTransparent);
					if (strokeWidth > 0.0f)
					{
						strokeColor = m_textStrokeColorExpression ? m_textStrokeColorExpression->Evaluate(processingContext)
							: fontClassDescription->GetDefaultStrokeColor(std::get<1>(foundClass));
					}
					else
					{
						strokeWidth = 0.0f;
					}

					Graphics::GLByteARGB textMultiplyColor(Graphics::GLByteARGB::kWhite);
					if (m_textMultiplyColorExpression)
					{
						textMultiplyColor = m_textMultiplyColorExpression->Evaluate(processingContext);
					}

					std::shared_ptr<Element::UIElementTextInputBox> createdInputBox;
					if (IsTextLineBoxed())
					{
						Graphics::BasicCoordType lineWidth = static_cast<Graphics::BasicCoordType>(m_textLineWidthExpression->Evaluate(processingContext));
						float lineSpaceRatio = static_cast<float>(m_textLineSpaceRatioExpression ? m_textLineSpaceRatioExpression->Evaluate(processingContext) : (float)1.5f);

						Font::GlyphManagerComponent *fontManagerComponent = &processingContext.manager.GetGlyphManager();
						Font::TextAlignType blockTextAlignCopy = m_textBlockTextAlign;
						Font::TextAlignType blockTextLastLineAlignCopy = m_textBlockTextLastLineAlign;
						Element::UIElementTextInputBox::TypesettingGeneratorType generatorFun([fontManagerComponent, fontSetting, fontSize, fontColor, strokeWidth, strokeColor, lineWidth, lineSpaceRatio, blockTextAlignCopy, blockTextLastLineAlignCopy](GameDataString printText) {
							return Font::TypesetCachedGlyphesHorizontalBlock(fontManagerComponent,
								printText,
								fontSetting, fontSize,
								lineWidth, blockTextAlignCopy, blockTextLastLineAlignCopy, lineSpaceRatio,
								Graphics::HorizontalAlignType::kLeft, Graphics::VerticalAlignType::kTop,
								fontColor, strokeWidth, strokeColor);
							});

						if (isAutoClipping)
						{
							createdInputBox = std::make_shared<Element::UIElementTextInputBox>(creatingManager, width, height,
								generatorFun, textMultiplyColor, initialString,
								dialogTitle, dialogMessage, dialogAcceptButton, dialogCancelButton);
						}
						else
						{
							createdInputBox = std::make_shared<Element::UIElementTextInputBox>(creatingManager, width, height,
								clippingLeft, clippingTop, clippingWidth, clippingHeight,
								generatorFun, textMultiplyColor, initialString,
								dialogTitle, dialogMessage, dialogAcceptButton, dialogCancelButton);
						}
					}
					else
					{
						Font::GlyphManagerComponent *fontManagerComponent = &processingContext.manager.GetGlyphManager();
						Element::UIElementTextInputBox::TypesettingGeneratorType generatorFun([fontManagerComponent, fontSetting, fontSize, fontColor, strokeWidth, strokeColor](GameDataString printText) {
							return Font::TypesetCachedGlyphesHorizontalSingleLine(fontManagerComponent,
								printText,
								fontSetting, fontSize, Graphics::HorizontalAlignType::kLeft, Graphics::VerticalAlignType::kTop,
								fontColor, strokeWidth, strokeColor);
							});

						if (isAutoClipping)
						{
							createdInputBox = std::make_shared<Element::UIElementTextInputBox>(creatingManager, width, height,
								generatorFun, textMultiplyColor, initialString,
								dialogTitle, dialogMessage, dialogAcceptButton, dialogCancelButton);
						}
						else
						{
							createdInputBox = std::make_shared<Element::UIElementTextInputBox>(creatingManager, width, height,
								clippingLeft, clippingTop, clippingWidth, clippingHeight,
								generatorFun, textMultiplyColor, initialString,
								dialogTitle, dialogMessage, dialogAcceptButton, dialogCancelButton);
						}
					}
					createdInputBox->InitializeAdjustClippingLayoutAsBox();

					// Text Layout Adjust
					{
						if (m_textSizeWidthModifyAddExpression || m_textSizeHeightModifyAddExpression || m_textSizeWidthModifyMultiplyExpression || m_textSizeHeightModifyMultiplyExpression)
						{
							std::shared_ptr<UIObjectSizeSubComponent> textSizing(std::dynamic_pointer_cast<UIObjectSizeSubComponent>(createdInputBox->GetTextSubUIObject()->GetLayoutComponent()->GetSizeSubComponent()));
							if (textSizing)
							{
								createdInputBox->GetTextSubUIObject()->ChangeLayoutComponent(std::make_shared<UILayoutComponent>(std::make_shared<ModifedSizeSubComponent>(textSizing,
									static_cast<UICoordinateType>(m_textSizeWidthModifyAddExpression ? m_textSizeWidthModifyAddExpression->Evaluate(processingContext) : 0.0f),
									static_cast<UICoordinateType>(m_textSizeHeightModifyAddExpression ? m_textSizeHeightModifyAddExpression->Evaluate(processingContext) : 0.0f),
									static_cast<UICoordinateRatioType>(m_textSizeWidthModifyMultiplyExpression ? m_textSizeWidthModifyMultiplyExpression->Evaluate(processingContext) : 1.0f),
									static_cast<UICoordinateRatioType>(m_textSizeHeightModifyMultiplyExpression ? m_textSizeHeightModifyMultiplyExpression->Evaluate(processingContext) : 1.0f))));
							}
						}

						if (m_textLayoutParentReferenceRatioXExpression || m_textLayoutParentReferenceRatioYExpression || m_textLayoutParentOffsetXExpression || m_textLayoutParentOffsetYExpression
							|| m_textLayoutAnchorReferenceRatioXExpression || m_textLayoutAnchorReferenceRatioYExpression || m_textLayoutAnchorOffsetXExpression || m_textLayoutAnchorOffsetYExpression)
						{
							UILayoutComponent &textLayoutComponent = *createdInputBox->GetTextSubUIObject()->GetLayoutComponent();

							if (m_textLayoutParentReferenceRatioXExpression)
							{
								textLayoutComponent.parentReferenceRatioX = static_cast<UICoordinateRatioType>(m_textLayoutParentReferenceRatioXExpression->Evaluate(processingContext));
							}
							if (m_textLayoutParentReferenceRatioYExpression)
							{
								textLayoutComponent.parentReferenceRatioY = static_cast<UICoordinateRatioType>(m_textLayoutParentReferenceRatioYExpression->Evaluate(processingContext));
							}
							if (m_textLayoutParentOffsetXExpression)
							{
								textLayoutComponent.parentOffsetX = static_cast<UICoordinateType>(m_textLayoutParentOffsetXExpression->Evaluate(processingContext));
							}
							if (m_textLayoutParentOffsetYExpression)
							{
								textLayoutComponent.parentOffsetY = static_cast<UICoordinateType>(m_textLayoutParentOffsetYExpression->Evaluate(processingContext));
							}

							if (m_textLayoutAnchorReferenceRatioXExpression)
							{
								textLayoutComponent.anchorReferenceRatioX = static_cast<UICoordinateRatioType>(m_textLayoutAnchorReferenceRatioXExpression->Evaluate(processingContext));
							}
							if (m_textLayoutAnchorReferenceRatioYExpression)
							{
								textLayoutComponent.anchorReferenceRatioY = static_cast<UICoordinateRatioType>(m_textLayoutAnchorReferenceRatioYExpression->Evaluate(processingContext));
							}
							if (m_textLayoutAnchorOffsetXExpression)
							{
								textLayoutComponent.anchorOffsetX = static_cast<UICoordinateType>(m_textLayoutAnchorOffsetXExpression->Evaluate(processingContext));
							}
							if (m_textLayoutAnchorOffsetYExpression)
							{
								textLayoutComponent.anchorOffsetY = static_cast<UICoordinateType>(m_textLayoutAnchorOffsetYExpression->Evaluate(processingContext));
							}
						}
					}

					return createdInputBox;
				}
			}
		}
	}
}
