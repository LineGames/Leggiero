////////////////////////////////////////////////////////////////////////////////
// Touch/ValuedTouchComponent.h (Leggiero/Modules - LegacyUI)
//
// Touch interaction component by given values
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__TOUCH__VALUED_TOUCH_COMPONENT_H
#define __LM_LUI__TOUCH__VALUED_TOUCH_COMPONENT_H


// Standard Library
#include <functional>

// Leggiero.LegacyUI
#include "UITouchComponent.h"


namespace Leggiero
{
	namespace LUI
	{
		// Touch component by values
		class ValuedTouchComponent
			: public UITouchComponent
		{
		public:
			ValuedTouchComponent();
			virtual ~ValuedTouchComponent() { }

		public:	// IUIComponent
			virtual std::shared_ptr<IUIComponent> Clone(const UIObject &ownerObject) const override;

		public:	// UITouchComponent
			virtual std::shared_ptr<IUIArea> GetInteractionArea() const override { return m_interactionArea; }
			virtual std::shared_ptr<IUIArea> GetCoverArea() const override { return m_coverArea; }

		public:	// UITouchComponent
			virtual void OnTouchDown(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time, bool isCoveredByDescendant) override { if (m_touchDownFunc) m_touchDownFunc(touch, x, y, time, isCoveredByDescendant); }
			virtual void OnTouchIn(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time, bool isFirstIn, bool isCoveredByDescendant) override { if (m_touchInFunc) m_touchInFunc(touch, x, y, time, isFirstIn, isCoveredByDescendant); }

			virtual void OnTouchMove(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time) override { if (m_touchMoveFunc) m_touchMoveFunc(touch, x, y, time); }

			virtual void OnTouchOut(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time) override { if (m_touchOutFunc) m_touchOutFunc(touch, x, y, time); }
			virtual void OnTouchCovered(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time) override { if (m_touchCoveredFunc) m_touchCoveredFunc(touch, x, y, time); }
			virtual void OnTouchCancel(std::shared_ptr<UITouch> touch, GameTimeClockType::time_point time) override { if (m_touchCancelFunc) m_touchCancelFunc(touch, time); }
			virtual void OnTouchUp(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time) override { if (m_touchUpFunc) m_touchUpFunc(touch, x, y, time); }

			virtual bool OnPrimaryTouchSteal(std::shared_ptr<UITouch> touch, std::shared_ptr<UIObject> stealer) override { return (m_primaryTouchStealFunc ? m_primaryTouchStealFunc(touch, stealer) : true); }
			virtual void OnPrimaryTouchHolderChanged(std::shared_ptr<UITouch> touch, std::shared_ptr<UIObject> primaryHolder) override { if (m_primaryTouchHolderChangedFunc) m_primaryTouchHolderChangedFunc(touch, primaryHolder); }

			virtual void OnPrimaryTouchGiven(std::shared_ptr<UITouch> touch) override { if (m_primaryTouchGivenFunc) m_primaryTouchGivenFunc(touch); }
			virtual void OnPrimaryTouchLose(std::shared_ptr<UITouch> touch) override { if (m_primaryTouchLoseFunc) m_primaryTouchLoseFunc(touch); }

		public:
			void SetInteractionArea(std::shared_ptr<IUIArea> area) { m_interactionArea = area; };
			void SetCoverArea(std::shared_ptr<IUIArea> area) { m_coverArea = area; };

			void SetTouchDownFunc(std::function<void(std::shared_ptr<UITouch>, UICoordinateType, UICoordinateType, GameTimeClockType::time_point, bool)> handlerFunc) { m_touchDownFunc = handlerFunc; }
			void SetTouchInFunc(std::function<void(std::shared_ptr<UITouch>, UICoordinateType, UICoordinateType, GameTimeClockType::time_point, bool, bool)> handlerFunc) { m_touchInFunc = handlerFunc; }

			void SetTouchMoveFunc(std::function<void(std::shared_ptr<UITouch>, UICoordinateType, UICoordinateType, GameTimeClockType::time_point)> handlerFunc) { m_touchMoveFunc = handlerFunc; }

			void SetTouchOutFunc(std::function<void(std::shared_ptr<UITouch>, UICoordinateType, UICoordinateType, GameTimeClockType::time_point)> handlerFunc) { m_touchOutFunc = handlerFunc; }
			void SetTouchCoveredFunc(std::function<void(std::shared_ptr<UITouch>, UICoordinateType, UICoordinateType, GameTimeClockType::time_point)> handlerFunc) { m_touchCoveredFunc = handlerFunc; }
			void SetTouchCancelFunc(std::function<void(std::shared_ptr<UITouch>, GameTimeClockType::time_point)> handlerFunc) { m_touchCancelFunc = handlerFunc; }
			void SetTouchUpFunc(std::function<void(std::shared_ptr<UITouch>, UICoordinateType, UICoordinateType, GameTimeClockType::time_point)> handlerFunc) { m_touchUpFunc = handlerFunc; }

			void SetPrimaryTouchStealFunc(std::function<bool(std::shared_ptr<UITouch>, std::shared_ptr<UIObject>)> handlerFunc) { m_primaryTouchStealFunc = handlerFunc; }
			void SetPrimaryTouchHolderChangedFunc(std::function<void(std::shared_ptr<UITouch>, std::shared_ptr<UIObject>)> handlerFunc) { m_primaryTouchHolderChangedFunc = handlerFunc; }

			void SetPrimaryTouchGivenFunc(std::function<void(std::shared_ptr<UITouch>)> handlerFunc) { m_primaryTouchGivenFunc = handlerFunc; }
			void SetPrimaryTouchLoseFunc(std::function<void(std::shared_ptr<UITouch>)> handlerFunc) { m_primaryTouchLoseFunc = handlerFunc; }

		protected:
			std::shared_ptr<IUIArea> m_interactionArea;
			std::shared_ptr<IUIArea> m_coverArea;

			std::function<void(std::shared_ptr<UITouch>, UICoordinateType, UICoordinateType, GameTimeClockType::time_point, bool)> m_touchDownFunc;
			std::function<void(std::shared_ptr<UITouch>, UICoordinateType, UICoordinateType, GameTimeClockType::time_point, bool, bool)> m_touchInFunc;

			std::function<void(std::shared_ptr<UITouch>, UICoordinateType, UICoordinateType, GameTimeClockType::time_point)> m_touchMoveFunc;

			std::function<void(std::shared_ptr<UITouch>, UICoordinateType, UICoordinateType, GameTimeClockType::time_point)> m_touchOutFunc;
			std::function<void(std::shared_ptr<UITouch>, UICoordinateType, UICoordinateType, GameTimeClockType::time_point)> m_touchCoveredFunc;
			std::function<void(std::shared_ptr<UITouch>, GameTimeClockType::time_point)> m_touchCancelFunc;
			std::function<void(std::shared_ptr<UITouch>, UICoordinateType, UICoordinateType, GameTimeClockType::time_point)> m_touchUpFunc;

			std::function<bool(std::shared_ptr<UITouch>, std::shared_ptr<UIObject>)> m_primaryTouchStealFunc;
			std::function<void(std::shared_ptr<UITouch>, std::shared_ptr<UIObject>)> m_primaryTouchHolderChangedFunc;

			std::function<void(std::shared_ptr<UITouch>)> m_primaryTouchGivenFunc;
			std::function<void(std::shared_ptr<UITouch>)> m_primaryTouchLoseFunc;
		};
	}
}

#endif
