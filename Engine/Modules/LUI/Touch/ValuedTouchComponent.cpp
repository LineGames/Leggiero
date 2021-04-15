////////////////////////////////////////////////////////////////////////////////
// Touch/ValuedTouchComponent.cpp (Leggiero/Modules - LegacyUI)
//
// Implementation of valued touch component
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "ValuedTouchComponent.h"

// Leggiero.LegacyUI
#include "../Common/IUIArea.h"


namespace Leggiero
{
	namespace LUI
	{
		//////////////////////////////////////////////////////////////////////////////// ValuedTouchComponent

		//------------------------------------------------------------------------------
		ValuedTouchComponent::ValuedTouchComponent()
			: m_interactionArea(IUIArea::kNoArea)
			, m_touchDownFunc([](std::shared_ptr<UITouch> _1, UICoordinateType _2, UICoordinateType _3, GameTimeClockType::time_point _4, bool _5) { })
			, m_touchInFunc([](std::shared_ptr<UITouch> _1, UICoordinateType _2, UICoordinateType _3, GameTimeClockType::time_point _4, bool _5, bool _6) { })
			, m_touchMoveFunc([](std::shared_ptr<UITouch> _1, UICoordinateType _2, UICoordinateType _3, GameTimeClockType::time_point _4) { })
			, m_touchOutFunc([](std::shared_ptr<UITouch> _1, UICoordinateType _2, UICoordinateType _3, GameTimeClockType::time_point _4) { })
			, m_touchCoveredFunc([](std::shared_ptr<UITouch> _1, UICoordinateType _2, UICoordinateType _3, GameTimeClockType::time_point _4) { })
			, m_touchCancelFunc([](std::shared_ptr<UITouch> _1, GameTimeClockType::time_point _4) { })
			, m_touchUpFunc([](std::shared_ptr<UITouch> _1, UICoordinateType _2, UICoordinateType _3, GameTimeClockType::time_point _4) { })
			, m_primaryTouchStealFunc([](std::shared_ptr<UITouch> _1, std::shared_ptr<UIObject>) { return true; })
			, m_primaryTouchHolderChangedFunc([](std::shared_ptr<UITouch> _1, std::shared_ptr<UIObject>) { })
			, m_primaryTouchGivenFunc([](std::shared_ptr<UITouch> _1) { })
			, m_primaryTouchLoseFunc([](std::shared_ptr<UITouch> _1) { })
		{
		}

		//------------------------------------------------------------------------------
		std::shared_ptr<IUIComponent> ValuedTouchComponent::Clone(const UIObject &ownerObject) const
		{
			std::shared_ptr<ValuedTouchComponent> copiedComponent(std::make_shared<ValuedTouchComponent>());
			copiedComponent->m_interactionArea = m_interactionArea;
			copiedComponent->m_coverArea = m_coverArea;
			copiedComponent->m_touchDownFunc = m_touchDownFunc;
			copiedComponent->m_touchInFunc = m_touchInFunc;
			copiedComponent->m_touchMoveFunc = m_touchMoveFunc;
			copiedComponent->m_touchOutFunc = m_touchOutFunc;
			copiedComponent->m_touchCoveredFunc = m_touchCoveredFunc;
			copiedComponent->m_touchCancelFunc = m_touchCancelFunc;
			copiedComponent->m_touchUpFunc = m_touchUpFunc;
			copiedComponent->m_primaryTouchStealFunc = m_primaryTouchStealFunc;
			copiedComponent->m_primaryTouchHolderChangedFunc = m_primaryTouchHolderChangedFunc;
			copiedComponent->m_primaryTouchGivenFunc = m_primaryTouchGivenFunc;
			copiedComponent->m_primaryTouchLoseFunc = m_primaryTouchLoseFunc;

			return copiedComponent;
		}
	}
}
