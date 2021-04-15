////////////////////////////////////////////////////////////////////////////////
// Element/UIElementFixedText.cpp (Leggiero/Modules - LegacyUI)
//
// Fixed Text Element Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIElementFixedText.h"

// Leggiero.LegacyUI
#include "../Component/UILayoutComponent.h"
#include "../Rendering/TextRenderingComponent.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Element
		{
			//////////////////////////////////////////////////////////////////////////////// UIElementFixedText

			// Dummy Generator Function
			UIElementFixedText::TypesettingGeneratorType UIElementFixedText::ms_dummyTypesettingGenerator([]() { return nullptr; });

			//------------------------------------------------------------------------------
			UIElementFixedText::UIElementFixedText(std::shared_ptr<UIManager> ownerManager, TypesettingGeneratorType typesettingGeneratorFunc,
				bool isRenderShadow, const Graphics::GLByteARGB &shadowColor, const UIVector2D &shadowOffset)
				: UIObject(ownerManager)
				, m_typesettingGeneratorFunc(typesettingGeneratorFunc), m_typesetting(nullptr)
			{
				m_textRenderingComponent = std::make_shared<Rendering::TextRenderingComponent>(nullptr, isRenderShadow, shadowColor, shadowOffset);
				RegisterComponent(m_textRenderingComponent);
			}

			//------------------------------------------------------------------------------
			UIElementFixedText::UIElementFixedText(std::shared_ptr<UIManager> ownerManager, std::shared_ptr<Font::CachedGlyphTypesetting> typesetting,
				bool isRenderShadow, const Graphics::GLByteARGB &shadowColor, const UIVector2D &shadowOffset)
				: UIObject(ownerManager)
				, m_typesettingGeneratorFunc(ms_dummyTypesettingGenerator), m_typesetting(typesetting)
			{
				m_textRenderingComponent = std::make_shared<Rendering::TextRenderingComponent>(typesetting, isRenderShadow, shadowColor, shadowOffset);
				RegisterComponent(m_textRenderingComponent);
			}

			//------------------------------------------------------------------------------
			UIElementFixedText::~UIElementFixedText()
			{
			}

			//------------------------------------------------------------------------------
			void UIElementFixedText::Prepare()
			{
				if (m_typesetting)
				{
					return;
				}
				m_typesetting = m_typesettingGeneratorFunc();
				m_textRenderingComponent->SetTypesetting(m_typesetting);
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UIObject> UIElementFixedText::Clone() const
			{
				std::shared_ptr<UIObject> clonedObject(UIObject::Clone());
				std::shared_ptr<UIElementFixedText> concreteCloned(std::dynamic_pointer_cast<UIElementFixedText>(clonedObject));
				if (!concreteCloned)
				{
					return clonedObject;
				}

				concreteCloned->m_typesettingGeneratorFunc = m_typesettingGeneratorFunc;
				concreteCloned->m_typesetting = m_typesetting;
				concreteCloned->m_textRenderingComponent = concreteCloned->GetComponentObject<Rendering::TextRenderingComponent>(UIComponentType::kRendering);

				return concreteCloned;
			}

			//------------------------------------------------------------------------------
			UIElementSize UIElementFixedText::CalculateSize()
			{
				if (!m_typesetting)
				{
					return UIElementSize();
				}
				return UIElementSize(m_typesetting->boundingBox.GetWidth(), m_typesetting->logicalHeight);
			}

			//------------------------------------------------------------------------------
			UICoordinateType UIElementFixedText::CalculateWidth()
			{
				if (!m_typesetting)
				{
					return static_cast<UICoordinateType>(0.0f);
				}
				return (UICoordinateType)m_typesetting->boundingBox.GetWidth();
			}

			//------------------------------------------------------------------------------
			UICoordinateType UIElementFixedText::CalculateHeight()
			{
				if (!m_typesetting)
				{
					return static_cast<UICoordinateType>(0.0f);
				}
				return (UICoordinateType)m_typesetting->logicalHeight;
			}

			//------------------------------------------------------------------------------
			void UIElementFixedText::GraphicPrepare()
			{
				if (m_typesetting)
				{
					return;
				}
				m_typesetting = m_typesettingGeneratorFunc();
				m_textRenderingComponent->SetTypesetting(m_typesetting);
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<UIObject> UIElementFixedText::_AllocateCloningObject() const
			{
				std::shared_ptr<UIManager> managerCopy(m_ownerManager.lock());
				if (!managerCopy)
				{
					return nullptr;
				}
				return std::shared_ptr<UIObject>(dynamic_cast<UIObject *>(new UIElementFixedText(managerCopy)));
			}

			//------------------------------------------------------------------------------
			void UIElementFixedText::SetMultiplyColor(Graphics::GLByteARGB multiplyColor)
			{
				if (!m_textRenderingComponent)
				{
					return;
				}
				m_textRenderingComponent->SetGlobalMultiplyColor(multiplyColor);
			}

			//------------------------------------------------------------------------------
			const Graphics::GLByteARGB UIElementFixedText::GetMultiplyColor() const
			{
				if (!m_textRenderingComponent)
				{
					return Graphics::GLByteARGB::kTransparent;
				}
				return m_textRenderingComponent->GetGlobalMultiplyColor();
			}

			//------------------------------------------------------------------------------
			void UIElementFixedText::TurnShadow(bool isTurnOn)
			{
				if (!m_textRenderingComponent)
				{
					return;
				}
				m_textRenderingComponent->TurnShadow(isTurnOn);
			}

			//------------------------------------------------------------------------------
			void UIElementFixedText::SetShadow(const Graphics::GLByteARGB &shadowColor, const UIVector2D &shadowOffset)
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
