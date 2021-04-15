////////////////////////////////////////////////////////////////////////////////
// Component/UIClippingComponent.cpp (Leggiero/Modules - LegacyUI)
//
// UI Clipping Component Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIClippingComponent.h"

// Standard Library
#include <initializer_list>

// Leggiero.LegacyUI
#include "../Common/UITransform.h"
#include "../UIObject.h"


namespace Leggiero
{
	namespace LUI
	{
		//////////////////////////////////////////////////////////////////////////////// UIRectangularClipping

		//------------------------------------------------------------------------------
		UIRectangularClipping::UIRectangularClipping(UICoordinateType left, UICoordinateType top, UICoordinateType right, UICoordinateType bottom)
			: m_clippingRectArea(left, top, right, bottom)
		{
			m_isClipping = true;
		}

		//------------------------------------------------------------------------------
		UIRectangularClipping::UIRectangularClipping(const UISimpleRectangular &clippingRect)
			: m_clippingRectArea(clippingRect)
		{
			m_isClipping = true;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<UIRectangularClipping> UIRectangularClipping::CreateByPositionSize(UICoordinateType left, UICoordinateType top, UICoordinateType width, UICoordinateType height)
		{
			return std::make_shared<UIRectangularClipping>(left, top, left + width, top + height);
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<UIRectangularClipping> UIRectangularClipping::CreateByPositionSize(UIVector2D position, UICoordinateType width, UICoordinateType height)
		{
			return std::make_shared<UIRectangularClipping>(position.x, position.y, position.x + width, position.y + height);
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<IUIComponent> UIRectangularClipping::Clone(const UIObject &ownerObject) const
		{
			std::shared_ptr<UIRectangularClipping> createdComponent(std::make_shared<UIRectangularClipping>(m_clippingRectArea));

			return createdComponent;
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<const IUIClipping> UIRectangularClipping::GetEffectiveClipping(const std::shared_ptr<const IUIClipping> &parentClipping, std::shared_ptr<IUITransform> transform)
		{
			if (!m_isClipping)
			{
				return parentClipping;
			}

			// Components cache their own clipping: UI processing can assume in single-thread
			std::shared_ptr<UIClippingConvexPolygon> myClipping(m_cachedClippingPolygon);
			if (!myClipping)
			{
				myClipping = std::make_shared<UIClippingConvexPolygon>(std::initializer_list<UIVector2D>({
					transform->ApplyTransform(m_clippingRectArea.left, m_clippingRectArea.top),
					transform->ApplyTransform(m_clippingRectArea.right, m_clippingRectArea.top),
					transform->ApplyTransform(m_clippingRectArea.right, m_clippingRectArea.bottom),
					transform->ApplyTransform(m_clippingRectArea.left, m_clippingRectArea.bottom),
					}));
			}
			else
			{
				std::vector<UIVector2D> transformedPolygon = {
					transform->ApplyTransform(m_clippingRectArea.left, m_clippingRectArea.top),
					transform->ApplyTransform(m_clippingRectArea.right, m_clippingRectArea.top),
					transform->ApplyTransform(m_clippingRectArea.right, m_clippingRectArea.bottom),
					transform->ApplyTransform(m_clippingRectArea.left, m_clippingRectArea.bottom)
				};
				myClipping->AccessRawPolygon().swap(transformedPolygon);
				if (!myClipping->IsValid())
				{
					myClipping->ForceUpdatePolygonStatus();
				}
			}

			if (!m_cachedClippingPolygon)
			{
				m_cachedClippingPolygon = myClipping;
			}

			return parentClipping->Combine(myClipping);
		}

		//------------------------------------------------------------------------------
		void UIRectangularClipping::SetRectangle(UICoordinateType left, UICoordinateType top, UICoordinateType right, UICoordinateType bottom)
		{
			m_clippingRectArea.left = left;
			m_clippingRectArea.top = top;
			m_clippingRectArea.right = right;
			m_clippingRectArea.bottom = bottom;
		}
	}
}
