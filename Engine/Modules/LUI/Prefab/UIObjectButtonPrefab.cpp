////////////////////////////////////////////////////////////////////////////////
// Prefab/UIOjbectButtonPrefab.cpp (Leggiero/Modules - LegacyUI)
//
// UI Button Object Prefab Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIObjectButtonPrefab.h"

// External Library
#include <tinyxml2/tinyxml2.h>

// Leggiero.LegacyUI
#include "../UIObject.h"
#include "../Common/UISizeSettable.h"
#include "../Component/IUIComponent.h"
#include "../Component/UILayoutComponent.h"
#include "../Component/UISizeSubComponent.h"
#include "../Element/UIElementSingleSimpleButton.h"
#include "../Description/UIPrefabDescription.h"
#include "../Description/UIDescriptionExpression.h"
#include "../Description/UIPrefabDescription_Reader.h"
#include "UIComponentPrefab.h"
#include "UIPrefabPlaceProcessor.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			namespace Prefab
			{
				//////////////////////////////////////////////////////////////////////////////// SimpleSingleButtonPrefab

				//------------------------------------------------------------------------------
				std::shared_ptr<SimpleSingleButtonPrefab> SimpleSingleButtonPrefab::ReadFromFromXMLElement(ParsingXMLElementType *elem)
				{
					const char *widthAttribute = elem->Attribute("width");
					const char *heightAttribute = elem->Attribute("height");
					if (widthAttribute == nullptr || heightAttribute == nullptr)
					{
						return nullptr;
					}

					std::shared_ptr<SimpleSingleButtonPrefab> createdPrefab(std::make_shared<SimpleSingleButtonPrefab>());

					createdPrefab->m_widthExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(widthAttribute, UIDescriptionValueType::kFloatingPoint));
					createdPrefab->m_heightExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(heightAttribute, UIDescriptionValueType::kFloatingPoint));

					const char *coverWidth = elem->Attribute("coverWidth");
					if (coverWidth != nullptr)
					{
						createdPrefab->m_coverWidthExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(coverWidth, UIDescriptionValueType::kFloatingPoint));
					}
					const char *coverHeight = elem->Attribute("coverHeight");
					if (coverHeight != nullptr)
					{
						createdPrefab->m_coverHeightExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(coverHeight, UIDescriptionValueType::kFloatingPoint));
					}

					createdPrefab->_ReadObjectCommonDataFromXMLElement(elem);

					return createdPrefab;
				}

				//------------------------------------------------------------------------------
				std::shared_ptr<UIObject> SimpleSingleButtonPrefab::_CreateUIObject(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, std::unordered_map<VariableNameType, std::weak_ptr<UIObject> > &idTable, ComponentPostProcessCreatingContext &postProcessContext, IUIPrefabPlaceProcessor *placeProcessor)
				{
					UICoordinateType width = m_widthExpression->Evaluate(processingContext);
					UICoordinateType height = m_heightExpression->Evaluate(processingContext);

					UICoordinateType coverWidth = static_cast<UICoordinateType>(m_coverWidthExpression ? m_coverWidthExpression->Evaluate(processingContext) : width);
					UICoordinateType coverHeight = static_cast<UICoordinateType>(m_coverHeightExpression ? m_coverHeightExpression->Evaluate(processingContext) : height);

					std::shared_ptr<Element::SingleSimpleButton> createdObject(std::make_shared<Element::SingleSimpleButton>(creatingManager, width, height, coverWidth, coverHeight));

					return createdObject;
				}
			}
		}
	}
}
