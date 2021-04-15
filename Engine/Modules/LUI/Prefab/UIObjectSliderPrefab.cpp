////////////////////////////////////////////////////////////////////////////////
// Prefab/UIObjectSliderPrefab.h (Leggiero/Modules - LegacyUI)
//
// UI Slider Object Prefab Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIObjectSliderPrefab.h"

// External Library
#include <tinyxml2/tinyxml2.h>

// Leggiero.LegacyUI
#include "../UIObject.h"
#include "../Common/UISizeSettable.h"
#include "../Component/IUIComponent.h"
#include "../Component/UILayoutComponent.h"
#include "../Component/UISizeSubComponent.h"
#include "../Element/UIElementSimpleSlideBar.h"
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
				//////////////////////////////////////////////////////////////////////////////// SimpleSliderPrefab

				//------------------------------------------------------------------------------
				std::shared_ptr<SimpleSliderPrefab> SimpleSliderPrefab::ReadFromFromXMLElement(ParsingXMLElementType *elem)
				{
					const char *widthAttribute = elem->Attribute("width");
					if (widthAttribute == nullptr)
					{
						return nullptr;
					}

					const char *handlePrefabPathAttribute = elem->Attribute("handlePrefab");
					if (handlePrefabPathAttribute == nullptr)
					{
						return nullptr;
					}

					std::shared_ptr<SimpleSliderPrefab> createdPrefab(std::make_shared<SimpleSliderPrefab>());

					createdPrefab->m_handlePrefabPath = TypeUtility::ParseVariablePath(handlePrefabPathAttribute);
					createdPrefab->m_widthExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(widthAttribute, UIDescriptionValueType::kFloatingPoint));

					const char *handleWidthRatio = elem->Attribute("handleWidthRatio");
					if (handleWidthRatio != nullptr)
					{
						createdPrefab->m_handlerTouchWidthRatioExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(handleWidthRatio, UIDescriptionValueType::kFloatingPoint));
					}
					const char *handleHeightRatio = elem->Attribute("handleHeightRatio");
					if (handleHeightRatio != nullptr)
					{
						createdPrefab->m_handlerTouchHeightRatioExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(handleHeightRatio, UIDescriptionValueType::kFloatingPoint));
					}

					const char *subRulerDivideAttribute = elem->Attribute("subRulerDivide");
					if (subRulerDivideAttribute != nullptr)
					{
						createdPrefab->m_subRulerDivideExpression = Expression::CachingExpression<IntegerValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(subRulerDivideAttribute, UIDescriptionValueType::kInteger));
					}

					createdPrefab->_ReadObjectCommonDataFromXMLElement(elem);

					return createdPrefab;
				}

				//------------------------------------------------------------------------------
				std::shared_ptr<UIObject> SimpleSliderPrefab::_CreateUIObject(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, std::unordered_map<VariableNameType, std::weak_ptr<UIObject> > &idTable, ComponentPostProcessCreatingContext &postProcessContext, IUIPrefabPlaceProcessor *placeProcessor)
				{
					// Find Marker Prefab
					ObjectEnvironmentPair handlePrefabFindResult = processingContext.Find(m_handlePrefabPath);
					if (!std::get<0>(handlePrefabFindResult))
					{
						return nullptr;
					}
					std::shared_ptr<UIPrefabDescription> prefabDescription(std::dynamic_pointer_cast<UIPrefabDescription>(std::get<0>(handlePrefabFindResult)));
					if (!prefabDescription)
					{
						return nullptr;
					}

					// Create Marker
					std::shared_ptr<UIObject> createdHandleObject(prefabDescription->FabricateInParentContext(std::get<1>(handlePrefabFindResult), creatingManager, postProcessContext, placeProcessor));
					if (!createdHandleObject)
					{
						return nullptr;
					}

					UICoordinateType width = (UICoordinateType)m_widthExpression->Evaluate(processingContext);

					UICoordinateRatioType handleWidthRatio = static_cast<UICoordinateRatioType>(m_handlerTouchWidthRatioExpression ? m_handlerTouchWidthRatioExpression->Evaluate(processingContext) : 1.0f);
					UICoordinateRatioType handleHeightRatio = static_cast<UICoordinateRatioType>(m_handlerTouchHeightRatioExpression ? m_handlerTouchHeightRatioExpression->Evaluate(processingContext) : 1.0f);

					int subRulerDivide = static_cast<int>(m_subRulerDivideExpression ? m_subRulerDivideExpression->Evaluate(processingContext) : 0);

					std::shared_ptr<Element::UIElementSimpleSlideBar> createdObject(std::make_shared<Element::UIElementSimpleSlideBar>(creatingManager, width, createdHandleObject, handleWidthRatio, handleHeightRatio,
						subRulerDivide));

					return createdObject;
				}
			}
		}
	}
}
