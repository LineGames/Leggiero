////////////////////////////////////////////////////////////////////////////////
// Prefab/UITransformComponentPrefab.cpp (Leggiero/Modules - LegacyUI)
//
// Transform Component Prefab Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UITransformComponentPrefab.h"

// External Library
#include <tinyxml2/tinyxml2.h>

// Leggiero.LegacyUI
#include "../Component/UITransformComponent.h"
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
				//////////////////////////////////////////////////////////////////////////////// TransformComponentPrefab

				//------------------------------------------------------------------------------
				std::shared_ptr<IUIComponent> TransformComponentPrefab::Fabricate(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, std::shared_ptr<UIObject> ownerObject, IUIPrefabPlaceProcessor *placeProcessor, ComponentPostProcessCreatingContext &postProcessingContext)
				{
					std::shared_ptr<UIParametericTransformComponent> createdComponent(std::make_shared<UIParametericTransformComponent>());

					if (m_translateXExpression) { createdComponent->SetTranslateX(m_translateXExpression->Evaluate(processingContext)); }
					if (m_translateYExpression) { createdComponent->SetTranslateY(m_translateYExpression->Evaluate(processingContext)); }

					if (m_anchorXExpression) { createdComponent->SetAnchorX(m_anchorXExpression->Evaluate(processingContext)); }
					if (m_anchorYExpression) { createdComponent->SetAnchorY(m_anchorYExpression->Evaluate(processingContext)); }

					if (m_rotateExpression)
					{
						if (m_isRotateDegree)
						{
							createdComponent->SetRotateInDegree(m_rotateExpression->Evaluate(processingContext));
						}
						else
						{
							createdComponent->SetRotate(m_rotateExpression->Evaluate(processingContext));
						}
					}

					if (m_scaleXExpression) { createdComponent->SetScaleX(m_scaleXExpression->Evaluate(processingContext)); }
					if (m_scaleYExpression) { createdComponent->SetScaleY(m_scaleYExpression->Evaluate(processingContext)); }

					if (m_skewXExpression) { createdComponent->SetSkewX(m_skewXExpression->Evaluate(processingContext)); }
					if (m_skewYExpression) { createdComponent->SetSkewY(m_skewYExpression->Evaluate(processingContext)); }

					if (m_perspectiveXExpression && m_perspectiveYExpression)
					{
						if (m_isPerspectiveXFocus && m_isPerspectiveYFocus)
						{
							createdComponent->SetPerspectiveFocus(m_perspectiveXExpression->Evaluate(processingContext), m_perspectiveYExpression->Evaluate(processingContext));
						}
						else
						{
							createdComponent->SetPerspective(m_perspectiveXExpression->Evaluate(processingContext), m_perspectiveYExpression->Evaluate(processingContext));
						}
					}
					else if (m_perspectiveXExpression)
					{
						if (m_isPerspectiveXFocus)
						{
							createdComponent->SetPerspectiveFocus(m_perspectiveXExpression->Evaluate(processingContext), (UICoordinateType)0.0f);
						}
						else
						{
							createdComponent->SetPerspectiveX(m_perspectiveXExpression->Evaluate(processingContext));
						}
					}
					else if (m_perspectiveYExpression)
					{
						if (m_isPerspectiveYFocus)
						{
							createdComponent->SetPerspectiveFocus((UICoordinateType)0.0f, m_perspectiveYExpression->Evaluate(processingContext));
						}
						else
						{
							createdComponent->SetPerspectiveY(m_perspectiveYExpression->Evaluate(processingContext));
						}
					}

					return createdComponent;
				}

				//------------------------------------------------------------------------------
				std::shared_ptr<TransformComponentPrefab> TransformComponentPrefab::ReadFromFromXMLElement(ParsingXMLElementType *elem)
				{
					std::shared_ptr<TransformComponentPrefab> createdPrefab(std::make_shared<TransformComponentPrefab>());

					const char *translateXAttribute = elem->Attribute("translateX");
					if (translateXAttribute != nullptr)
					{
						createdPrefab->m_translateXExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(translateXAttribute, UIDescriptionValueType::kFloatingPoint));
					}
					const char *translateYAttribute = elem->Attribute("translateY");
					if (translateYAttribute != nullptr)
					{
						createdPrefab->m_translateYExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(translateYAttribute, UIDescriptionValueType::kFloatingPoint));
					}

					const char *anchorXAttribute = elem->Attribute("anchorX");
					if (anchorXAttribute != nullptr)
					{
						createdPrefab->m_anchorXExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(anchorXAttribute, UIDescriptionValueType::kFloatingPoint));
					}
					const char *anchorYAttribute = elem->Attribute("anchorY");
					if (anchorYAttribute != nullptr)
					{
						createdPrefab->m_anchorYExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(anchorYAttribute, UIDescriptionValueType::kFloatingPoint));
					}

					if (elem->Attribute("rotate") != nullptr)
					{
						createdPrefab->m_isRotateDegree = true;
						createdPrefab->m_rotateExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(elem->Attribute("rotate"), UIDescriptionValueType::kFloatingPoint));
					}
					else if (elem->Attribute("rotateRadian") != nullptr)
					{
						createdPrefab->m_isRotateDegree = false;
						createdPrefab->m_rotateExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(elem->Attribute("rotateRadian"), UIDescriptionValueType::kFloatingPoint));
					}

					const char *scaleXAttribute = elem->Attribute("scaleX");
					if (scaleXAttribute != nullptr)
					{
						createdPrefab->m_scaleXExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(scaleXAttribute, UIDescriptionValueType::kFloatingPoint));
					}
					const char *scaleYAttribute = elem->Attribute("scaleY");
					if (scaleYAttribute != nullptr)
					{
						createdPrefab->m_scaleYExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(scaleYAttribute, UIDescriptionValueType::kFloatingPoint));
					}

					const char *skewXAttribute = elem->Attribute("skewX");
					if (skewXAttribute != nullptr)
					{
						createdPrefab->m_skewXExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(skewXAttribute, UIDescriptionValueType::kFloatingPoint));
					}
					const char *skewYAttribute = elem->Attribute("skewY");
					if (skewYAttribute != nullptr)
					{
						createdPrefab->m_skewYExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(skewYAttribute, UIDescriptionValueType::kFloatingPoint));
					}

					if (elem->Attribute("perspectiveX") != nullptr)
					{
						createdPrefab->m_isPerspectiveXFocus = false;
						createdPrefab->m_perspectiveXExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(elem->Attribute("perspectiveX"), UIDescriptionValueType::kFloatingPoint));
					}
					else if (elem->Attribute("perspectiveFocusX") != nullptr)
					{
						createdPrefab->m_isPerspectiveXFocus = true;
						createdPrefab->m_perspectiveXExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(elem->Attribute("perspectiveFocusX"), UIDescriptionValueType::kFloatingPoint));
					}
					if (elem->Attribute("perspectiveY") != nullptr)
					{
						createdPrefab->m_isPerspectiveYFocus = false;
						createdPrefab->m_perspectiveYExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(elem->Attribute("perspectiveY"), UIDescriptionValueType::kFloatingPoint));
					}
					else if (elem->Attribute("perspectiveFocusY") != nullptr)
					{
						createdPrefab->m_isPerspectiveYFocus = true;
						createdPrefab->m_perspectiveYExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(elem->Attribute("perspectiveFocusY"), UIDescriptionValueType::kFloatingPoint));
					}
					if (createdPrefab->m_perspectiveXExpression && createdPrefab->m_perspectiveYExpression)
					{
						if ((!createdPrefab->m_isPerspectiveXFocus && createdPrefab->m_isPerspectiveYFocus) || (createdPrefab->m_isPerspectiveXFocus && !createdPrefab->m_isPerspectiveYFocus))
						{
							// Cannot mix perspective focus and parameter
							return nullptr;
						}
					}

					return createdPrefab;
				}
			}
		}
	}
}
