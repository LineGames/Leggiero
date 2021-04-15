////////////////////////////////////////////////////////////////////////////////
// Prefab/UIShapeRenderingComponentPrefab.cpp (Leggiero/Modules - LegacyUI)
//
// Basic Shape Rendering Component Prefab Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIShapeRenderingComponentPrefab.h"

// External Library
#include <tinyxml2/tinyxml2.h>

// Leggiero.LegacyUI
#include "../Rendering/ShapeSetRenderingComponent.h"
#include "../Rendering/UIShapeRect.h"
#include "../Description/UIPrefabDescription.h"
#include "../Description/UIDescriptionExpression.h"
#include "../Common/UISizeSettable.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			namespace Prefab
			{
				//////////////////////////////////////////////////////////////////////////////// IRenderingShapePrefab

				// Interface for Shape Prefab
				class IRenderingShapePrefab
				{
				public:
					virtual std::shared_ptr<IUIRenderingShape> Fabricate(DescriptionProcessingContext &processingContext, ComponentPostProcessCreatingContext &postProcessingContext) = 0;

				protected:
					virtual bool _IsSizeFittingNeeded(DescriptionProcessingContext &processingContext)
					{
						if (!m_isFitSizeExpression)
						{
							return false;
						}
						return m_isFitSizeExpression->Evaluate(processingContext);
					}

				protected:
					std::shared_ptr<Expression::IExpression<BooleanValueType> > m_isFitSizeExpression;
				};

				namespace RenderingShape
				{
					namespace _Internal
					{
						//------------------------------------------------------------------------------
						bool IsSameStringNonCaseSensitive(const char *string1, const char *string2)
						{
							if (string1 == string2)
							{
								return true;
							}
							if (string1 == nullptr || string2 == nullptr)
							{
								return false;
							}

							size_t length1 = strlen(string1);
							size_t length2 = strlen(string2);
							if (length1 != length2)
							{
								return false;
							}

							for (size_t i = 0; i < length1; ++i)
							{
								if (tolower(string1[i]) != tolower(string2[i]))
								{
									return false;
								}
							}

							return true;
						}

						//------------------------------------------------------------------------------
						ShapeBorderAlignType ParseShapeBorderAlign(const std::string &valueString)
						{
							if (IsSameStringNonCaseSensitive(valueString.c_str(), "inside"))
							{
								return ShapeBorderAlignType::kInside;
							}
							if (IsSameStringNonCaseSensitive(valueString.c_str(), "outside"))
							{
								return ShapeBorderAlignType::kOutside;
							}
							return ShapeBorderAlignType::kCenter;
						}
					}

					//////////////////////////////////////////////////////////////////////////////// RectPrefab

					// Prefab of UIShapeRect
					class RectPrefab
						: public IRenderingShapePrefab
					{
					public:
						RectPrefab() : m_borderAlign(ShapeBorderAlignType::kCenter) { }
						virtual ~RectPrefab() { }

						// IRenderingShapePrefab
					public:
						virtual std::shared_ptr<IUIRenderingShape> Fabricate(DescriptionProcessingContext &processingContext, ComponentPostProcessCreatingContext &postProcessingContext) override;

					public:
						static std::shared_ptr<RectPrefab> ReadFromFromXMLElement(tinyxml2::XMLElement *elem);

					protected:
						std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_widthExpression;
						std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_heightExpression;

						std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_offsetXExpression;
						std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_offsetYExpression;

						std::shared_ptr<Expression::IExpression<ColorARGBValueType> > m_colorExpression;

						std::shared_ptr<Expression::IExpression<FloatingPointValueType> > m_borderWidthExpression;
						std::shared_ptr<Expression::IExpression<ColorARGBValueType> > m_borderColorExpression;

						ShapeBorderAlignType m_borderAlign;
					};

					//------------------------------------------------------------------------------
					std::shared_ptr<IUIRenderingShape> RectPrefab::Fabricate(DescriptionProcessingContext &processingContext, ComponentPostProcessCreatingContext &postProcessingContext)
					{
						UICoordinateType width = static_cast<UICoordinateType>(m_widthExpression ? m_widthExpression->Evaluate(processingContext) : 0.0f);
						UICoordinateType height = static_cast<UICoordinateType>(m_heightExpression ? m_heightExpression->Evaluate(processingContext) : 0.0f);

						UICoordinateType offsetX = static_cast<UICoordinateType>(m_offsetXExpression ? m_offsetXExpression->Evaluate(processingContext) : 0.0f);
						UICoordinateType offsetY = static_cast<UICoordinateType>(m_offsetYExpression ? m_offsetYExpression->Evaluate(processingContext) : 0.0f);

						Graphics::GLByteARGB rectColor = m_colorExpression ? m_colorExpression->Evaluate(processingContext) : Graphics::GLByteARGB::kTransparent;

						UICoordinateType borderWidth = static_cast<UICoordinateType>(m_borderWidthExpression ? m_borderWidthExpression->Evaluate(processingContext) : 0.0f);
						Graphics::GLByteARGB borderColor = m_borderColorExpression ? m_borderColorExpression->Evaluate(processingContext) : Graphics::GLByteARGB::kTransparent;

						std::shared_ptr<UIShapeRect> createdShape(std::make_shared<UIShapeRect>(width, height, rectColor, borderWidth, borderColor, m_borderAlign, offsetX, offsetY));

						if (_IsSizeFittingNeeded(processingContext))
						{
							postProcessingContext.needToSizeSets.push_back(std::dynamic_pointer_cast<IRectSizeSettable>(createdShape));
						}

						return createdShape;
					}

					//------------------------------------------------------------------------------
					std::shared_ptr<RectPrefab> RectPrefab::ReadFromFromXMLElement(tinyxml2::XMLElement *elem)
					{
						std::shared_ptr<RectPrefab> createdPrefab(std::make_shared<RectPrefab>());

						const char *widthAttribute = elem->Attribute("width");
						if (widthAttribute != nullptr)
						{
							createdPrefab->m_widthExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(widthAttribute, UIDescriptionValueType::kFloatingPoint));
						}
						const char *heightAttribute = elem->Attribute("height");
						if (heightAttribute != nullptr)
						{
							createdPrefab->m_heightExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(heightAttribute, UIDescriptionValueType::kFloatingPoint));
						}

						const char *offsetXAttribute = elem->Attribute("offsetX");
						if (offsetXAttribute != nullptr)
						{
							createdPrefab->m_offsetXExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(offsetXAttribute, UIDescriptionValueType::kFloatingPoint));
						}
						const char *offsetYAttribute = elem->Attribute("offsetY");
						if (offsetYAttribute != nullptr)
						{
							createdPrefab->m_offsetYExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(offsetYAttribute, UIDescriptionValueType::kFloatingPoint));
						}

						const char *colorAttribute = elem->Attribute("color");
						if (colorAttribute != nullptr)
						{
							createdPrefab->m_colorExpression = Expression::CachingExpression<ColorARGBValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(colorAttribute, UIDescriptionValueType::kColor));
						}

						if (elem->Attribute("borderWidth") != nullptr && elem->Attribute("borderColor") != nullptr)
						{
							createdPrefab->m_borderWidthExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(elem->Attribute("borderWidth"), UIDescriptionValueType::kFloatingPoint));
							createdPrefab->m_borderColorExpression = Expression::CachingExpression<ColorARGBValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(elem->Attribute("borderColor"), UIDescriptionValueType::kColor));

							const char *borderAlignAttribute = elem->Attribute("borderAlign");
							if (borderAlignAttribute != nullptr)
							{
								createdPrefab->m_borderAlign = _Internal::ParseShapeBorderAlign(borderAlignAttribute);
							}
						}

						const char *isFitSizeAttribute = elem->Attribute("isFitSize");
						if (isFitSizeAttribute != nullptr)
						{
							createdPrefab->m_isFitSizeExpression = Expression::CachingExpression<BooleanValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(isFitSizeAttribute, UIDescriptionValueType::kBoolean));
						}

						return createdPrefab;
					}


					//////////////////////////////////////////////////////////////////////////////// Internal Reader Implementation
					namespace _Internal
					{
						namespace XMLTagNames
						{
							const char *kRect = "Rect";
						}

						//------------------------------------------------------------------------------
						std::shared_ptr<IRenderingShapePrefab> ReadShapePrefabFromElement(tinyxml2::XMLElement *elem)
						{
							const char *elementName = elem->Value();

							if (strcmp(elementName, XMLTagNames::kRect) == 0)
							{
								return RectPrefab::ReadFromFromXMLElement(elem);
							}

							return nullptr;
						}
					}
				}


				//////////////////////////////////////////////////////////////////////////////// ShapeSetRenderingComponentPrefab

				//------------------------------------------------------------------------------
				std::shared_ptr<IUIComponent> ShapeSetRenderingComponentPrefab::Fabricate(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, std::shared_ptr<UIObject> ownerObject, IUIPrefabPlaceProcessor *placeProcessor, ComponentPostProcessCreatingContext &postProcessingContext)
				{
					std::shared_ptr<Rendering::ShapeSetRenderingComponent> createdComponent(std::make_shared<Rendering::ShapeSetRenderingComponent>());

					for (std::shared_ptr<IRenderingShapePrefab> &currentShapePrefab : m_shapePrefabs)
					{
						std::shared_ptr<IUIRenderingShape> createdShape(currentShapePrefab->Fabricate(processingContext, postProcessingContext));
						if (!createdShape)
						{
							continue;
						}
						createdComponent->AddShape(createdShape);
					}

					return createdComponent;
				}

				//------------------------------------------------------------------------------
				std::shared_ptr<ShapeSetRenderingComponentPrefab> ShapeSetRenderingComponentPrefab::ReadFromFromXMLElement(ParsingXMLElementType *elem)
				{
					std::shared_ptr<ShapeSetRenderingComponentPrefab> createdPrefab(std::make_shared<ShapeSetRenderingComponentPrefab>());

					for (tinyxml2::XMLElement *currentNode = elem->FirstChildElement(); currentNode != nullptr; currentNode = currentNode->NextSiblingElement())
					{
						std::shared_ptr<IRenderingShapePrefab> loadedShapePrefab(RenderingShape::_Internal::ReadShapePrefabFromElement(currentNode));
						if (!loadedShapePrefab)
						{
							continue;
						}
						createdPrefab->m_shapePrefabs.push_back(loadedShapePrefab);
					}

					return createdPrefab;
				}
			}
		}
	}
}
