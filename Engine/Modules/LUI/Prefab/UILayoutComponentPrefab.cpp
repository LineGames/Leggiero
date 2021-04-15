////////////////////////////////////////////////////////////////////////////////
// Prefab/UILayoutComponentPrefab.cpp (Leggiero/Modules - LegacyUI)
//
// Layout Component Prefab Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UILayoutComponentPrefab.h"

// External Library
#include <tinyxml2/tinyxml2.h>

// Leggiero.LegacyUI
#include "../Component/UILayoutComponent.h"
#include "../Component/UISizeSubComponent.h"
#include "../Description/UIPrefabDescription.h"
#include "../Description/UIDescriptionExpression.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			namespace Prefab
			{
				//////////////////////////////////////////////////////////////////////////////// LayoutComponentPrefab

				//------------------------------------------------------------------------------
				LayoutComponentPrefab::LayoutComponentPrefab()
					: m_sizeReference(SizeReferenceType::kINVALID)
				{

				}

				//------------------------------------------------------------------------------
				std::shared_ptr<IUIComponent> LayoutComponentPrefab::Fabricate(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, std::shared_ptr<UIObject> ownerObject, IUIPrefabPlaceProcessor *placeProcessor, ComponentPostProcessCreatingContext &postProcessingContext)
				{
					std::shared_ptr<IUISizeSubComponent> sizeSubComponent;
					switch (m_sizeReference)
					{
						case SizeReferenceType::kObject:
							{
								sizeSubComponent = std::make_shared<UIVisualSizeSubComponent>(ownerObject);
							}
							break;

						case SizeReferenceType::kObjectElement:
							{
								sizeSubComponent = std::make_shared<UIObjectSizeSubComponent>(ownerObject);
							}
							break;

						case SizeReferenceType::kHorizontalChildren:
							{
								sizeSubComponent = std::make_shared<UIChildrenStackSizeSubComponent>(ownerObject, true);
							}
							break;

						case SizeReferenceType::kVerticalChildren:
							{
								sizeSubComponent = std::make_shared<UIChildrenStackSizeSubComponent>(ownerObject, false);
							}
							break;

						case SizeReferenceType::kParent:
							{
								sizeSubComponent = std::make_shared<UIParentSizeSubComponent>();
							}
							break;

						default:
							{
								UICoordinateType fixedWidth = static_cast<UICoordinateType>(m_fixedWidthExpression ? m_fixedWidthExpression->Evaluate(processingContext) : 0.0f);
								UICoordinateType fixedHeight = static_cast<UICoordinateType>(m_fixedHeightExpression ? m_fixedHeightExpression->Evaluate(processingContext) : 0.0f);
								sizeSubComponent = std::make_shared<UIValuedSizeSubComponent>(fixedWidth, fixedHeight);
							}
					}

					if (m_widthModifyAddExpression || m_heightModifyAddExpression || m_widthModifyMultiplyExpression || m_heightModifyMultiplyExpression)
					{
						UICoordinateType widthModifyAdd = static_cast<UICoordinateType>(m_widthModifyAddExpression ? m_widthModifyAddExpression->Evaluate(processingContext) : 0.0f);
						UICoordinateType heightModifyAdd = static_cast<UICoordinateType>(m_heightModifyAddExpression ? m_heightModifyAddExpression->Evaluate(processingContext) : 0.0f);
						UICoordinateRatioType widthModifyMultiplier = static_cast<UICoordinateRatioType>(m_widthModifyMultiplyExpression ? m_widthModifyMultiplyExpression->Evaluate(processingContext) : 1.0f);
						UICoordinateRatioType heightModifyMultiplier = static_cast<UICoordinateRatioType>(m_heightModifyMultiplyExpression ? m_heightModifyMultiplyExpression->Evaluate(processingContext) : 1.0f);
						sizeSubComponent = std::make_shared<ModifedSizeSubComponent>(sizeSubComponent, widthModifyAdd, heightModifyAdd, widthModifyMultiplier, heightModifyMultiplier);
					}

					std::shared_ptr<UILayoutComponent> createdLayoutComponent(std::make_shared<UILayoutComponent>(sizeSubComponent));

					createdLayoutComponent->parentReferenceRatioX = static_cast<UICoordinateRatioType>(m_parentReferenceRatioXExpression ? m_parentReferenceRatioXExpression->Evaluate(processingContext) : 0.0f);
					createdLayoutComponent->parentReferenceRatioY = static_cast<UICoordinateRatioType>(m_parentReferenceRatioYExpression ? m_parentReferenceRatioYExpression->Evaluate(processingContext) : 0.0f);
					createdLayoutComponent->parentOffsetX = static_cast<UICoordinateType>(m_parentOffsetXExpression ? m_parentOffsetXExpression->Evaluate(processingContext) : 0.0f);
					createdLayoutComponent->parentOffsetY = static_cast<UICoordinateType>(m_parentOffsetYExpression ? m_parentOffsetYExpression->Evaluate(processingContext) : 0.0f);

					createdLayoutComponent->anchorReferenceRatioX = static_cast<UICoordinateRatioType>(m_anchorReferenceRatioXExpression ? m_anchorReferenceRatioXExpression->Evaluate(processingContext) : 0.0f);
					createdLayoutComponent->anchorReferenceRatioY = static_cast<UICoordinateRatioType>(m_anchorReferenceRatioYExpression ? m_anchorReferenceRatioYExpression->Evaluate(processingContext) : 0.0f);
					createdLayoutComponent->anchorOffsetX = static_cast<UICoordinateType>(m_anchorOffsetXExpression ? m_anchorOffsetXExpression->Evaluate(processingContext) : 0.0f);
					createdLayoutComponent->anchorOffsetY = static_cast<UICoordinateType>(m_anchorOffsetYExpression ? m_anchorOffsetYExpression->Evaluate(processingContext) : 0.0f);

					return createdLayoutComponent;
				}

				//------------------------------------------------------------------------------
				std::shared_ptr<LayoutComponentPrefab> LayoutComponentPrefab::ReadFromFromXMLElement(ParsingXMLElementType *elem)
				{
					std::shared_ptr<LayoutComponentPrefab> createdPrefab(std::make_shared<LayoutComponentPrefab>());

					const char *sizeReferenceAttribute = elem->Attribute("sizeRef");
					if (sizeReferenceAttribute != nullptr)
					{
						if (strcmp(sizeReferenceAttribute, "object") == 0)
						{
							createdPrefab->m_sizeReference = SizeReferenceType::kObject;
						}
						else if (strcmp(sizeReferenceAttribute, "childrenH") == 0)
						{
							createdPrefab->m_sizeReference = SizeReferenceType::kHorizontalChildren;
						}
						else if (strcmp(sizeReferenceAttribute, "childrenV") == 0)
						{
							createdPrefab->m_sizeReference = SizeReferenceType::kVerticalChildren;
						}
						else if (strcmp(sizeReferenceAttribute, "parent") == 0)
						{
							createdPrefab->m_sizeReference = SizeReferenceType::kParent;
						}
						else if (strcmp(sizeReferenceAttribute, "objectElement") == 0)
						{
							createdPrefab->m_sizeReference = SizeReferenceType::kObjectElement;
						}
					}

					if (createdPrefab->m_sizeReference == SizeReferenceType::kINVALID)
					{
						const char *widthAttribute = elem->Attribute("width");
						if (widthAttribute != nullptr)
						{
							createdPrefab->m_fixedWidthExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(widthAttribute, UIDescriptionValueType::kFloatingPoint));
						}

						const char *heightAttribute = elem->Attribute("height");
						if (heightAttribute != nullptr)
						{
							createdPrefab->m_fixedHeightExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(heightAttribute, UIDescriptionValueType::kFloatingPoint));
						}
					}

					{
						const char *widthModifyAddAttribute = elem->Attribute("widthModifyAdd");
						if (widthModifyAddAttribute != nullptr)
						{
							createdPrefab->m_widthModifyAddExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(widthModifyAddAttribute, UIDescriptionValueType::kFloatingPoint));
						}
						const char *heightModifyAddAttribute = elem->Attribute("heightModifyAdd");
						if (heightModifyAddAttribute != nullptr)
						{
							createdPrefab->m_heightModifyAddExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(heightModifyAddAttribute, UIDescriptionValueType::kFloatingPoint));
						}

						const char *widthModifyMultiplyAttribute = elem->Attribute("widthModifyMultiply");
						if (widthModifyMultiplyAttribute != nullptr)
						{
							createdPrefab->m_widthModifyMultiplyExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(widthModifyMultiplyAttribute, UIDescriptionValueType::kFloatingPoint));
						}
						const char *heightModifyMultiplyAttribute = elem->Attribute("heightModifyMultiply");
						if (heightModifyMultiplyAttribute != nullptr)
						{
							createdPrefab->m_heightModifyMultiplyExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(heightModifyMultiplyAttribute, UIDescriptionValueType::kFloatingPoint));
						}
					}

					const char *parentReferenceRatioXAttribute = elem->Attribute("parentRefRatioX");
					if (parentReferenceRatioXAttribute != nullptr)
					{
						createdPrefab->m_parentReferenceRatioXExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(parentReferenceRatioXAttribute, UIDescriptionValueType::kFloatingPoint));
					}
					const char *parentReferenceRatioYAttribute = elem->Attribute("parentRefRatioY");
					if (parentReferenceRatioYAttribute != nullptr)
					{
						createdPrefab->m_parentReferenceRatioYExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(parentReferenceRatioYAttribute, UIDescriptionValueType::kFloatingPoint));
					}
					const char *parentOffsetXAttribute = elem->Attribute("parentOffsetX");
					if (parentOffsetXAttribute != nullptr)
					{
						createdPrefab->m_parentOffsetXExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(parentOffsetXAttribute, UIDescriptionValueType::kFloatingPoint));
					}
					const char *parentOffsetYAttribute = elem->Attribute("parentOffsetY");
					if (parentOffsetYAttribute != nullptr)
					{
						createdPrefab->m_parentOffsetYExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(parentOffsetYAttribute, UIDescriptionValueType::kFloatingPoint));
					}

					const char *anchorReferenceRatioXAttribute = elem->Attribute("anchorRefRatioX");
					if (anchorReferenceRatioXAttribute != nullptr)
					{
						createdPrefab->m_anchorReferenceRatioXExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(anchorReferenceRatioXAttribute, UIDescriptionValueType::kFloatingPoint));
					}
					const char *anchorReferenceRatioYAttribute = elem->Attribute("anchorRefRatioY");
					if (anchorReferenceRatioYAttribute != nullptr)
					{
						createdPrefab->m_anchorReferenceRatioYExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(anchorReferenceRatioYAttribute, UIDescriptionValueType::kFloatingPoint));
					}
					const char *anchorOffsetXAttribute = elem->Attribute("anchorOffsetX");
					if (anchorOffsetXAttribute != nullptr)
					{
						createdPrefab->m_anchorOffsetXExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(anchorOffsetXAttribute, UIDescriptionValueType::kFloatingPoint));
					}
					const char *anchorOffsetYAttribute = elem->Attribute("anchorOffsetY");
					if (anchorOffsetYAttribute != nullptr)
					{
						createdPrefab->m_anchorOffsetYExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(anchorOffsetYAttribute, UIDescriptionValueType::kFloatingPoint));
					}

					return createdPrefab;
				}
			}
		}
	}
}
