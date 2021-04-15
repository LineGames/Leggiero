////////////////////////////////////////////////////////////////////////////////
// Prefab/UITextureRenderingComponentPrefab.h (Leggiero/Modules - LegacyUI)
//
// Texture Rendering Component Prefab Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UITextureRenderingComponentPrefab.h"

// External Library
#include <tinyxml2/tinyxml2.h>

// Leggiero.LegacyUI
#include "../Rendering/TextureRenderingComponent.h"
#include "../Description/UIDescription.h"
#include "../Description/UIDescriptionTexture.h"
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
				//////////////////////////////////////////////////////////////////////////////// TextureRenderingComponentPrefab

				//------------------------------------------------------------------------------
				std::shared_ptr<IUIComponent> TextureRenderingComponentPrefab::Fabricate(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, std::shared_ptr<UIObject> ownerObject, IUIPrefabPlaceProcessor *placeProcessor, ComponentPostProcessCreatingContext &postProcessingContext)
				{
					ObjectEnvironmentPair foundTexture(processingContext.Find(m_texturePath));
					if (!std::get<0>(foundTexture))
					{
						// Texture Not Found
						return nullptr;
					}

					std::shared_ptr<ITextureSegmentDescription> foundTextureSegmentDescription(std::dynamic_pointer_cast<ITextureSegmentDescription>(std::get<0>(foundTexture)));
					if (!foundTextureSegmentDescription)
					{
						// Invalid Texture
						return nullptr;
					}

					UIRenderTextureSection foundTextureSegment = foundTextureSegmentDescription->GetTextureSegment(std::get<1>(foundTexture));

					UICoordinateType width = static_cast<UICoordinateType>(m_widthExpression ? m_widthExpression->Evaluate(processingContext)
						: ((float)foundTextureSegment.section.GetPixelWidth() * (static_cast<float>(m_scaleXExpression ? m_scaleXExpression->Evaluate(processingContext) : 1.0f))));
					UICoordinateType height = static_cast<UICoordinateType>(m_heightExpression ? m_heightExpression->Evaluate(processingContext)
						: ((float)foundTextureSegment.section.GetPixelHeight() * (static_cast<float>(m_scaleXExpression ? m_scaleXExpression->Evaluate(processingContext) : 1.0f))));

					Graphics::GLByteARGB multiplyColor = m_colorExpression ? m_colorExpression->Evaluate(processingContext) : Graphics::GLByteARGB::kWhite;

					std::shared_ptr<Rendering::TextureRenderingComponent> createdComponent(std::make_shared<Rendering::TextureRenderingComponent>(
						foundTextureSegment.texture, foundTextureSegment.section,
						width, height, multiplyColor));

					if (_IsSizeFittingNeeded(processingContext))
					{
						postProcessingContext.needToSizeSets.push_back(std::dynamic_pointer_cast<IRectSizeSettable>(createdComponent));
					}

					return createdComponent;
				}

				//------------------------------------------------------------------------------
				std::shared_ptr<TextureRenderingComponentPrefab> TextureRenderingComponentPrefab::ReadFromFromXMLElement(ParsingXMLElementType *elem)
				{
					std::shared_ptr<TextureRenderingComponentPrefab> createdPrefab(std::make_shared<TextureRenderingComponentPrefab>());

					const char *textureTarget = elem->Attribute("texture");
					if (textureTarget == nullptr)
					{
						// No Texture
						return nullptr;
					}
					createdPrefab->m_texturePath = TypeUtility::ParseVariablePath(textureTarget);

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
					if (scaleXAttribute == nullptr && scaleYAttribute == nullptr)
					{
						const char *scaleAttribute = elem->Attribute("scale");
						if (scaleAttribute != nullptr)
						{
							createdPrefab->m_scaleXExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(scaleAttribute, UIDescriptionValueType::kFloatingPoint));
							createdPrefab->m_scaleYExpression = createdPrefab->m_scaleXExpression;
						}
					}

					const char *colorAttribute = elem->Attribute("color");
					if (colorAttribute != nullptr)
					{
						createdPrefab->m_colorExpression = Expression::CachingExpression<ColorARGBValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(colorAttribute, UIDescriptionValueType::kColor));
					}

					const char *isFitSizeAttribute = elem->Attribute("isFitSize");
					if (isFitSizeAttribute != nullptr)
					{
						createdPrefab->m_isFitSizeExpression = Expression::CachingExpression<BooleanValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(isFitSizeAttribute, UIDescriptionValueType::kBoolean));
					}

					return createdPrefab;
				}
			}
		}
	}
}
