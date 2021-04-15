////////////////////////////////////////////////////////////////////////////////
// Prefab/UITextObjectPrefab.h (Leggiero/Modules - LegacyUI)
//
// Text UI Object Prefabs Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UITextObjectPrefab.h"

// Standard Library
#include <functional>

// External Library
#include <tinyxml2/tinyxml2.h>

// Leggiero.LegacyUI
#include "../UIObject.h"
#include "../Component/UILayoutComponent.h"
#include "../Component/UISizeSubComponent.h"
#include "../Element/UIElementFixedText.h"
#include "../Element/UIElementVariantText.h"
#include "../Element/UIElementCachedVariantText.h"
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
				//////////////////////////////////////////////////////////////////////////////// TextLabelPrefab

				//------------------------------------------------------------------------------
				std::shared_ptr<TextLabelPrefab> TextLabelPrefab::ReadFromFromXMLElement(ParsingXMLElementType *elem)
				{
					const char *fontClassAttribute = elem->Attribute("fontClass");
					if (fontClassAttribute == nullptr)
					{
						// No Font
						return nullptr;
					}

					std::shared_ptr<TextLabelPrefab> createdPrefab(std::make_shared<TextLabelPrefab>(TypeUtility::ParseVariablePath(fontClassAttribute)));

					// String Source
					const char *stringKeyAttribute = elem->Attribute("string");
					if (stringKeyAttribute == nullptr)
					{
						const char *generateKeyAttribute = elem->Attribute("generateKey");
						if (generateKeyAttribute == nullptr)
						{
							const char *stringVariablePathAttribute = elem->Attribute("variable");
							if (stringVariablePathAttribute == nullptr)
							{
								const char *inlineStringExpressionAttribute = elem->Attribute("inline");
								if (inlineStringExpressionAttribute == nullptr)
								{
									const char *rawStringAttribute = elem->Attribute("raw");
									if (rawStringAttribute == nullptr)
									{
										// No String to Show
										return nullptr;
									}
									else
									{
										createdPrefab->m_sourceString.stringSourceType = StringSourceType::kRawString;
										createdPrefab->m_sourceString.rawString = rawStringAttribute;
									}
								}
								else
								{
									createdPrefab->m_sourceString.stringSourceType = StringSourceType::kInline;
									createdPrefab->m_sourceString.inlineStringExpression = Expression::CachingExpression<StringValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(inlineStringExpressionAttribute, UIDescriptionValueType::kString));
								}
							}
							else
							{
								createdPrefab->m_sourceString.stringSourceType = StringSourceType::kVariable;
								createdPrefab->m_sourceString.stringVariablePath = TypeUtility::ParseVariablePath(stringVariablePathAttribute);
							}
						}
						else
						{
							createdPrefab->m_sourceString.stringSourceType = StringSourceType::kRuntimeGenerating;
							createdPrefab->m_sourceString.generateKey = generateKeyAttribute;
						}
					}
					else
					{
						createdPrefab->m_sourceString.stringSourceType = StringSourceType::kTextInBag;
						createdPrefab->m_sourceString.stringKey = stringKeyAttribute;
					}

					const char *lineWidthAttribute = elem->Attribute("lineWidth");
					if (lineWidthAttribute != nullptr)
					{
						createdPrefab->m_lineWidthExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(lineWidthAttribute, UIDescriptionValueType::kFloatingPoint));

						const char *lineSpaceAttribute = elem->Attribute("lineSpace");
						if (lineSpaceAttribute != nullptr)
						{
							createdPrefab->m_lineSpaceRatioExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(lineSpaceAttribute, UIDescriptionValueType::kFloatingPoint));
						}

						const char *textAlignAttribute = elem->Attribute("textAlign");
						if (textAlignAttribute != nullptr)
						{
							createdPrefab->m_blockTextAlign = Font::ParseTextAlignType(textAlignAttribute);
						}
						const char *lastLineTextAlignAttribute = elem->Attribute("lastLineAlign");
						if (lastLineTextAlignAttribute != nullptr)
						{
							createdPrefab->m_blockTextLastLineAlign = Font::ParseTextAlignType(lastLineTextAlignAttribute);
						}
					}

					const char *sizeAttribute = elem->Attribute("size");
					if (sizeAttribute != nullptr)
					{
						createdPrefab->m_sizeExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(sizeAttribute, UIDescriptionValueType::kFloatingPoint));
					}

					const char *colorAttribute = elem->Attribute("color");
					if (colorAttribute != nullptr)
					{
						createdPrefab->m_colorExpression = Expression::CachingExpression<ColorARGBValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(colorAttribute, UIDescriptionValueType::kColor));
					}

					const char *strokeWidthAttribute = elem->Attribute("strokeWidth");
					if (strokeWidthAttribute != nullptr)
					{
						createdPrefab->m_strokeWidthExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(strokeWidthAttribute, UIDescriptionValueType::kFloatingPoint));
					}
					const char *strokeColorAttribute = elem->Attribute("strokeColor");
					if (strokeColorAttribute != nullptr)
					{
						createdPrefab->m_strokeColorExpression = Expression::CachingExpression<ColorARGBValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(strokeColorAttribute, UIDescriptionValueType::kColor));
					}

					const char *shadowOffsetXAttribute = elem->Attribute("shadowOffsetX");
					if (shadowOffsetXAttribute != nullptr)
					{
						createdPrefab->m_shadowOffsetX = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(shadowOffsetXAttribute, UIDescriptionValueType::kFloatingPoint));
					}
					const char *shadowOffsetYAttribute = elem->Attribute("shadowOffsetY");
					if (shadowOffsetYAttribute != nullptr)
					{
						createdPrefab->m_shadowOffsetY = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(shadowOffsetYAttribute, UIDescriptionValueType::kFloatingPoint));
					}
					const char *shadowColorAttribute = elem->Attribute("shadowColor");
					if (shadowColorAttribute != nullptr)
					{
						createdPrefab->m_shadowColorExpression = Expression::CachingExpression<ColorARGBValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(shadowColorAttribute, UIDescriptionValueType::kColor));
					}

					createdPrefab->_ReadObjectCommonDataFromXMLElement(elem);

					return createdPrefab;
				}

				//------------------------------------------------------------------------------
				std::shared_ptr<UIObject> TextLabelPrefab::_CreateUIObject(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, std::unordered_map<VariableNameType, std::weak_ptr<UIObject> > &idTable, ComponentPostProcessCreatingContext &postProcessContext, IUIPrefabPlaceProcessor *placeProcessor)
				{
					ObjectEnvironmentPair foundClass(processingContext.Find(m_classPath));
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

					GameDataString stringToPrint(_GetStringValue(processingContext, placeProcessor));

					Font::GlyphSizeType fontSize = static_cast<Font::GlyphSizeType>(floor(m_sizeExpression ? m_sizeExpression->Evaluate(processingContext)
						: fontClassDescription->GetDefaultSize(std::get<1>(foundClass))));
					Graphics::GLByteARGB fontColor(m_colorExpression ? m_colorExpression->Evaluate(processingContext)
						: fontClassDescription->GetDefaultColor(std::get<1>(foundClass)));
					float strokeWidth = static_cast<float>(m_strokeWidthExpression ? m_strokeWidthExpression->Evaluate(processingContext)
						: fontClassDescription->GetDefaultStrokeWidth(std::get<1>(foundClass)));
					Graphics::GLByteARGB strokeColor(Graphics::GLByteARGB::kTransparent);
					if (strokeWidth > 0.0f)
					{
						strokeColor = m_strokeColorExpression ? m_strokeColorExpression->Evaluate(processingContext)
							: fontClassDescription->GetDefaultStrokeColor(std::get<1>(foundClass));
					}
					else
					{
						strokeWidth = 0.0f;
					}

					std::shared_ptr<Font::CachedGlyphTypesetting> typesetting;
					if (IsLineBoxed())
					{
						Graphics::BasicCoordType lineWidth = static_cast<Graphics::BasicCoordType>(m_lineWidthExpression->Evaluate(processingContext));
						float lineSpaceRatio = static_cast<float>(m_lineSpaceRatioExpression ? m_lineSpaceRatioExpression->Evaluate(processingContext) : (float)1.5f);

						typesetting = Font::TypesetCachedGlyphesHorizontalBlock(&processingContext.manager.GetGlyphManager(),
							stringToPrint,
							fontSetting, fontSize,
							lineWidth, m_blockTextAlign, m_blockTextLastLineAlign, lineSpaceRatio,
							Graphics::HorizontalAlignType::kLeft, Graphics::VerticalAlignType::kTop,
							fontColor, strokeWidth, strokeColor);
					}
					else
					{
						typesetting = Font::TypesetCachedGlyphesHorizontalSingleLine(&processingContext.manager.GetGlyphManager(),
							stringToPrint,
							fontSetting, fontSize, Graphics::HorizontalAlignType::kLeft, Graphics::VerticalAlignType::kTop,
							fontColor, strokeWidth, strokeColor);
					}
					if (typesetting)
					{
						UICoordinateType shadowOffsetX = static_cast<UICoordinateType>(m_shadowOffsetX ? m_shadowOffsetX->Evaluate(processingContext) : static_cast<float>(0.0f));
						UICoordinateType shadowOffsetY = static_cast<UICoordinateType>(m_shadowOffsetY ? m_shadowOffsetY->Evaluate(processingContext) : static_cast<float>(0.0f));
						bool hasShadow = ((shadowOffsetX * shadowOffsetX + shadowOffsetY * shadowOffsetY) > kFloatEpsilon);
						Graphics::GLByteARGB shadowColor(Graphics::GLByteARGB::kTransparent);
						if (hasShadow)
						{
							shadowColor = m_shadowColorExpression ? m_shadowColorExpression->Evaluate(processingContext) : Graphics::GLByteARGB::kBlack;
						}

						return std::make_shared<Element::UIElementFixedText>(creatingManager, typesetting, hasShadow, shadowColor, UIVector2D(shadowOffsetX, shadowOffsetY));
					}
					return nullptr;
				}

				//------------------------------------------------------------------------------
				GameDataString TextLabelPrefab::_GetStringValue(DescriptionProcessingContext &processingContext, IUIPrefabPlaceProcessor *placeProcessor)
				{
					return m_sourceString.GetStringValue(processingContext, placeProcessor);
				}


				//////////////////////////////////////////////////////////////////////////////// TextPrintPrefab

				//------------------------------------------------------------------------------
				std::shared_ptr<TextPrintPrefab> TextPrintPrefab::ReadFromFromXMLElement(ParsingXMLElementType *elem)
				{
					const char *fontClassAttribute = elem->Attribute("fontClass");
					if (fontClassAttribute == nullptr)
					{
						// No Font
						return nullptr;
					}

					const char *placeholderAttribute = "";

					// Insert Placeholder String only in Debug Mode
					#ifdef _LEGGIERO_DEBUG
					placeholderAttribute = elem->Attribute("placeholder");
					if (placeholderAttribute == nullptr)
					{
						placeholderAttribute = "";
					}
					#endif

					std::shared_ptr<TextPrintPrefab> createdPrefab(std::make_shared<TextPrintPrefab>(TypeUtility::ParseVariablePath(fontClassAttribute), placeholderAttribute));

					const char *lineWidthAttribute = elem->Attribute("lineWidth");
					if (lineWidthAttribute != nullptr)
					{
						createdPrefab->m_lineWidthExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(lineWidthAttribute, UIDescriptionValueType::kFloatingPoint));

						const char *lineSpaceAttribute = elem->Attribute("lineSpace");
						if (lineSpaceAttribute != nullptr)
						{
							createdPrefab->m_lineSpaceRatioExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(lineSpaceAttribute, UIDescriptionValueType::kFloatingPoint));
						}

						const char *textAlignAttribute = elem->Attribute("textAlign");
						if (textAlignAttribute != nullptr)
						{
							createdPrefab->m_blockTextAlign = Font::ParseTextAlignType(textAlignAttribute);
						}
						const char *lastLineTextAlignAttribute = elem->Attribute("lastLineAlign");
						if (lastLineTextAlignAttribute != nullptr)
						{
							createdPrefab->m_blockTextLastLineAlign = Font::ParseTextAlignType(lastLineTextAlignAttribute);
						}
					}

					const char *sizeAttribute = elem->Attribute("size");
					if (sizeAttribute != nullptr)
					{
						createdPrefab->m_sizeExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(sizeAttribute, UIDescriptionValueType::kFloatingPoint));
					}

					const char *colorAttribute = elem->Attribute("color");
					if (colorAttribute != nullptr)
					{
						createdPrefab->m_colorExpression = Expression::CachingExpression<ColorARGBValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(colorAttribute, UIDescriptionValueType::kColor));
					}

					const char *strokeWidthAttribute = elem->Attribute("strokeWidth");
					if (strokeWidthAttribute != nullptr)
					{
						createdPrefab->m_strokeWidthExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(strokeWidthAttribute, UIDescriptionValueType::kFloatingPoint));
					}
					const char *strokeColorAttribute = elem->Attribute("strokeColor");
					if (strokeColorAttribute != nullptr)
					{
						createdPrefab->m_strokeColorExpression = Expression::CachingExpression<ColorARGBValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(strokeColorAttribute, UIDescriptionValueType::kColor));
					}

					const char *shadowOffsetXAttribute = elem->Attribute("shadowOffsetX");
					if (shadowOffsetXAttribute != nullptr)
					{
						createdPrefab->m_shadowOffsetX = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(shadowOffsetXAttribute, UIDescriptionValueType::kFloatingPoint));
					}
					const char *shadowOffsetYAttribute = elem->Attribute("shadowOffsetY");
					if (shadowOffsetYAttribute != nullptr)
					{
						createdPrefab->m_shadowOffsetY = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(shadowOffsetYAttribute, UIDescriptionValueType::kFloatingPoint));
					}
					const char *shadowColorAttribute = elem->Attribute("shadowColor");
					if (shadowColorAttribute != nullptr)
					{
						createdPrefab->m_shadowColorExpression = Expression::CachingExpression<ColorARGBValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(shadowColorAttribute, UIDescriptionValueType::kColor));
					}

					createdPrefab->_ReadObjectCommonDataFromXMLElement(elem);

					return createdPrefab;
				}

				//------------------------------------------------------------------------------
				std::shared_ptr<UIObject> TextPrintPrefab::_CreateUIObject(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, std::unordered_map<VariableNameType, std::weak_ptr<UIObject> > &idTable, ComponentPostProcessCreatingContext &postProcessContext, IUIPrefabPlaceProcessor *placeProcessor)
				{
					ObjectEnvironmentPair foundClass(processingContext.Find(m_classPath));
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

					GameDataString initialString;

					// Insert Placeholder String only in Debug Mode
					#ifdef _LEGGIERO_DEBUG
					initialString = m_placeholderString;
					#endif

					Font::GlyphSizeType fontSize = static_cast<Font::GlyphSizeType>(floor(m_sizeExpression ? m_sizeExpression->Evaluate(processingContext)
						: fontClassDescription->GetDefaultSize(std::get<1>(foundClass))));
					Graphics::GLByteARGB fontColor(m_colorExpression ? m_colorExpression->Evaluate(processingContext)
						: fontClassDescription->GetDefaultColor(std::get<1>(foundClass)));
					float strokeWidth = static_cast<float>(m_strokeWidthExpression ? m_strokeWidthExpression->Evaluate(processingContext)
						: fontClassDescription->GetDefaultStrokeWidth(std::get<1>(foundClass)));
					Graphics::GLByteARGB strokeColor(Graphics::GLByteARGB::kTransparent);
					if (strokeWidth > 0.0f)
					{
						strokeColor = m_strokeColorExpression ? m_strokeColorExpression->Evaluate(processingContext)
							: fontClassDescription->GetDefaultStrokeColor(std::get<1>(foundClass));
					}
					else
					{
						strokeWidth = 0.0f;
					}

					UICoordinateType shadowOffsetX = static_cast<UICoordinateType>(m_shadowOffsetX ? m_shadowOffsetX->Evaluate(processingContext) : static_cast<float>(0.0f));
					UICoordinateType shadowOffsetY = static_cast<UICoordinateType>(m_shadowOffsetY ? m_shadowOffsetY->Evaluate(processingContext) : static_cast<float>(0.0f));
					bool hasShadow = ((shadowOffsetX * shadowOffsetX + shadowOffsetY * shadowOffsetY) > kFloatEpsilon);
					Graphics::GLByteARGB shadowColor(Graphics::GLByteARGB::kTransparent);
					if (hasShadow)
					{
						shadowColor = m_shadowColorExpression ? m_shadowColorExpression->Evaluate(processingContext) : Graphics::GLByteARGB::kBlack;
					}

					if (IsLineBoxed())
					{
						Graphics::BasicCoordType lineWidth = static_cast<Graphics::BasicCoordType>(m_lineWidthExpression->Evaluate(processingContext));
						float lineSpaceRatio = static_cast<float>(m_lineSpaceRatioExpression ? m_lineSpaceRatioExpression->Evaluate(processingContext) : (float)1.5f);

						Font::GlyphManagerComponent *fontManagerComponent = &processingContext.manager.GetGlyphManager();
						Font::TextAlignType blockTextAlignCopy = m_blockTextAlign;
						Font::TextAlignType blockTextLastLineAlignCopy = m_blockTextLastLineAlign;
						Element::UIElementVariantText::TypesettingGeneratorType generatorFun([fontManagerComponent, fontSetting, fontSize, fontColor, strokeWidth, strokeColor, lineWidth, lineSpaceRatio, blockTextAlignCopy, blockTextLastLineAlignCopy](GameDataString printText) {
							return Font::TypesetCachedGlyphesHorizontalBlock(fontManagerComponent,
								printText,
								fontSetting, fontSize,
								lineWidth, blockTextAlignCopy, blockTextLastLineAlignCopy, lineSpaceRatio,
								Graphics::HorizontalAlignType::kLeft, Graphics::VerticalAlignType::kTop,
								fontColor, strokeWidth, strokeColor);
							});

						return std::make_shared<Element::UIElementVariantText>(creatingManager, generatorFun, initialString,
							hasShadow, shadowColor, UIVector2D(shadowOffsetX, shadowOffsetY));
					}
					else
					{
						Font::GlyphManagerComponent *fontManagerComponent = &processingContext.manager.GetGlyphManager();
						Element::UIElementVariantText::TypesettingGeneratorType generatorFun([fontManagerComponent, fontSetting, fontSize, fontColor, strokeWidth, strokeColor](GameDataString printText) {
							return Font::TypesetCachedGlyphesHorizontalSingleLine(fontManagerComponent,
								printText,
								fontSetting, fontSize, Graphics::HorizontalAlignType::kLeft, Graphics::VerticalAlignType::kTop,
								fontColor, strokeWidth, strokeColor);
							});

						return std::make_shared<Element::UIElementVariantText>(creatingManager, generatorFun, initialString,
							hasShadow, shadowColor, UIVector2D(shadowOffsetX, shadowOffsetY));
					}
				}


				//////////////////////////////////////////////////////////////////////////////// CachedTextPrintPrefab

				//------------------------------------------------------------------------------
				std::shared_ptr<CachedTextPrintPrefab> CachedTextPrintPrefab::ReadFromFromXMLElement(ParsingXMLElementType *elem)
				{
					const char *fontClassAttribute = elem->Attribute("fontClass");
					if (fontClassAttribute == nullptr)
					{
						// No Font
						return nullptr;
					}

					const char *placeholderAttribute = "";

					// Insert Placeholder String only in Debug Mode
					#ifdef _LEGGIERO_DEBUG
					placeholderAttribute = elem->Attribute("placeholder");
					if (placeholderAttribute == nullptr)
					{
						placeholderAttribute = "";
					}
					#endif

					std::shared_ptr<CachedTextPrintPrefab> createdPrefab(std::make_shared<CachedTextPrintPrefab>(TypeUtility::ParseVariablePath(fontClassAttribute), placeholderAttribute));

					const char *sizeAttribute = elem->Attribute("size");
					if (sizeAttribute != nullptr)
					{
						createdPrefab->m_sizeExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(sizeAttribute, UIDescriptionValueType::kFloatingPoint));
					}

					const char *colorAttribute = elem->Attribute("color");
					if (colorAttribute != nullptr)
					{
						createdPrefab->m_colorExpression = Expression::CachingExpression<ColorARGBValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(colorAttribute, UIDescriptionValueType::kColor));
					}

					const char *strokeWidthAttribute = elem->Attribute("strokeWidth");
					if (strokeWidthAttribute != nullptr)
					{
						createdPrefab->m_strokeWidthExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(strokeWidthAttribute, UIDescriptionValueType::kFloatingPoint));
					}
					const char *strokeColorAttribute = elem->Attribute("strokeColor");
					if (strokeColorAttribute != nullptr)
					{
						createdPrefab->m_strokeColorExpression = Expression::CachingExpression<ColorARGBValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(strokeColorAttribute, UIDescriptionValueType::kColor));
					}

					const char *shadowOffsetXAttribute = elem->Attribute("shadowOffsetX");
					if (shadowOffsetXAttribute != nullptr)
					{
						createdPrefab->m_shadowOffsetX = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(shadowOffsetXAttribute, UIDescriptionValueType::kFloatingPoint));
					}
					const char *shadowOffsetYAttribute = elem->Attribute("shadowOffsetY");
					if (shadowOffsetYAttribute != nullptr)
					{
						createdPrefab->m_shadowOffsetY = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(shadowOffsetYAttribute, UIDescriptionValueType::kFloatingPoint));
					}
					const char *shadowColorAttribute = elem->Attribute("shadowColor");
					if (shadowColorAttribute != nullptr)
					{
						createdPrefab->m_shadowColorExpression = Expression::CachingExpression<ColorARGBValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(shadowColorAttribute, UIDescriptionValueType::kColor));
					}

					createdPrefab->_ReadObjectCommonDataFromXMLElement(elem);

					return createdPrefab;
				}

				//------------------------------------------------------------------------------
				std::shared_ptr<UIObject> CachedTextPrintPrefab::_CreateUIObject(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, std::unordered_map<VariableNameType, std::weak_ptr<UIObject> > &idTable, ComponentPostProcessCreatingContext &postProcessContext, IUIPrefabPlaceProcessor *placeProcessor)
				{
					ObjectEnvironmentPair foundClass(processingContext.Find(m_classPath));
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

					GameDataString initialString;

					// Insert Placeholder String only in Debug Mode
					#ifdef _LEGGIERO_DEBUG
					initialString = m_placeholderString;
					#endif

					Font::GlyphSizeType fontSize = static_cast<Font::GlyphSizeType>(floor(m_sizeExpression ? m_sizeExpression->Evaluate(processingContext)
						: fontClassDescription->GetDefaultSize(std::get<1>(foundClass))));
					Graphics::GLByteARGB fontColor(m_colorExpression ? m_colorExpression->Evaluate(processingContext)
						: fontClassDescription->GetDefaultColor(std::get<1>(foundClass)));
					float strokeWidth = static_cast<float>(m_strokeWidthExpression ? m_strokeWidthExpression->Evaluate(processingContext)
						: fontClassDescription->GetDefaultStrokeWidth(std::get<1>(foundClass)));
					Graphics::GLByteARGB strokeColor(Graphics::GLByteARGB::kTransparent);
					if (strokeWidth > 0.0f)
					{
						strokeColor = m_strokeColorExpression ? m_strokeColorExpression->Evaluate(processingContext)
							: fontClassDescription->GetDefaultStrokeColor(std::get<1>(foundClass));
					}
					else
					{
						strokeWidth = 0.0f;
					}

					UICoordinateType shadowOffsetX = static_cast<UICoordinateType>(m_shadowOffsetX ? m_shadowOffsetX->Evaluate(processingContext) : static_cast<float>(0.0f));
					UICoordinateType shadowOffsetY = static_cast<UICoordinateType>(m_shadowOffsetY ? m_shadowOffsetY->Evaluate(processingContext) : static_cast<float>(0.0f));
					bool hasShadow = ((shadowOffsetX * shadowOffsetX + shadowOffsetY * shadowOffsetY) > kFloatEpsilon);
					Graphics::GLByteARGB shadowColor(Graphics::GLByteARGB::kTransparent);
					if (hasShadow)
					{
						shadowColor = m_shadowColorExpression ? m_shadowColorExpression->Evaluate(processingContext) : Graphics::GLByteARGB::kBlack;
					}

					Font::GlyphManagerComponent *fontManagerComponent = &processingContext.manager.GetGlyphManager();
					Element::UIElementCachedVariantText::GlyphGeneratorType generatorFun(Element::UIElementCachedVariantText::GlyphGeneratorFuncGen(fontManagerComponent, fontSetting, fontSize, fontColor, strokeWidth, strokeColor));
					return std::make_shared<Element::UIElementCachedVariantText>(creatingManager, generatorFun, fontSize, initialString,
						hasShadow, shadowColor, UIVector2D(shadowOffsetX, shadowOffsetY));
				}
			}
		}
	}
}
