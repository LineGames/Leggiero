////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionTexture.cpp (Leggiero/Modules - LegacyUI)
//
// UI Description Texture Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIDescriptionTexture.h"

// External Library
#include <fmt/format.h>
#include <tinyxml2/tinyxml2.h>

// Leggiero.Utility
#include <Utility/String/AsciiStringUtility.h>

// Leggiero.Graphics
#include <Graphics/Texture/GLTextureResource.h>
#include <Graphics/Texture/TextureAtlasTable.h>

// Leggiero.LegacyUI
#include "UIDescriptionManager.h"
#include "UIDescriptionProcessingContext.h"
#include "UIDescriptionExpression.h"
#include "../Rendering/PatchedTextureRenderingComponent.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			//////////////////////////////////////////////////////////////////////////////// ImportedTexture

			//------------------------------------------------------------------------------
			ObjectEnvironmentPair ImportedTexture::Select(const VariableNameType &name, DescriptionProcessingContext &processingContext)
			{
				std::unordered_map<VariableNameType, std::shared_ptr<TextureSegment> >::iterator findIt = m_cachedSearchTable.find(name);
				if (findIt != m_cachedSearchTable.end())
				{
					return ObjectEnvironmentPair(findIt->second, processingContext);
				}

				std::shared_ptr<UICachedTexture> currentTexture = GetTexture(processingContext);
				if (!currentTexture)
				{
					// No Texture
					return ObjectEnvironmentPair(nullptr, processingContext);
				}

				if (!currentTexture->IsAtlasTexture())
				{
					// Not an Atlas Texture
					return ObjectEnvironmentPair(nullptr, processingContext);
				}

				std::shared_ptr<Graphics::TextureRectSection> findResult(currentTexture->GetAtlasTable().GetTextureSection(name));
				if (!findResult)
				{
					return ObjectEnvironmentPair(nullptr, processingContext);
				}
				std::shared_ptr<TextureSegment> loadedSegment(std::make_shared<TextureSegment>(UIRenderTextureSection(currentTexture->GetTexture(), *findResult)));
				if (loadedSegment)
				{
					m_cachedSearchTable[name] = loadedSegment;
				}
				return ObjectEnvironmentPair(loadedSegment, processingContext);
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UICachedTexture> ImportedTexture::GetTexture(DescriptionProcessingContext &processingContext)
			{
				if (m_cachedTexture)
				{
					return m_cachedTexture;
				}
				m_cachedTexture = processingContext.manager.GetCachedTexture(m_textureName);
				return m_cachedTexture;
			}

			//------------------------------------------------------------------------------
			UIRenderTextureSection ImportedTexture::GetTextureSegment(DescriptionProcessingContext &processingContext)
			{
				std::shared_ptr<Graphics::GLTextureResource> glTexture(GetTexture(processingContext)->GetTexture());
				return UIRenderTextureSection(m_cachedTexture->GetTexture(),
					Graphics::TextureRectSection(0, 0, glTexture->GetTextureInfo().contentWidth, glTexture->GetTextureInfo().contentHeight, glTexture));
			}


			//////////////////////////////////////////////////////////////////////////////// SubTextureSegment

			//------------------------------------------------------------------------------
			SubTextureSegment::SubTextureSegment(const VariablePathType &texturePath)
				: m_texturePath(texturePath)
			{

			}

			//------------------------------------------------------------------------------
			UIRenderTextureSection SubTextureSegment::GetTextureSegment(DescriptionProcessingContext &processingContext)
			{
				if (m_cachedTexture)
				{
					return *m_cachedTexture;
				}

				ObjectEnvironmentPair foundTexture(processingContext.Find(m_texturePath));
				if (!std::get<0>(foundTexture))
				{
					// Texture Not Found
					return UIRenderTextureSection(nullptr, Graphics::TextureRectSection(0, 0, 0, 0, 0, 0));
				}

				std::shared_ptr<ITextureSegmentDescription> foundTextureSegmentDescription(std::dynamic_pointer_cast<ITextureSegmentDescription>(std::get<0>(foundTexture)));
				if (!foundTextureSegmentDescription)
				{
					// Invalid Texture
					return UIRenderTextureSection(nullptr, Graphics::TextureRectSection(0, 0, 0, 0, 0, 0));
				}

				UIRenderTextureSection foundTextureSegment = foundTextureSegmentDescription->GetTextureSegment(std::get<1>(foundTexture));
				Graphics::TextureRectSection foundTextureSection(foundTextureSegment.section);

				int x = static_cast<int>(m_startXExpression ? m_startXExpression->Evaluate(processingContext) : 0);
				int y = static_cast<int>(m_startYExpression ? m_startYExpression->Evaluate(processingContext) : 0);
				int width = static_cast<int>(m_widthExpression ? m_widthExpression->Evaluate(processingContext) : foundTextureSection.GetPixelWidth() - x);
				int height = static_cast<int>(m_heightExpression ? m_heightExpression->Evaluate(processingContext) : foundTextureSection.GetPixelHeight() - y);

				m_cachedTexture = std::make_shared<UIRenderTextureSection>(foundTextureSegment.texture,
					Graphics::TextureRectSection(foundTextureSection.GetPixelLeft() + x, foundTextureSection.GetPixelTop() + y, width, height, foundTextureSegment.texture));

				return *m_cachedTexture;
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<SubTextureSegment> SubTextureSegment::ReadFromFromXMLElement(ParsingXMLElementType *elem)
			{
				const char *textureTarget = elem->Attribute("texture");
				if (textureTarget == nullptr)
				{
					// No Texture
					return nullptr;
				}

				std::shared_ptr<SubTextureSegment> createdDescription(std::make_shared<SubTextureSegment>(TypeUtility::ParseVariablePath(textureTarget)));

				const char *startXAttribute = elem->Attribute("x");
				if (startXAttribute == nullptr)
				{
					startXAttribute = elem->Attribute("left");
				}
				if (startXAttribute != nullptr)
				{
					createdDescription->m_startXExpression = Expression::CachingExpression<IntegerValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(startXAttribute, UIDescriptionValueType::kInteger));
				}
				const char *startYAttribute = elem->Attribute("y");
				if (startYAttribute == nullptr)
				{
					startYAttribute = elem->Attribute("top");
				}
				if (startYAttribute != nullptr)
				{
					createdDescription->m_startYExpression = Expression::CachingExpression<IntegerValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(startYAttribute, UIDescriptionValueType::kInteger));
				}

				const char *widthAttribute = elem->Attribute("width");
				if (widthAttribute == nullptr)
				{
					widthAttribute = elem->Attribute("w");
				}
				if (widthAttribute != nullptr)
				{
					createdDescription->m_widthExpression = Expression::CachingExpression<IntegerValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(widthAttribute, UIDescriptionValueType::kInteger));
				}
				const char *heightAttribute = elem->Attribute("height");
				if (heightAttribute == nullptr)
				{
					heightAttribute = elem->Attribute("h");
				}
				if (heightAttribute != nullptr)
				{
					createdDescription->m_heightExpression = Expression::CachingExpression<IntegerValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(heightAttribute, UIDescriptionValueType::kInteger));
				}

				return createdDescription;
			}


			//////////////////////////////////////////////////////////////////////////////// SlicedTexture

			//------------------------------------------------------------------------------
			SlicedTexture::SlicedTexture(const VariablePathType &texturePath, bool isVertical)
				: m_texturePath(texturePath), m_isVertical(isVertical)
			{

			}

			//------------------------------------------------------------------------------
			ObjectEnvironmentPair SlicedTexture::Select(const VariableNameType &name, DescriptionProcessingContext &processingContext)
			{
				std::unordered_map<VariableNameType, std::shared_ptr<TextureSegment> >::iterator findIt = m_cachedSearchTable.find(name);
				if (findIt != m_cachedSearchTable.end())
				{
					return ObjectEnvironmentPair(findIt->second, processingContext);
				}

				if (!m_cachedTexture)
				{
					ObjectEnvironmentPair foundTexture(processingContext.Find(m_texturePath));
					if (!std::get<0>(foundTexture))
					{
						// Texture Not Found
						return ObjectEnvironmentPair(nullptr, processingContext);
					}

					std::shared_ptr<ITextureSegmentDescription> foundTextureSegmentDescription(std::dynamic_pointer_cast<ITextureSegmentDescription>(std::get<0>(foundTexture)));
					if (!foundTextureSegmentDescription)
					{
						// Invalid Texture
						return ObjectEnvironmentPair(nullptr, processingContext);
					}

					UIRenderTextureSection foundTextureSegment = foundTextureSegmentDescription->GetTextureSegment(std::get<1>(foundTexture));
					m_cachedTexture = std::make_shared<UIRenderTextureSection>(foundTextureSegment.texture, foundTextureSegment.section);
				}

				for (size_t i = 0; i <= m_slicePositions.size(); ++i)
				{
					VariableNameType currentSliceName = TypeUtility::ToVariableName(fmt::format("{0}", i).c_str());
					if (currentSliceName == name)
					{
						int sliceStart = 0;
						if (i == 0)
						{
							sliceStart = 0;
						}
						else
						{
							sliceStart = _GetSlicePosition(i - 1, processingContext);
							if (sliceStart < 0)
							{
								sliceStart = (m_isVertical ? m_cachedTexture->section.GetPixelHeight() : m_cachedTexture->section.GetPixelWidth()) + sliceStart;
							}
						}

						int sliceFinish = 0;
						if (i == m_slicePositions.size())
						{
							sliceFinish = m_isVertical ? m_cachedTexture->section.GetPixelHeight() : m_cachedTexture->section.GetPixelWidth();
						}
						else
						{
							sliceFinish = _GetSlicePosition(i, processingContext);
							if (sliceFinish < 0)
							{
								sliceFinish = (m_isVertical ? m_cachedTexture->section.GetPixelHeight() : m_cachedTexture->section.GetPixelWidth()) + sliceFinish;
							}
						}

						Graphics::TextureRectSection foundTextureSection(m_cachedTexture->section);
						std::shared_ptr<TextureSegment> loadedSegment(std::make_shared<TextureSegment>(UIRenderTextureSection(m_cachedTexture->texture,
							Graphics::TextureRectSection(
								foundTextureSection.GetPixelLeft() + (m_isVertical ? 0 : sliceStart),
								foundTextureSection.GetPixelTop() + (m_isVertical ? sliceStart : 0),
								m_isVertical ? foundTextureSection.GetPixelWidth() : (sliceFinish - sliceStart),
								m_isVertical ? (sliceFinish - sliceStart) : foundTextureSection.GetPixelHeight(),
								m_cachedTexture->texture))));
						if (loadedSegment)
						{
							m_cachedSearchTable[name] = loadedSegment;
						}
						return ObjectEnvironmentPair(loadedSegment, processingContext);
					}
				}

				// Out of Index
				return ObjectEnvironmentPair(nullptr, processingContext);
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<SlicedTexture> SlicedTexture::ReadFromFromXMLElement(ParsingXMLElementType *elem)
			{
				const char *textureTarget = elem->Attribute("texture");
				if (textureTarget == nullptr)
				{
					// No Texture
					return nullptr;
				}

				// Parse Cut Positions
				std::vector<const char *> cutAttributeList;
				if (elem->Attribute("at") != nullptr)
				{
					cutAttributeList.push_back(elem->Attribute("at"));
				}
				else
				{
					size_t i = 0;
					while (true)
					{
						std::string currentAttributeName = fmt::format("cut{0}", i);
						++i;

						if (elem->Attribute(currentAttributeName.c_str()) == nullptr)
						{
							if (i == 1)
							{
								// 1-Based?
								continue;
							}
							break;
						}

						cutAttributeList.push_back(elem->Attribute(currentAttributeName.c_str()));
					}
				}
				if (cutAttributeList.empty())
				{
					return nullptr;
				}

				// Parse Orientation
				bool isVertical = false;
				const char *orientationAttribute = elem->Attribute("orientation");
				if (orientationAttribute != nullptr)
				{
					if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(orientationAttribute, "v")
						|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(orientationAttribute, "vertical"))
					{
						isVertical = true;
					}
				}

				// Make Description
				std::shared_ptr<SlicedTexture> createdDescription(std::make_shared<SlicedTexture>(TypeUtility::ParseVariablePath(textureTarget), isVertical));

				for (const char *currentCutAttribute : cutAttributeList)
				{
					createdDescription->m_slicePositions.push_back(Expression::CachingExpression<IntegerValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(currentCutAttribute, UIDescriptionValueType::kInteger)));
				}

				return createdDescription;
			}

			//------------------------------------------------------------------------------
			int SlicedTexture::_GetSlicePosition(size_t index, DescriptionProcessingContext &processingContext)
			{
				std::unordered_map<size_t, int>::iterator findIt = m_cachedCalculatedSlicePositions.find(index);
				if (findIt != m_cachedCalculatedSlicePositions.end())
				{
					return findIt->second;
				}

				int calculatedValue = static_cast<int>(m_slicePositions[index]->Evaluate(processingContext));
				m_cachedCalculatedSlicePositions[index] = calculatedValue;
				return calculatedValue;
			}


			//////////////////////////////////////////////////////////////////////////////// ThreePatchTextureClass

			//------------------------------------------------------------------------------
			ThreePatchTextureClass::ThreePatchTextureClass(bool isVertical)
				: m_isVertical(isVertical), m_hasStartTexture(false), m_hasFinishTexture(false)
			{

			}

			//------------------------------------------------------------------------------
			std::shared_ptr<ThreePatchTextureClass> ThreePatchTextureClass::ReadFromFromXMLElement(ParsingXMLElementType *elem, UIDescriptionObjectType nodeDefinedClassType)
			{
				const char *startTextureAttribute = elem->Attribute("start");
				const char *middleTextureAttribute = elem->Attribute("middle");
				const char *finishTextureAttribute = elem->Attribute("finish");
				if (middleTextureAttribute == nullptr || (startTextureAttribute == nullptr && finishTextureAttribute == nullptr))
				{
					// No Texture
					return nullptr;
				}

				if (nodeDefinedClassType == UIDescriptionObjectType::kINVALID)
				{
					nodeDefinedClassType = UIDescriptionObjectType::kThreePatchHClass;
					const char *orientationAttribute = elem->Attribute("orientation");
					if (orientationAttribute != nullptr)
					{
						if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(orientationAttribute, "v")
							|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(orientationAttribute, "vertical"))
						{
							nodeDefinedClassType = UIDescriptionObjectType::kThreePatchVClass;
						}
					}
				}

				// Make Description
				std::shared_ptr<ThreePatchTextureClass> createdDescription(std::make_shared<ThreePatchTextureClass>(nodeDefinedClassType == UIDescriptionObjectType::kThreePatchVClass));

				if (startTextureAttribute != nullptr)
				{
					createdDescription->m_hasStartTexture = true;
					createdDescription->m_startTexturePath = TypeUtility::ParseVariablePath(startTextureAttribute);
				}

				createdDescription->m_middleTexturePath = TypeUtility::ParseVariablePath(middleTextureAttribute);

				if (finishTextureAttribute != nullptr)
				{
					createdDescription->m_hasFinishTexture = true;
					createdDescription->m_finishTexturePath = TypeUtility::ParseVariablePath(finishTextureAttribute);
				}

				return createdDescription;
			}

			//------------------------------------------------------------------------------
			std::vector<std::tuple<UIRenderTextureSection, bool, bool> > &ThreePatchTextureClass::GetPatchDescription(DescriptionProcessingContext &processingContext)
			{
				if (!m_cachedDescription.empty())
				{
					return m_cachedDescription;
				}

				if (m_hasStartTexture)
				{
					m_cachedDescription.push_back(std::make_tuple(_FindTextureSection(processingContext, m_startTexturePath), false, false));
				}
				else
				{
					m_cachedDescription.push_back(std::make_tuple(_FindTextureSection(processingContext, m_finishTexturePath), !m_isVertical, m_isVertical));
				}

				m_cachedDescription.push_back(std::make_tuple(_FindTextureSection(processingContext, m_middleTexturePath), false, false));

				if (m_hasFinishTexture)
				{
					if (m_hasStartTexture)
					{
						m_cachedDescription.push_back(std::make_tuple(_FindTextureSection(processingContext, m_finishTexturePath), false, false));
					}
					else
					{
						m_cachedDescription.push_back(std::make_tuple(std::get<0>(m_cachedDescription[0]), false, false));
					}
				}
				else
				{
					m_cachedDescription.push_back(std::make_tuple(std::get<0>(m_cachedDescription[0]), !m_isVertical, m_isVertical));
				}

				return m_cachedDescription;
			}

			//------------------------------------------------------------------------------
			UIRenderTextureSection ThreePatchTextureClass::_FindTextureSection(DescriptionProcessingContext &processingContext, VariablePathType &path)
			{
				ObjectEnvironmentPair foundTexture(processingContext.Find(path));
				if (!std::get<0>(foundTexture))
				{
					// Texture Not Found
					return UIRenderTextureSection(nullptr, Graphics::TextureRectSection(0, 0, 0, 0, 0, 0));
				}

				std::shared_ptr<ITextureSegmentDescription> foundTextureSegmentDescription(std::dynamic_pointer_cast<ITextureSegmentDescription>(std::get<0>(foundTexture)));
				if (!foundTextureSegmentDescription)
				{
					// Invalid Texture
					return UIRenderTextureSection(nullptr, Graphics::TextureRectSection(0, 0, 0, 0, 0, 0));
				}

				return foundTextureSegmentDescription->GetTextureSegment(std::get<1>(foundTexture));
			}


			//////////////////////////////////////////////////////////////////////////////// NinePatchTextureClass

			//------------------------------------------------------------------------------
			std::shared_ptr<NinePatchTextureClass> NinePatchTextureClass::ReadFromFromXMLElement(ParsingXMLElementType *elem)
			{
				std::shared_ptr<NinePatchTextureClass> createdDescription(std::make_shared<NinePatchTextureClass>());

				for (const tinyxml2::XMLAttribute *currentAttribute = elem->FirstAttribute(); currentAttribute != nullptr; currentAttribute = currentAttribute->Next())
				{
					const char *attributeName = currentAttribute->Name();

					if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "lt")
						|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "tl")
						|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "lefttop")
						|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "topleft"))
					{
						createdDescription->m_texturePathTable[(size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLT] = TypeUtility::ParseVariablePath(currentAttribute->Value());
					}
					else if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "ct")
						|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "tc")
						|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "centertop")
						|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "topcenter"))
					{
						createdDescription->m_texturePathTable[(size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kCT] = TypeUtility::ParseVariablePath(currentAttribute->Value());
					}
					else if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "rt")
						|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "tr")
						|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "righttop")
						|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "topright"))
					{
						createdDescription->m_texturePathTable[(size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRT] = TypeUtility::ParseVariablePath(currentAttribute->Value());
					}
					else if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "lm")
						|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "ml")
						|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "leftmiddle")
						|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "middleleft"))
					{
						createdDescription->m_texturePathTable[(size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLM] = TypeUtility::ParseVariablePath(currentAttribute->Value());
					}
					else if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "cm")
						|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "mc")
						|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "centermiddle")
						|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "middlecenter"))
					{
						createdDescription->m_texturePathTable[(size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kCM] = TypeUtility::ParseVariablePath(currentAttribute->Value());
					}
					else if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "rm")
						|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "mr")
						|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "rightmiddle")
						|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "middleright"))
					{
						createdDescription->m_texturePathTable[(size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRM] = TypeUtility::ParseVariablePath(currentAttribute->Value());
					}
					else if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "lb")
						|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "bl")
						|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "leftbottom")
						|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "bottomleft"))
					{
						createdDescription->m_texturePathTable[(size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLB] = TypeUtility::ParseVariablePath(currentAttribute->Value());
					}
					else if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "cb")
						|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "bc")
						|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "centerbottom")
						|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "bottomcenter"))
					{
						createdDescription->m_texturePathTable[(size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kCB] = TypeUtility::ParseVariablePath(currentAttribute->Value());
					}
					else if (Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "rb")
						|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "br")
						|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "rightbottom")
						|| Utility::String::ASCIIStringUtility::IsSameStringNonCaseSensitive(attributeName, "bottomright"))
					{
						createdDescription->m_texturePathTable[(size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRB] = TypeUtility::ParseVariablePath(currentAttribute->Value());
					}
				}

				// Sufficiency Check
				if (!createdDescription->_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kCM))
				{
					return nullptr;
				}
				if (!createdDescription->_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLT)
					&& !createdDescription->_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRT)
					&& !createdDescription->_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLB)
					&& !createdDescription->_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRB))
				{
					return nullptr;
				}
				if (!createdDescription->_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kCT)
					&& !createdDescription->_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kCB))
				{
					return nullptr;
				}
				if (!createdDescription->_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLM)
					&& !createdDescription->_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRM))
				{
					return nullptr;
				}

				const char *partScaleXAttribute = elem->Attribute("partScaleX");
				if (partScaleXAttribute != nullptr)
				{
					createdDescription->m_patchScaleXExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(partScaleXAttribute, UIDescriptionValueType::kFloatingPoint));
				}
				const char *partScaleYAttribute = elem->Attribute("partScaleY");
				if (partScaleYAttribute != nullptr)
				{
					createdDescription->m_patchScaleYExpression = Expression::CachingExpression<FloatingPointValueType>::FromArbitrarilyTypedExpression(Expression::Reader::ParseInlineExpression(partScaleYAttribute, UIDescriptionValueType::kFloatingPoint));
				}

				return createdDescription;
			}

			//------------------------------------------------------------------------------
			std::vector<std::tuple<UIRenderTextureSection, bool, bool> > &NinePatchTextureClass::GetPatchDescription(DescriptionProcessingContext &processingContext)
			{
				if (!m_cachedDescription.empty())
				{
					return m_cachedDescription;
				}

				// LT
				if (_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLT))
				{
					m_cachedDescription.push_back(std::make_tuple(_GetTexture(processingContext, (size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLT), false, false));
				}
				else
				{
					if (_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRT))
					{
						m_cachedDescription.push_back(std::make_tuple(_GetTexture(processingContext, (size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRT), true, false));
					}
					else if (_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLB))
					{
						m_cachedDescription.push_back(std::make_tuple(_GetTexture(processingContext, (size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLB), false, true));
					}
					else if (_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRB))
					{
						m_cachedDescription.push_back(std::make_tuple(_GetTexture(processingContext, (size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRB), true, true));
					}
					else
					{
						return m_cachedDescription;
					}
				}

				// CT
				if (_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kCT))
				{
					m_cachedDescription.push_back(std::make_tuple(_GetTexture(processingContext, (size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kCT), false, false));
				}
				else
				{
					if (_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kCB))
					{
						m_cachedDescription.push_back(std::make_tuple(_GetTexture(processingContext, (size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kCB), false, true));
					}
					else
					{
						return m_cachedDescription;
					}
				}

				// RT
				if (_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRT))
				{
					m_cachedDescription.push_back(std::make_tuple(_GetTexture(processingContext, (size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRT), false, false));
				}
				else
				{
					if (_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLT))
					{
						m_cachedDescription.push_back(std::make_tuple(_GetTexture(processingContext, (size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLT), true, false));
					}
					else if (_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRB))
					{
						m_cachedDescription.push_back(std::make_tuple(_GetTexture(processingContext, (size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRB), false, true));
					}
					else if (_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLB))
					{
						m_cachedDescription.push_back(std::make_tuple(_GetTexture(processingContext, (size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLB), true, true));
					}
					else
					{
						return m_cachedDescription;
					}
				}

				// LM
				if (_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLM))
				{
					m_cachedDescription.push_back(std::make_tuple(_GetTexture(processingContext, (size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLM), false, false));
				}
				else
				{
					if (_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRM))
					{
						m_cachedDescription.push_back(std::make_tuple(_GetTexture(processingContext, (size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRM), true, false));
					}
					else
					{
						return m_cachedDescription;
					}
				}

				// CM
				if (_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kCM))
				{
					m_cachedDescription.push_back(std::make_tuple(_GetTexture(processingContext, (size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kCM), false, false));
				}
				else
				{
					return m_cachedDescription;
				}

				// RM
				if (_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRM))
				{
					m_cachedDescription.push_back(std::make_tuple(_GetTexture(processingContext, (size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRM), false, false));
				}
				else
				{
					if (_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLM))
					{
						m_cachedDescription.push_back(std::make_tuple(_GetTexture(processingContext, (size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLM), true, false));
					}
					else
					{
						return m_cachedDescription;
					}
				}

				// LB
				if (_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLB))
				{
					m_cachedDescription.push_back(std::make_tuple(_GetTexture(processingContext, (size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLB), false, false));
				}
				else
				{
					if (_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRB))
					{
						m_cachedDescription.push_back(std::make_tuple(_GetTexture(processingContext, (size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRB), true, false));
					}
					else if (_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLT))
					{
						m_cachedDescription.push_back(std::make_tuple(_GetTexture(processingContext, (size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLT), false, true));
					}
					else if (_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRT))
					{
						m_cachedDescription.push_back(std::make_tuple(_GetTexture(processingContext, (size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRT), true, true));
					}
					else
					{
						return m_cachedDescription;
					}
				}

				// CB
				if (_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kCB))
				{
					m_cachedDescription.push_back(std::make_tuple(_GetTexture(processingContext, (size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kCB), false, false));
				}
				else
				{
					if (_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kCT))
					{
						m_cachedDescription.push_back(std::make_tuple(_GetTexture(processingContext, (size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kCT), false, true));
					}
					else
					{
						return m_cachedDescription;
					}
				}

				// RB
				if (_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRB))
				{
					m_cachedDescription.push_back(std::make_tuple(_GetTexture(processingContext, (size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRB), false, false));
				}
				else
				{
					if (_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLB))
					{
						m_cachedDescription.push_back(std::make_tuple(_GetTexture(processingContext, (size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLB), true, false));
					}
					else if (_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRT))
					{
						m_cachedDescription.push_back(std::make_tuple(_GetTexture(processingContext, (size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kRT), false, true));
					}
					else if (_HasRealTexture((size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLT))
					{
						m_cachedDescription.push_back(std::make_tuple(_GetTexture(processingContext, (size_t)Rendering::Patch9TextureRenderingComponent::SubPatchIndexType::kLT), true, true));
					}
					else
					{
						return m_cachedDescription;
					}
				}

				return m_cachedDescription;
			}

			//------------------------------------------------------------------------------
			UICoordinateRatioType NinePatchTextureClass::GetPatchScaleX(DescriptionProcessingContext &processingContext)
			{
				return static_cast<UICoordinateRatioType>(m_patchScaleXExpression ? m_patchScaleXExpression->Evaluate(processingContext) : 1.0f);
			}

			//------------------------------------------------------------------------------
			UICoordinateRatioType NinePatchTextureClass::GetPatchScaleY(DescriptionProcessingContext &processingContext)
			{
				return static_cast<UICoordinateRatioType>(m_patchScaleYExpression ? m_patchScaleYExpression->Evaluate(processingContext) : 1.0f);
			}

			//------------------------------------------------------------------------------
			bool NinePatchTextureClass::_HasRealTexture(size_t patchIndex) const
			{
				if (m_texturePathTable.find(patchIndex) == m_texturePathTable.cend())
				{
					return false;
				}
				return true;
			}

			//------------------------------------------------------------------------------
			UIRenderTextureSection NinePatchTextureClass::_GetTexture(DescriptionProcessingContext &processingContext, size_t index)
			{
				std::unordered_map<size_t, UIRenderTextureSection>::iterator cacheFindIt = m_cachedRealTexture.find(index);
				if (cacheFindIt != m_cachedRealTexture.end())
				{
					return cacheFindIt->second;
				}

				std::unordered_map<size_t, VariablePathType>::iterator pathFindIt = m_texturePathTable.find(index);
				if (pathFindIt == m_texturePathTable.end())
				{
					// No Texture
					return UIRenderTextureSection(nullptr, Graphics::TextureRectSection(0, 0, 0, 0, 0, 0));
				}

				UIRenderTextureSection loadedTexture = _FindTextureSection(processingContext, pathFindIt->second);
				m_cachedRealTexture.insert(std::pair<size_t, UIRenderTextureSection>(index, loadedTexture));

				return loadedTexture;
			}

			//------------------------------------------------------------------------------
			UIRenderTextureSection NinePatchTextureClass::_FindTextureSection(DescriptionProcessingContext &processingContext, VariablePathType &path)
			{
				ObjectEnvironmentPair foundTexture(processingContext.Find(path));
				if (!std::get<0>(foundTexture))
				{
					// Texture Not Found
					return UIRenderTextureSection(nullptr, Graphics::TextureRectSection(0, 0, 0, 0, 0, 0));
				}

				std::shared_ptr<ITextureSegmentDescription> foundTextureSegmentDescription(std::dynamic_pointer_cast<ITextureSegmentDescription>(std::get<0>(foundTexture)));
				if (!foundTextureSegmentDescription)
				{
					// Invalid Texture
					return UIRenderTextureSection(nullptr, Graphics::TextureRectSection(0, 0, 0, 0, 0, 0));
				}

				return foundTextureSegmentDescription->GetTextureSegment(std::get<1>(foundTexture));
			}
		}
	}
}
