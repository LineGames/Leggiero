////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionSourcedString.cpp (Leggiero/Modules - LegacyUI)
//
// Sourced generated string implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIDescriptionSourcedString.h"

// External Library
#include <fmt/format.h>
#include <tinyxml2/tinyxml2.h>

// Leggiero.LegacyUI
#include "../Prefab/UIPrefabPlaceProcessor.h"
#include "UIDescriptionExpression.h"
#include "UIDescriptionProcessingContext.h"
#include "UIDescriptionManager.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			//////////////////////////////////////////////////////////////////////////////// SourcedStringEntry

			//------------------------------------------------------------------------------
			bool SourcedStringEntry::ReadFromFromXMLElement(ParsingXMLElementType *elem, const std::string &prefix)
			{
				if (elem == nullptr)
				{
					// No Element
					stringSourceType = StringSourceType::kRawString;
					rawString = g_EmptyGameDataString;
					return false;
				}

				const bool isNoPrefix = prefix.empty();

				// String Source
				const char *stringKeyAttribute = isNoPrefix ? elem->Attribute("string") : elem->Attribute(fmt::format("{0}string", prefix).c_str());
				if (stringKeyAttribute == nullptr)
				{
					const char *generateKeyAttribute = isNoPrefix ? elem->Attribute("generateKey") : elem->Attribute(fmt::format("{0}generateKey", prefix).c_str());
					if (generateKeyAttribute == nullptr)
					{
						const char *stringVariablePathAttribute = isNoPrefix ? elem->Attribute("variable") : elem->Attribute(fmt::format("{0}variable", prefix).c_str());
						if (stringVariablePathAttribute == nullptr)
						{
							const char *inlineStringExpressionAttribute = isNoPrefix ? elem->Attribute("inline") : elem->Attribute(fmt::format("{0}inline", prefix).c_str());
							if (inlineStringExpressionAttribute == nullptr)
							{
								const char *rawStringAttribute = isNoPrefix ? elem->Attribute("raw") : elem->Attribute(fmt::format("{0}raw", prefix).c_str());
								if (rawStringAttribute == nullptr)
								{
									// No String Data
									stringSourceType = StringSourceType::kRawString;
									rawString = g_EmptyGameDataString;
									return false;
								}
								else
								{
									stringSourceType = StringSourceType::kRawString;
									rawString = rawStringAttribute;
								}
							}
							else
							{
								stringSourceType = StringSourceType::kInline;
								inlineStringExpression = Expression::CachingExpression<StringValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(inlineStringExpressionAttribute, UIDescriptionValueType::kString));
							}
						}
						else
						{
							stringSourceType = StringSourceType::kVariable;
							stringVariablePath = TypeUtility::ParseVariablePath(stringVariablePathAttribute);
						}
					}
					else
					{
						stringSourceType = StringSourceType::kRuntimeGenerating;
						generateKey = generateKeyAttribute;
					}
				}
				else
				{
					stringSourceType = StringSourceType::kTextInBag;
					stringKey = stringKeyAttribute;
				}

				return true;
			}

			//------------------------------------------------------------------------------
			GameDataString SourcedStringEntry::GetStringValue(DescriptionProcessingContext &processingContext, Prefab::IUIPrefabPlaceProcessor *placeProcessor)
			{
				switch (stringSourceType)
				{
					case StringSourceType::kTextInBag:
						{
							return processingContext.manager.GetDataString(stringKey);
						}
						break;

					case StringSourceType::kRuntimeGenerating:
						{
							if (placeProcessor != nullptr)
							{
								return placeProcessor->ProcessStringGeneration(generateKey);
							}
						}
						break;

					case StringSourceType::kVariable:
						{
							ObjectEnvironmentPair foundVariable(processingContext.Find(stringVariablePath));
							if (std::get<0>(foundVariable))
							{
								std::shared_ptr<IValueGetter> valueGettingTarget(std::dynamic_pointer_cast<IValueGetter>(std::get<0>(foundVariable)));
								if (valueGettingTarget)
								{
									return GameDataString(valueGettingTarget->GetValueString(std::get<1>(foundVariable)));
								}
							}
						}
						break;

					case StringSourceType::kInline:
						{
							if (inlineStringExpression)
							{
								return GameDataString(inlineStringExpression->Evaluate(processingContext));
							}
						}
						break;

					case StringSourceType::kRawString:
						{
							return rawString;
						}
						break;
				}

				return g_EmptyGameDataString;
			}
		}
	}
}
