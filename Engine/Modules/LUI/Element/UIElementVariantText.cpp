////////////////////////////////////////////////////////////////////////////////
// Element/UIElementVariantText.cpp (Leggiero/Modules - LegacyUI)
//
// Changable UI Text Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIElementVariantText.h"

// Standard Library
#include <unordered_set>

// Leggiero.LegacyUI
#include "../Component/UILayoutComponent.h"
#include "../Component/UISizeSubComponent.h"
#include "../Rendering/TextRenderingComponent.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Element
		{
			//////////////////////////////////////////////////////////////////////////////// UIElementVariantText

			//------------------------------------------------------------------------------
			UIElementVariantText::UIElementVariantText(std::shared_ptr<UIManager> ownerManager, TypesettingGeneratorType typesettingGeneratorFunc, const GameDataString &initialString,
				bool isRenderShadow, const Graphics::GLByteARGB &shadowColor, const UIVector2D &shadowOffset)
				: UIObject(ownerManager)
				, m_typesettingGeneratorFunc(typesettingGeneratorFunc), m_typesetting(nullptr)
				, m_isDirty(true), m_toChangeString(initialString)
			{
				m_textRenderingComponent = std::make_shared<Rendering::TextRenderingComponent>(nullptr, isRenderShadow, shadowColor, shadowOffset);
				RegisterComponent(m_textRenderingComponent);
			}

			//------------------------------------------------------------------------------
			UIElementVariantText::~UIElementVariantText()
			{
			}

			//------------------------------------------------------------------------------
			void UIElementVariantText::Prepare()
			{
				if (m_typesetting && !m_isDirty)
				{
					return;
				}
				m_typesetting = m_typesettingGeneratorFunc(m_toChangeString);
				m_isDirty = false;
				m_currentString = m_toChangeString;
				m_textRenderingComponent->SetTypesetting(m_typesetting);
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UIObject> UIElementVariantText::Clone() const
			{
				std::shared_ptr<UIManager> managerCopy(m_ownerManager.lock());
				if (!managerCopy)
				{
					return nullptr;
				}

				std::shared_ptr<UIElementVariantText> clonedObject(new UIElementVariantText(managerCopy, m_typesettingGeneratorFunc, m_toChangeString));

				// Clone UIObject Data
				clonedObject->m_isVisible = m_isVisible;
				clonedObject->m_isUpdating = m_isUpdating;
				clonedObject->m_isEnable = m_isEnable;
				clonedObject->m_isParentEnable = m_isParentEnable;

				clonedObject->ChangeLayoutComponent(std::dynamic_pointer_cast<UILayoutComponent>(m_layoutComponent->Clone(*clonedObject)));

				// Copy Object and Tree
				std::unordered_set<UIComponentType, Utility::SyntacticSugar::EnumClassHash> copiedComponentSet;
				copiedComponentSet.insert(UIComponentType::kRendering);
				while (copiedComponentSet.size() < m_components.size())
				{
					bool isCopiedLoop = false;
					for (std::unordered_map<UIComponentType, std::shared_ptr<IUIComponent>, Utility::SyntacticSugar::EnumClassHash>::const_iterator it = m_components.begin(); it != m_components.end(); ++it)
					{
						if (copiedComponentSet.find(it->first) != copiedComponentSet.end())
						{
							continue;
						}

						const std::vector<UIComponentType> &dependencyList = it->second->GetDependentComponentTypes();
						bool isUnResolvedDependencyExists = false;
						for (const UIComponentType &dependingComponentType : dependencyList)
						{
							if (copiedComponentSet.find(dependingComponentType) == copiedComponentSet.end())
							{
								isUnResolvedDependencyExists = true;
								break;
							}
						}
						if (isUnResolvedDependencyExists)
						{
							continue;
						}

						isCopiedLoop = true;
						copiedComponentSet.insert(it->first);
						std::shared_ptr<IUIComponent> clonedComponent(it->second->Clone(*clonedObject));
						if (clonedComponent)
						{
							clonedObject->m_components[it->first] = clonedComponent;
						}
					}
					if (!isCopiedLoop)
					{
						// Cyclic Component Dependency Exists
						break;
					}
				}

				clonedObject->m_preChildren.reserve(m_preChildren.size());
				for (const std::shared_ptr<UIObject> &currentChild : m_preChildren)
				{
					std::shared_ptr<UIObject> clonedChild(currentChild->Clone());
					if (clonedChild)
					{
						clonedChild->UpdateParent(*clonedObject);
						clonedObject->m_preChildren.push_back((clonedChild));
					}
				}
				clonedObject->m_postChildren.reserve(m_postChildren.size());
				for (const std::shared_ptr<UIObject> &currentChild : m_postChildren)
				{
					std::shared_ptr<UIObject> clonedChild(currentChild->Clone());
					if (clonedChild)
					{
						clonedChild->UpdateParent(*clonedObject);
						clonedObject->m_postChildren.push_back((clonedChild));
					}
				}

				return clonedObject;
			}

			//------------------------------------------------------------------------------
			void UIElementVariantText::GraphicPrepare()
			{
				if (m_typesetting && !m_isDirty)
				{
					return;
				}
				m_typesetting = m_typesettingGeneratorFunc(m_toChangeString);
				m_isDirty = false;
				m_currentString = m_toChangeString;
				m_textRenderingComponent->SetTypesetting(m_typesetting);
			}

			//------------------------------------------------------------------------------
			void UIElementVariantText::FrameGraphicPrepare(const UIRenderer &renderer, UIVector2D offsetPosition, const IUIClipping &effectiveClipping)
			{
				Prepare();
			}

			//------------------------------------------------------------------------------
			UIElementSize UIElementVariantText::CalculateSize()
			{
				if (!m_typesetting)
				{
					return UIElementSize();
				}
				return UIElementSize(m_typesetting->boundingBox.GetWidth(), m_typesetting->logicalHeight);
			}

			//------------------------------------------------------------------------------
			UICoordinateType UIElementVariantText::CalculateWidth()
			{
				if (!m_typesetting)
				{
					return static_cast<UICoordinateType>(0.0f);
				}
				return (UICoordinateType)m_typesetting->boundingBox.GetWidth();
			}

			//------------------------------------------------------------------------------
			UICoordinateType UIElementVariantText::CalculateHeight()
			{
				if (!m_typesetting)
				{
					return static_cast<UICoordinateType>(0.0f);
				}
				return static_cast<UICoordinateType>(m_typesetting->logicalHeight);
			}

			//------------------------------------------------------------------------------
			void UIElementVariantText::SetString(const GameDataString &stringToSet)
			{
				if (m_isDirty)
				{
					m_toChangeString = stringToSet;
				}
				else
				{
					if (m_currentString == stringToSet)
					{
						return;
					}
					m_isDirty = true;
					m_toChangeString = stringToSet;
				}
			}

			//------------------------------------------------------------------------------
			void UIElementVariantText::SetMultiplyColor(Graphics::GLByteARGB multiplyColor)
			{
				if (!m_textRenderingComponent)
				{
					return;
				}
				m_textRenderingComponent->SetGlobalMultiplyColor(multiplyColor);
			}

			//------------------------------------------------------------------------------
			const Graphics::GLByteARGB UIElementVariantText::GetMultiplyColor() const
			{
				if (!m_textRenderingComponent)
				{
					return Graphics::GLByteARGB::kTransparent;
				}
				return m_textRenderingComponent->GetGlobalMultiplyColor();
			}

			//------------------------------------------------------------------------------
			void UIElementVariantText::TurnShadow(bool isTurnOn)
			{
				if (!m_textRenderingComponent)
				{
					return;
				}
				m_textRenderingComponent->TurnShadow(isTurnOn);
			}

			//------------------------------------------------------------------------------
			void UIElementVariantText::SetShadow(const Graphics::GLByteARGB &shadowColor, const UIVector2D &shadowOffset)
			{
				if (!m_textRenderingComponent)
				{
					return;
				}
				m_textRenderingComponent->SetShadow(shadowColor, shadowOffset);
			}
		}
	}
}
