////////////////////////////////////////////////////////////////////////////////
// Component/UISizeSubComponent.cpp (Leggiero/Modules - LegacyUI)
//
// Implementation of size sub-components
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UISizeSubComponent.h"

// Leggiero.LegacyUI
#include "../UIObject.h"
#include "../Rendering/UIRenderingComponent.h"
#include "UILayoutComponent.h"


namespace Leggiero
{
	namespace LUI
	{
		//////////////////////////////////////////////////////////////////////////////// UIValuedSizeSubComponent

		//------------------------------------------------------------------------------
		UIValuedSizeSubComponent::UIValuedSizeSubComponent(UICoordinateType width, UICoordinateType height)
			: m_width(width), m_height(height)
		{
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<IUISizeSubComponent> UIValuedSizeSubComponent::Clone() const
		{
			std::shared_ptr<UIValuedSizeSubComponent> clonedSizing(std::make_shared<UIValuedSizeSubComponent>(m_width, m_height));
			return clonedSizing;
		}


		//////////////////////////////////////////////////////////////////////////////// UIObjectSizeSubComponent

		//------------------------------------------------------------------------------
		UIElementSize UIObjectSizeSubComponent::CalculateSize()
		{
			std::shared_ptr<UIObject> targetObject(m_targetObject.lock());
			if (!targetObject)
			{
				return UIElementSize();
			}

			return targetObject->CalculateSize();
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<IUISizeSubComponent> UIObjectSizeSubComponent::Clone() const
		{
			std::shared_ptr<UIObjectSizeSubComponent> clonedSizing(std::make_shared<UIObjectSizeSubComponent>(m_targetObject.lock()));
			return clonedSizing;
		}


		//////////////////////////////////////////////////////////////////////////////// UIVisualSizeSubComponent

		//------------------------------------------------------------------------------
		UIVisualSizeSubComponent::UIVisualSizeSubComponent(const std::shared_ptr<UIObject> &targetObject)
			: IOwnerTargetingSizeSubComponent(targetObject)
		{
		}

		//------------------------------------------------------------------------------
		UIElementSize UIVisualSizeSubComponent::CalculateSize()
		{
			std::shared_ptr<UIObject> targetObject(m_targetObject.lock());
			if (!targetObject)
			{
				return UIElementSize();
			}

			std::shared_ptr<UIRenderingComponent> renderingComponent(targetObject->GetComponentObject<UIRenderingComponent>(UIComponentType::kRendering));
			if (!renderingComponent)
			{
				return UIElementSize();
			}

			return renderingComponent->GetVisualSize();
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<IUISizeSubComponent> UIVisualSizeSubComponent::Clone() const
		{
			std::shared_ptr<UIVisualSizeSubComponent> clonedSizing(std::make_shared<UIVisualSizeSubComponent>(m_targetObject.lock()));
			return clonedSizing;
		}


		//////////////////////////////////////////////////////////////////////////////// UIParentSizeSubComponent

		//------------------------------------------------------------------------------
		UIParentSizeSubComponent::UIParentSizeSubComponent(std::shared_ptr<UILayoutComponent> parentLayoutComponent)
			: m_parentLayoutComponent(parentLayoutComponent)
		{
		}

		//------------------------------------------------------------------------------
		UIElementSize UIParentSizeSubComponent::CalculateSize()
		{
			std::shared_ptr<UILayoutComponent> parentLayout(m_parentLayoutComponent.lock());
			if (!parentLayout)
			{
				return UIElementSize();
			}

			std::shared_ptr<IUISizeSubComponent> parentSizeSubComponent(parentLayout->GetSizeSubComponent());
			if (!parentSizeSubComponent)
			{
				return UIElementSize();
			}
			return parentSizeSubComponent->CalculateSize();
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<IUISizeSubComponent> UIParentSizeSubComponent::Clone() const
		{
			std::shared_ptr<UIParentSizeSubComponent> clonedSizing(std::make_shared<UIParentSizeSubComponent>(m_parentLayoutComponent.lock()));
			return clonedSizing;
		}

		//------------------------------------------------------------------------------
		void UIParentSizeSubComponent::SetParentOfOwnerObject(const UIObject &parentObject)
		{
			m_parentLayoutComponent.reset();

			std::shared_ptr<UILayoutComponent> parentLayout(parentObject.GetLayoutComponent());
			if (!parentLayout)
			{
				return;
			}

			m_parentLayoutComponent = parentLayout;
		}

		//------------------------------------------------------------------------------
		void UIParentSizeSubComponent::SetNoParentOfOwnerObject()
		{
			m_parentLayoutComponent.reset();
		}

		//------------------------------------------------------------------------------
		bool UIParentSizeSubComponent::IsParentDependencySolved() const
		{
			std::shared_ptr<UILayoutComponent> parentLayout(m_parentLayoutComponent.lock());
			if (!parentLayout)
			{
				return false;
			}

			std::shared_ptr<IUISizeSubComponent> parentSizeSubComponent(parentLayout->GetSizeSubComponent());
			if (!parentSizeSubComponent)
			{
				return false;
			}

			if (!parentSizeSubComponent->HasParentDependency())
			{
				return true;
			}

			return parentSizeSubComponent->IsParentDependencySolved();
		}


		//////////////////////////////////////////////////////////////////////////////// UIChildrenStackSizeSubComponent

		//------------------------------------------------------------------------------
		UIChildrenStackSizeSubComponent::UIChildrenStackSizeSubComponent(const std::shared_ptr<UIObject> &targetObject, bool isHorizontal)
			: IOwnerTargetingSizeSubComponent(targetObject)
			, m_isHorizontal(isHorizontal)
		{
		}

		//------------------------------------------------------------------------------
		UIElementSize UIChildrenStackSizeSubComponent::CalculateSize()
		{
			std::shared_ptr<UIObject> targetObject(m_targetObject.lock());
			if (!targetObject)
			{
				return UIElementSize();
			}

			UICoordinateType currentWidth = static_cast<UICoordinateType>(0.0f);
			UICoordinateType currentHeight = static_cast<UICoordinateType>(0.0f);

			for (std::shared_ptr<UIObject> currentChild : targetObject->GetPreChildren())
			{
				if (currentChild->IsVirtualLogicObject())
				{
					continue;
				}
				std::shared_ptr<UILayoutComponent> childLayout(currentChild->GetLayoutComponent());
				if (!childLayout)
				{
					continue;
				}
				std::shared_ptr<IUISizeSubComponent> childSizeSubComponent(childLayout->GetSizeSubComponent());
				if (!childSizeSubComponent)
				{
					continue;
				}
				UIElementSize childSize = childSizeSubComponent->CalculateSize();
				if (m_isHorizontal)
				{
					currentWidth += childSize.width;
					if (childSize.height > currentHeight)
					{
						currentHeight = childSize.height;
					}
				}
				else
				{
					currentHeight += childSize.height;
					if (childSize.width > currentWidth)
					{
						currentWidth = childSize.width;
					}
				}
			}

			for (std::shared_ptr<UIObject> currentChild : targetObject->GetPostChildren())
			{
				if (currentChild->IsVirtualLogicObject())
				{
					continue;
				}
				std::shared_ptr<UILayoutComponent> childLayout(currentChild->GetLayoutComponent());
				if (!childLayout)
				{
					continue;
				}
				std::shared_ptr<IUISizeSubComponent> childSizeSubComponent(childLayout->GetSizeSubComponent());
				if (!childSizeSubComponent)
				{
					continue;
				}
				UIElementSize childSize = childSizeSubComponent->CalculateSize();
				if (m_isHorizontal)
				{
					currentWidth += childSize.width;
					if (childSize.height > currentHeight)
					{
						currentHeight = childSize.height;
					}
				}
				else
				{
					currentHeight += childSize.height;
					if (childSize.width > currentWidth)
					{
						currentWidth = childSize.width;
					}
				}
			}

			return UIElementSize(currentWidth, currentHeight);
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<IUISizeSubComponent> UIChildrenStackSizeSubComponent::Clone() const
		{
			std::shared_ptr<UIChildrenStackSizeSubComponent> clonedSizing(std::make_shared<UIChildrenStackSizeSubComponent>(m_targetObject.lock(), m_isHorizontal));
			return clonedSizing;
		}


		//////////////////////////////////////////////////////////////////////////////// ModifedSizeSubComponent

		//------------------------------------------------------------------------------
		ModifedSizeSubComponent::ModifedSizeSubComponent(std::shared_ptr<IUISizeSubComponent> original,
			UICoordinateType widthAdd, UICoordinateType heightAdd,
			UICoordinateRatioType widthMultiplier, UICoordinateRatioType heightMultiplier)
			: m_original(original)
			, m_widthAdd(widthAdd), m_heightAdd(heightAdd)
			, m_widthMultiplier(widthMultiplier), m_heightMultiplier(heightMultiplier)
		{ }

		//------------------------------------------------------------------------------
		UIElementSize ModifedSizeSubComponent::CalculateSize()
		{
			UIElementSize originalSize(m_original->CalculateSize());

			return UIElementSize(
				static_cast<UICoordinateType>(originalSize.width * m_widthMultiplier + m_widthAdd),
				static_cast<UICoordinateType>(originalSize.height * m_heightMultiplier + m_heightAdd));
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<IUISizeSubComponent> ModifedSizeSubComponent::Clone() const
		{
			std::shared_ptr<ModifedSizeSubComponent> clonedSizing(std::make_shared<ModifedSizeSubComponent>(m_original,
				m_widthAdd, m_heightAdd, m_widthMultiplier, m_heightMultiplier));
			return clonedSizing;
		}
	}
}
