////////////////////////////////////////////////////////////////////////////////
// Prefab/UIPatchedTextureRenderingComponentPrefab.cpp (Leggiero/Modules - LegacyUI)
//
// Texture Patch Rendering Component Prefab Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIPatchedTextureRenderingComponentPrefab.h"

// External Library
#include <tinyxml2/tinyxml2.h>

// Leggiero.LegacyUI
#include "../Rendering/PatchedTextureRenderingComponent.h"
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
				//////////////////////////////////////////////////////////////////////////////// PatchedTextureRenderingComponentPrefab

				//------------------------------------------------------------------------------
				std::shared_ptr<IUIComponent> PatchedTextureRenderingComponentPrefab::Fabricate(DescriptionProcessingContext &processingContext, std::shared_ptr<UIManager> creatingManager, std::shared_ptr<UIObject> ownerObject, IUIPrefabPlaceProcessor *placeProcessor, ComponentPostProcessCreatingContext &postProcessingContext)
				{
					ObjectEnvironmentPair foundClass(processingContext.Find(m_classPath));
					if (!std::get<0>(foundClass))
					{
						// Class Not Found
						return nullptr;
					}

					UICoordinateType width = static_cast<UICoordinateType>(m_widthExpression ? m_widthExpression->Evaluate(processingContext) : static_cast<float>(1.0f));
					UICoordinateType height = static_cast<UICoordinateType>(m_heightExpression ? m_heightExpression->Evaluate(processingContext) : static_cast<float>(1.0f));

					Graphics::GLByteARGB multiplyColor = m_colorExpression ? m_colorExpression->Evaluate(processingContext) : Graphics::GLByteARGB::kWhite;

					std::shared_ptr<IUIComponent> createdComponent;

					std::shared_ptr<ThreePatchTextureClass> threePatchClassDescription(std::dynamic_pointer_cast<ThreePatchTextureClass>(std::get<0>(foundClass)));
					if (threePatchClassDescription)
					{
						std::vector<std::tuple<UIRenderTextureSection, bool, bool> > &patchDescription = threePatchClassDescription->GetPatchDescription(std::get<1>(foundClass));

						std::shared_ptr<Rendering::Patch3TextureRenderingComponent> createdPatchComponent(std::make_shared<Rendering::Patch3TextureRenderingComponent>(
							width, height, (threePatchClassDescription->GetType() == UIDescriptionObjectType::kThreePatchVClass),
							std::get<0>(patchDescription[0]).texture, std::get<0>(patchDescription[0]).section,
							std::get<0>(patchDescription[1]).texture, std::get<0>(patchDescription[1]).section,
							std::get<0>(patchDescription[2]).texture, std::get<0>(patchDescription[2]).section,
							multiplyColor));

						for (size_t i = 0; i < 3; ++i)
						{
							createdPatchComponent->SetFlipState(i, std::get<1>(patchDescription[i]), std::get<2>(patchDescription[i]));
						}

						createdComponent = createdPatchComponent;
					}
					else
					{
						std::shared_ptr<NinePatchTextureClass> ninePatchClassDescription(std::dynamic_pointer_cast<NinePatchTextureClass>(std::get<0>(foundClass)));
						if (!ninePatchClassDescription)
						{
							// Invalid Class
							return nullptr;
						}

						std::vector<std::tuple<UIRenderTextureSection, bool, bool> > &patchDescription = ninePatchClassDescription->GetPatchDescription(std::get<1>(foundClass));

						std::shared_ptr<Rendering::Patch9TextureRenderingComponent> createdPatchComponent(std::make_shared<Rendering::Patch9TextureRenderingComponent>(
							width, height,
							std::get<0>(patchDescription[(size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLT]).texture, std::get<0>(patchDescription[(size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLT]).section,
							std::get<0>(patchDescription[(size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kCT]).texture, std::get<0>(patchDescription[(size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kCT]).section,
							std::get<0>(patchDescription[(size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRT]).texture, std::get<0>(patchDescription[(size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRT]).section,
							std::get<0>(patchDescription[(size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLM]).texture, std::get<0>(patchDescription[(size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLM]).section,
							std::get<0>(patchDescription[(size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kCM]).texture, std::get<0>(patchDescription[(size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kCM]).section,
							std::get<0>(patchDescription[(size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRM]).texture, std::get<0>(patchDescription[(size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRM]).section,
							std::get<0>(patchDescription[(size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLB]).texture, std::get<0>(patchDescription[(size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLB]).section,
							std::get<0>(patchDescription[(size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kCB]).texture, std::get<0>(patchDescription[(size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kCB]).section,
							std::get<0>(patchDescription[(size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRB]).texture, std::get<0>(patchDescription[(size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRB]).section,
							multiplyColor,
							ninePatchClassDescription->GetPatchScaleX(std::get<1>(foundClass)), ninePatchClassDescription->GetPatchScaleY(std::get<1>(foundClass))));

						for (size_t i = 0; i < 9; ++i)
						{
							createdPatchComponent->SetFlipState((Rendering::Patch9TextureRenderingComponent::SubPatchIndexType)i, std::get<1>(patchDescription[i]), std::get<2>(patchDescription[i]));
						}

						createdComponent = createdPatchComponent;
					}
					if (!createdComponent)
					{
						// Create Failed
						return nullptr;
					}

					if (_IsSizeFittingNeeded(processingContext))
					{
						postProcessingContext.needToSizeSets.push_back(std::dynamic_pointer_cast<IRectSizeSettable>(createdComponent));
					}

					return createdComponent;
				}

				//------------------------------------------------------------------------------
				std::shared_ptr<PatchedTextureRenderingComponentPrefab> PatchedTextureRenderingComponentPrefab::ReadFromFromXMLElement(ParsingXMLElementType *elem)
				{
					std::shared_ptr<PatchedTextureRenderingComponentPrefab> createdPrefab(std::make_shared<PatchedTextureRenderingComponentPrefab>());

					const char *patchClassTarget = elem->Attribute("patch");
					if (patchClassTarget == nullptr)
					{
						// No Texture
						return nullptr;
					}
					createdPrefab->m_classPath = TypeUtility::ParseVariablePath(patchClassTarget);

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
