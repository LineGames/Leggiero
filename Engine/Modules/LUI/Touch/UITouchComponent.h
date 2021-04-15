////////////////////////////////////////////////////////////////////////////////
// Touch/UITouchComponent.h (Leggiero/Modules - LegacyUI)
//
// Touch interaction component of an UI object
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__TOUCH__UI_TOUCH_COMPONENT_H
#define __LM_LUI__TOUCH__UI_TOUCH_COMPONENT_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"
#include "../Component/IUIComponent.h"


namespace Leggiero
{
	namespace LUI
	{
		// Forward Declarations
		class UITouch;
		class IUIArea;
		class UIObject;


		// Base Touch Interaction UI Component
		class UITouchComponent
			: public IUIComponent
		{
		public:
			UITouchComponent() { }
			virtual ~UITouchComponent() { }

		public:	// IUIComponent
			virtual UIComponentType GetComponentType() const override { return UIComponentType::kTouchInteraction; }

		public:
			virtual std::shared_ptr<IUIArea> GetInteractionArea() const;
			virtual std::shared_ptr<IUIArea> GetCoverArea() const { return nullptr; }

		public:
			virtual void OnTouchDown(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time, bool isCoveredByDescendant) { }
			virtual void OnTouchIn(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time, bool isFirstIn, bool isCoveredByDescendant) { }

			virtual void OnTouchMove(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time) { }

			virtual void OnTouchOut(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time) { }
			virtual void OnTouchCovered(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time) { }
			virtual void OnTouchCancel(std::shared_ptr<UITouch> touch, GameTimeClockType::time_point time) { }
			virtual void OnTouchUp(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time) { }

			virtual bool OnPrimaryTouchSteal(std::shared_ptr<UITouch> touch, std::shared_ptr<UIObject> stealer) { return true; }
			virtual void OnPrimaryTouchHolderChanged(std::shared_ptr<UITouch> touch, std::shared_ptr<UIObject> primaryHolder) { }

			virtual void OnPrimaryTouchGiven(std::shared_ptr<UITouch> touch) { }
			virtual void OnPrimaryTouchLose(std::shared_ptr<UITouch> touch) { }
		};
	}
}

#endif
