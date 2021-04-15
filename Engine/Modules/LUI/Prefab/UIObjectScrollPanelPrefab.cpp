////////////////////////////////////////////////////////////////////////////////
// Prefab/UIObjectScrollPanelPrefab.h (Leggiero/Modules - LegacyUI)
//
// Scrollable Panel Object Prefab Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIObjectScrollPanelPrefab.h"

// External Library
#include <tinyxml2/tinyxml2.h>

// Leggiero.LegacyUI
#include "../UIObject.h"
#include "../Common/UISizeSettable.h"
#include "../Component/IUIComponent.h"
#include "../Component/UILayoutComponent.h"
#include "../Component/UISizeSubComponent.h"
#include "../Element/UIElementSimpleVerticalScrollPanel.h"
#include "../Element/UIElementSimpleHorizontalScrollPanel.h"
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
				//////////////////////////////////////////////////////////////////////////////// SimpleVerticalScrollPanelPrefab

				//------------------------------------------------------------------------------
				std::shared_ptr<SimpleVerticalScrollPanelPrefab> SimpleVerticalScrollPanelPrefab::ReadFromFromXMLElement(ParsingXMLElementType *elem)
				{
					const char *widthAttribute = elem->Attribute("width");
					if (widthAttribute == nullptr)
					{
						return nullptr;
					}

					const char *heightAttribute = elem->Attribute("height");
					if (heightAttribute == nullptr)
					{
						return nullptr;
					}

					tinyxml2::XMLElement *contentsChild = elem->FirstChildElement("Contents");
					if (contentsChild == nullptr)
					{
						return nullptr;
					}
					tinyxml2::XMLElement *contentsObjectElem = contentsChild->FirstChildElement();
					if (contentsObjectElem == nullptr)
					{
						return nullptr;
					}
					std::shared_ptr<IUIObjectPrefab> contentsObjectPrefab(Reader::XML::ReadObjectPrefabFromElement(contentsObjectElem));
					if (!contentsObjectPrefab)
					{
						return nullptr;
					}

					std::shared_ptr<SimpleVerticalScrollPanelPrefab> createdPrefab(std::make_shared<SimpleVerticalScrollPanelPrefab>());

					createdPrefab->m_widthExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(widthAttribute, UIDescriptionValueType::kFloatingPoint));
					createdPrefab->m_heightExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(heightAttribute, UIDescriptionValueType::kFloatingPoint));
					createdPrefab->contentsObjectPrefab = contentsObjectPrefab;

					const char *contentsHeightAttribute = elem->Attribute("contentsHeight");
					if (contentsHeightAttribute != nullptr)
					{
						createdPrefab->m_contentsHeightExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(contentsHeightAttribute, UIDescriptionValueType::kFloatingPoint));
					}

					const char *moveThresholdAttribute = elem->Attribute("moveThreshold");
					if (moveThresholdAttribute != nullptr)
					{
						createdPrefab->m_moveThresholdAmountExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(moveThresholdAttribute, UIDescriptionValueType::kFloatingPoint));
					}

					const char *isClippingAttribute = elem->Attribute("isClipping");
					if (isClippingAttribute != nullptr)
					{
						createdPrefab->m_isClippingExpression = Expression::CachingExpression<BooleanValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(isClippingAttribute, UIDescriptionValueType::kBoolean));
					}

					createdPrefab->_ReadObjectCommonDataFromXMLElement(elem);

					return createdPrefab;
				}

				//------------------------------------------------------------------------------
				bool SimpleVerticalScrollPanelPrefab::_IsObjectSpecialXMLElement(ParsingXMLElementType *elem)
				{
					if (elem == nullptr)
					{
						return false;
					}
					const char *childName = elem->Value();

					if (strcmp(childName, "Contents") == 0)
					{
						return true;
					}
					return false;
				}

				//------------------------------------------------------------------------------
				std::shared_ptr<UIObject> SimpleVerticalScrollPanelPrefab::_CreateUIObject(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, std::unordered_map<VariableNameType, std::weak_ptr<UIObject> > &idTable, ComponentPostProcessCreatingContext &postProcessContext, IUIPrefabPlaceProcessor *placeProcessor)
				{
					// Create Contents Object
					std::shared_ptr<UIObject> fabricatedContentsObject(contentsObjectPrefab->FabricateObject(processingContext, creatingManager, idTable, postProcessContext, placeProcessor));
					if (!fabricatedContentsObject)
					{
						return nullptr;
					}

					UICoordinateType width = (UICoordinateType)m_widthExpression->Evaluate(processingContext);
					UICoordinateType height = (UICoordinateType)m_heightExpression->Evaluate(processingContext);

					UICoordinateType contentsHeight = static_cast<UICoordinateType>(0.0f);
					if (m_contentsHeightExpression)
					{
						contentsHeight = static_cast<UICoordinateRatioType>(m_contentsHeightExpression->Evaluate(processingContext));
					}
					else
					{
						if (fabricatedContentsObject->GetLayoutComponent() && fabricatedContentsObject->GetLayoutComponent()->GetSizeSubComponent())
						{
							contentsHeight = fabricatedContentsObject->GetLayoutComponent()->GetSizeSubComponent()->CalculateSize().height;
						}
						else
						{
							contentsHeight = fabricatedContentsObject->CalculateHeight();
						}
					}

					UICoordinateRatioType moveThreshold = static_cast<UICoordinateType>(m_moveThresholdAmountExpression ? m_moveThresholdAmountExpression->Evaluate(processingContext) : 0.0f);
					bool isClipping = static_cast<bool>(m_isClippingExpression ? m_isClippingExpression->Evaluate(processingContext) : true);

					std::shared_ptr<Element::UIElementSimpleVerticalScrollPanel> createdObject(std::make_shared<Element::UIElementSimpleVerticalScrollPanel>(creatingManager, width, height, fabricatedContentsObject, contentsHeight, moveThreshold, isClipping));
					createdObject->AddPostUIObject(fabricatedContentsObject);

					return createdObject;
				}


				//////////////////////////////////////////////////////////////////////////////// SimpleHorizontalScrollPanelPrefab

				//------------------------------------------------------------------------------
				std::shared_ptr<SimpleHorizontalScrollPanelPrefab> SimpleHorizontalScrollPanelPrefab::ReadFromFromXMLElement(ParsingXMLElementType *elem)
				{
					const char *widthAttribute = elem->Attribute("width");
					if (widthAttribute == nullptr)
					{
						return nullptr;
					}

					const char *heightAttribute = elem->Attribute("height");
					if (heightAttribute == nullptr)
					{
						return nullptr;
					}

					tinyxml2::XMLElement *contentsChild = elem->FirstChildElement("Contents");
					if (contentsChild == nullptr)
					{
						return nullptr;
					}
					tinyxml2::XMLElement *contentsObjectElem = contentsChild->FirstChildElement();
					if (contentsObjectElem == nullptr)
					{
						return nullptr;
					}
					std::shared_ptr<IUIObjectPrefab> contentsObjectPrefab(Reader::XML::ReadObjectPrefabFromElement(contentsObjectElem));
					if (!contentsObjectPrefab)
					{
						return nullptr;
					}

					std::shared_ptr<SimpleHorizontalScrollPanelPrefab> createdPrefab(std::make_shared<SimpleHorizontalScrollPanelPrefab>());

					createdPrefab->m_widthExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(widthAttribute, UIDescriptionValueType::kFloatingPoint));
					createdPrefab->m_heightExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(heightAttribute, UIDescriptionValueType::kFloatingPoint));
					createdPrefab->contentsObjectPrefab = contentsObjectPrefab;

					const char *contentsHeightAttribute = elem->Attribute("contentsWidth");
					if (contentsHeightAttribute != nullptr)
					{
						createdPrefab->m_contentsWidthExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(contentsHeightAttribute, UIDescriptionValueType::kFloatingPoint));
					}

					const char *moveThresholdAttribute = elem->Attribute("moveThreshold");
					if (moveThresholdAttribute != nullptr)
					{
						createdPrefab->m_moveThresholdAmountExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(moveThresholdAttribute, UIDescriptionValueType::kFloatingPoint));
					}

					const char *isClippingAttribute = elem->Attribute("isClipping");
					if (isClippingAttribute != nullptr)
					{
						createdPrefab->m_isClippingExpression = Expression::CachingExpression<BooleanValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(isClippingAttribute, UIDescriptionValueType::kBoolean));
					}

					createdPrefab->_ReadObjectCommonDataFromXMLElement(elem);

					return createdPrefab;
				}

				//------------------------------------------------------------------------------
				bool SimpleHorizontalScrollPanelPrefab::_IsObjectSpecialXMLElement(ParsingXMLElementType *elem)
				{
					if (elem == nullptr)
					{
						return false;
					}
					const char *childName = elem->Value();

					if (strcmp(childName, "Contents") == 0)
					{
						return true;
					}
					return false;
				}

				//------------------------------------------------------------------------------
				std::shared_ptr<UIObject> SimpleHorizontalScrollPanelPrefab::_CreateUIObject(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, std::unordered_map<VariableNameType, std::weak_ptr<UIObject> > &idTable, ComponentPostProcessCreatingContext &postProcessContext, IUIPrefabPlaceProcessor *placeProcessor)
				{
					// Create Contents Object
					std::shared_ptr<UIObject> fabricatedContentsObject(contentsObjectPrefab->FabricateObject(processingContext, creatingManager, idTable, postProcessContext, placeProcessor));
					if (!fabricatedContentsObject)
					{
						return nullptr;
					}

					UICoordinateType width = (UICoordinateType)m_widthExpression->Evaluate(processingContext);
					UICoordinateType height = (UICoordinateType)m_heightExpression->Evaluate(processingContext);

					UICoordinateType contentsWidth = static_cast<UICoordinateType>(0.0f);
					if (m_contentsWidthExpression)
					{
						contentsWidth = static_cast<UICoordinateRatioType>(m_contentsWidthExpression->Evaluate(processingContext));
					}
					else
					{
						if (fabricatedContentsObject->GetLayoutComponent() && fabricatedContentsObject->GetLayoutComponent()->GetSizeSubComponent())
						{
							contentsWidth = fabricatedContentsObject->GetLayoutComponent()->GetSizeSubComponent()->CalculateSize().width;
						}
						else
						{
							contentsWidth = fabricatedContentsObject->CalculateWidth();
						}
					}

					UICoordinateRatioType moveThreshold = static_cast<UICoordinateType>(m_moveThresholdAmountExpression ? m_moveThresholdAmountExpression->Evaluate(processingContext) : 0.0f);
					bool isClipping = static_cast<bool>(m_isClippingExpression ? m_isClippingExpression->Evaluate(processingContext) : true);

					std::shared_ptr<Element::UIElementSimpleHorizontalScrollPanel> createdObject(std::make_shared<Element::UIElementSimpleHorizontalScrollPanel>(creatingManager, height, width, fabricatedContentsObject, contentsWidth, moveThreshold, isClipping));
					createdObject->AddPostUIObject(fabricatedContentsObject);

					return createdObject;
				}
			}
		}
	}
}
