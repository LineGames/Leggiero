////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionFont.cpp (Leggiero/Modules - LegacyUI)
//
// UI Description about Font Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIDescriptionFont.h"

// Standard Library
#include <cstring>
#include <limits>
#include <string>

// External Library
#include <fmt/format.h>
#include <tinyxml2/tinyxml2.h>
#include <utfcpp/utf8.h>

// Leggiero.Font
#include <Font/Common/MultiPageFontSetting.h>

// Leggiero.LegacyUI
#include "UIDescriptionManager.h"
#include "UIDescriptionProcessingContext.h"
#include "UIDescriptionExpression.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			//////////////////////////////////////////////////////////////////////////////// Utility

			namespace _Internal
			{
				// Constant for default font size
				constexpr Font::GlyphSizeType kDefaultFontSize = (Font::GlyphSizeType)32.0f;

				//------------------------------------------------------------------------------
				Font::GlyphCodePointType ParseCodePointValue(const char *codePointValueString)
				{
					if (codePointValueString == nullptr)
					{
						return static_cast<Font::GlyphCodePointType>(0);
					}

					size_t stringLength = strlen(codePointValueString);
					if (stringLength == 0)
					{
						return static_cast<Font::GlyphCodePointType>(0);
					}

					if (stringLength > 4)
					{
						if (tolower(codePointValueString[0]) == 'c'
							&& tolower(codePointValueString[1]) == 'h'
							&& tolower(codePointValueString[2]) == 'r'
							&& codePointValueString[3] == ':')
						{
							uint32_t codepoint = 0;
							try
							{
								const char *stringStartIter = codePointValueString + 4;
								codepoint = utf8::next(stringStartIter, codePointValueString + stringLength);
							}
							catch (utf8::not_enough_room ex)
							{
								// Invalid Character Code
								return static_cast<Font::GlyphCodePointType>(0);
							}

							return static_cast<Font::GlyphCodePointType>(codepoint);
						}
					}

					return static_cast<Font::GlyphCodePointType>(strtol(codePointValueString, nullptr, 0));
				}

				// Data to create each font entry in font class
				struct FontEntryData
				{
				public:
					bool																isCodePointRangeSpecified;
					Font::GlyphCodePointType											codePointRangeStart;
					Font::GlyphCodePointType											codePointRangeFinish;

					std::string															fontName;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	baselineOffsetExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	scaleMultiplierExpression;
					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	horizontalRatioMultiplierExpression;

					std::shared_ptr<Expression::IExpression<BooleanValueType> >			isForceBoldExpression;
					std::shared_ptr<Expression::IExpression<BooleanValueType> >			isForceItalicExpression;

					std::shared_ptr<Expression::IExpression<FloatingPointValueType> >	monospaceRatioExpression;

					std::shared_ptr<Expression::IExpression<BooleanValueType> >			isPreferredForTofu;

				public:
					FontEntryData(const char *fontName)
						: fontName(fontName)
						, isCodePointRangeSpecified(false), codePointRangeStart(0), codePointRangeFinish(std::numeric_limits<Font::GlyphCodePointType>::max())
					{ }

				public:
					static std::shared_ptr<FontEntryData> ReadFromFromXMLElement(tinyxml2::XMLElement *elem);
				};

				//------------------------------------------------------------------------------
				std::shared_ptr<FontEntryData> FontEntryData::ReadFromFromXMLElement(tinyxml2::XMLElement *elem)
				{
					const char *fontNameAttribute = elem->Attribute("font");
					if (fontNameAttribute == nullptr)
					{
						// Font cannot be null
						return nullptr;
					}

					std::shared_ptr<FontEntryData> createdData(std::make_shared<FontEntryData>(fontNameAttribute));

					const char *codeRangeStartAttribute = elem->Attribute("codeStart");
					const char *codeRangeFinishAttribute = elem->Attribute("codeFinish");
					if (codeRangeStartAttribute != nullptr || codeRangeFinishAttribute != nullptr)
					{
						createdData->isCodePointRangeSpecified = true;
						if (codeRangeStartAttribute != nullptr)
						{
							createdData->codePointRangeStart = ParseCodePointValue(codeRangeStartAttribute);
						}
						if (codeRangeFinishAttribute != nullptr)
						{
							createdData->codePointRangeFinish = ParseCodePointValue(codeRangeFinishAttribute);
						}
					}

					const char *baselineOffsetAttribute = elem->Attribute("baselineOffset");
					if (baselineOffsetAttribute != nullptr)
					{
						createdData->baselineOffsetExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(baselineOffsetAttribute, UIDescriptionValueType::kFloatingPoint));
					}

					const char *scaleAttribute = elem->Attribute("scaleMultiply");
					if (scaleAttribute != nullptr)
					{
						createdData->scaleMultiplierExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(scaleAttribute, UIDescriptionValueType::kFloatingPoint));
					}

					const char *horizontalRatioAttribute = elem->Attribute("horizontalRatioMultiply");
					if (horizontalRatioAttribute != nullptr)
					{
						createdData->horizontalRatioMultiplierExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(horizontalRatioAttribute, UIDescriptionValueType::kFloatingPoint));
					}

					const char *isForceBoldAttribute = elem->Attribute("isForceBold");
					if (isForceBoldAttribute != nullptr)
					{
						createdData->isForceBoldExpression = Expression::CachingExpression<BooleanValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(isForceBoldAttribute, UIDescriptionValueType::kBoolean));
					}

					const char *isForceItalicAttribute = elem->Attribute("isForceItalic");
					if (isForceItalicAttribute != nullptr)
					{
						createdData->isForceItalicExpression = Expression::CachingExpression<BooleanValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(isForceItalicAttribute, UIDescriptionValueType::kBoolean));
					}

					const char *monospaceRatioAttribute = elem->Attribute("monospaceWidthRatio");
					if (monospaceRatioAttribute != nullptr)
					{
						createdData->monospaceRatioExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(monospaceRatioAttribute, UIDescriptionValueType::kFloatingPoint));
					}

					const char *isPreferredForTofuAttribute = elem->Attribute("isPreferredForTofu");
					if (isPreferredForTofuAttribute != nullptr)
					{
						createdData->isPreferredForTofu = Expression::CachingExpression<BooleanValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(isPreferredForTofuAttribute, UIDescriptionValueType::kBoolean));
					}

					return createdData;
				}
			}


			//////////////////////////////////////////////////////////////////////////////// FontClass

			//------------------------------------------------------------------------------
			std::shared_ptr<FontClass> FontClass::ReadFromFromXMLElement(ParsingXMLElementType *elem)
			{
				std::shared_ptr<FontClass> createdDescription(std::make_shared<FontClass>());

				const char *defaultSizeAttribute = elem->Attribute("size");
				if (defaultSizeAttribute != nullptr)
				{
					createdDescription->m_defaultSizeExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(defaultSizeAttribute, UIDescriptionValueType::kFloatingPoint));
				}

				const char *defaultColorAttribute = elem->Attribute("color");
				if (defaultColorAttribute != nullptr)
				{
					createdDescription->m_defaultColorExpression = Expression::CachingExpression<ColorARGBValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(defaultColorAttribute, UIDescriptionValueType::kColor));
				}

				const char *defaultStrokeWidthAttribute = elem->Attribute("strokeWidth");
				if (defaultStrokeWidthAttribute != nullptr)
				{
					createdDescription->m_defaultStrokeWidthExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(defaultStrokeWidthAttribute, UIDescriptionValueType::kFloatingPoint));
				}
				const char *defaultStrokeColorAttribute = elem->Attribute("strokeColor");
				if (defaultStrokeColorAttribute != nullptr)
				{
					createdDescription->m_defaultStrokeColorExpression = Expression::CachingExpression<ColorARGBValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(defaultStrokeColorAttribute, UIDescriptionValueType::kColor));
				}

				for (tinyxml2::XMLElement *currentNode = elem->FirstChildElement("Font"); currentNode != nullptr; currentNode = currentNode->NextSiblingElement("Font"))
				{
					std::shared_ptr<_Internal::FontEntryData> loadedFontEntryData(_Internal::FontEntryData::ReadFromFromXMLElement(currentNode));
					if (!loadedFontEntryData)
					{
						continue;
					}
					createdDescription->m_classFontData.push_back(loadedFontEntryData);
				}

				return createdDescription;
			}

			//------------------------------------------------------------------------------
			Font::GlyphSizeType FontClass::GetDefaultSize(DescriptionProcessingContext &processingContext)
			{
				return static_cast<Font::GlyphSizeType>(m_defaultSizeExpression ? m_defaultSizeExpression->Evaluate(processingContext) : _Internal::kDefaultFontSize);
			}

			//------------------------------------------------------------------------------
			Graphics::GLByteARGB FontClass::GetDefaultColor(DescriptionProcessingContext &processingContext)
			{
				return (m_defaultColorExpression ? m_defaultColorExpression->Evaluate(processingContext) : Graphics::GLByteARGB::kWhite);
			}

			//------------------------------------------------------------------------------
			float FontClass::GetDefaultStrokeWidth(DescriptionProcessingContext &processingContext)
			{
				return static_cast<float>(m_defaultStrokeWidthExpression ? m_defaultStrokeWidthExpression->Evaluate(processingContext) : (float)0.0f);
			}

			//------------------------------------------------------------------------------
			Graphics::GLByteARGB FontClass::GetDefaultStrokeColor(DescriptionProcessingContext &processingContext)
			{
				return (m_defaultStrokeColorExpression ? m_defaultStrokeColorExpression->Evaluate(processingContext) : Graphics::GLByteARGB::kTransparent);
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<Font::MultiPageFontSetting> FontClass::GetFontSetting(DescriptionProcessingContext &processingContext)
			{
				if (m_cachedFontSetting)
				{
					return m_cachedFontSetting;
				}

				std::shared_ptr<Font::MultiPageFontSetting> createdFontSetting(std::make_shared<Font::MultiPageFontSetting>());

				for (std::shared_ptr<_Internal::FontEntryData> &fontData : m_classFontData)
				{
					std::shared_ptr<Font::IFontFace> face(processingContext.manager.GetFontFace(fontData->fontName));
					if (!face)
					{
						// No Font Face
						continue;
					}

					bool isForceMonospace = (bool)(fontData->monospaceRatioExpression);
					float monospaceWidthRatio = 0.0f;
					if (isForceMonospace)
					{
						monospaceWidthRatio = static_cast<float>(fontData->monospaceRatioExpression->Evaluate(processingContext));
					}

					Font::MultiPageFontSetting::FontSetting setting(face,
						static_cast<float>(fontData->baselineOffsetExpression ? fontData->baselineOffsetExpression->Evaluate(processingContext) : (float)0.0f),
						static_cast<float>(fontData->scaleMultiplierExpression ? fontData->scaleMultiplierExpression->Evaluate(processingContext) : (float)1.0f),
						static_cast<float>(fontData->horizontalRatioMultiplierExpression ? fontData->horizontalRatioMultiplierExpression->Evaluate(processingContext) : (float)1.0f),
						static_cast<bool>(fontData->isForceBoldExpression ? fontData->isForceBoldExpression->Evaluate(processingContext) : false),
						static_cast<bool>(fontData->isForceItalicExpression ? fontData->isForceItalicExpression->Evaluate(processingContext) : false),
						isForceMonospace, monospaceWidthRatio,
						static_cast<bool>(fontData->isPreferredForTofu ? fontData->isPreferredForTofu->Evaluate(processingContext) : false)
					);

					if (fontData->isCodePointRangeSpecified)
					{
						createdFontSetting->RegisterSetting(Font::MultiPageFontSetting::PageRange(fontData->codePointRangeStart, fontData->codePointRangeFinish), setting);
					}
					else
					{
						createdFontSetting->RegisterSetting(Font::MultiPageFontSetting::PageRange::kAll, setting);
					}
				}

				m_cachedFontSetting = createdFontSetting;
				return createdFontSetting;
			}
		}
	}
}
