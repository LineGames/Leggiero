////////////////////////////////////////////////////////////////////////////////
// Element/UIElementCancelPanelArea.h (Leggiero/Modules - LegacyUI)
//
// Panel to receive touch to cancel
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__ELEMENT__UI_ELEMENT_CANCEL_PANEL_AREA_H
#define __LM_LUI__ELEMENT__UI_ELEMENT_CANCEL_PANEL_AREA_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <functional>
#include <memory>

// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"
#include "../UIObject.h"


namespace Leggiero
{
	namespace LUI
	{
		// Forward Declarations
		class UIManager;
		class IUIArea;
		class UITouch;
		class ValuedTouchComponent;
		class UILayoutComponent;


		namespace Element
		{
			// Cancel Panel Element
			class CancelPanelArea
				: public UIObject
			{
			public:
				using OnCancelHandlerType = std::function<void(std::shared_ptr<CancelPanelArea>)>;

			public:
				CancelPanelArea(std::shared_ptr<UIManager> ownerManager, std::shared_ptr<IUIArea> cancelTouchArea, UICoordinateType moveThresholdAmount,
					std::shared_ptr<IUIArea> coveringArea = nullptr, bool isInitialLocked = false, bool isLazyLayout = false,
					OnCancelHandlerType onCancel = ms_dummyOnCancelHandler);
				CancelPanelArea(std::shared_ptr<UIManager> ownerManager, std::shared_ptr<IUIArea> cancelTouchArea, UICoordinateType moveThresholdAmount,
					std::shared_ptr<IUIArea> coveringArea = nullptr, bool isInitialLocked = false, std::shared_ptr<UILayoutComponent> layout = nullptr,
					OnCancelHandlerType onCancel = ms_dummyOnCancelHandler);
				virtual ~CancelPanelArea();

			protected:
				CancelPanelArea(std::shared_ptr<UIManager> ownerManager);

			public:	// UIObject
				virtual std::shared_ptr<UIObject> Clone() const override;

			protected:
				virtual std::shared_ptr<UIObject> _AllocateCloningObject() const override;

			public:
				void SetOnCancelHandler(OnCancelHandlerType handler) { m_onCancelHandlerFunc = handler; }
				void DiscardOnCancelHandler() { m_onCancelHandlerFunc = ms_dummyOnCancelHandler; }

				void SetCancelArea(std::shared_ptr<IUIArea> area);
				void SetCoveringArea(std::shared_ptr<IUIArea> area);

				UICoordinateType GetMoveThresholdAmount() const { return m_moveThreshold; }
				void SetMoveThreshold(UICoordinateType amount) { m_moveThreshold = amount; }

				void CancelCurrentTouch();
				bool IsLocked() const { return m_isLocked; }
				void SetIsLocked(bool isLocked)
				{
					m_isLocked = isLocked;
					if (isLocked)
					{
						CancelCurrentTouch();
					}
				}

			protected:
				void _OnTouchDown(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time, bool isCoveredByDescendant);

				void _OnTouchMove(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time);

				void _OnTouchOut(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time);
				void _OnTouchCovered(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time);
				void _OnTouchCancel(std::shared_ptr<UITouch> touch, GameTimeClockType::time_point time);
				void _OnTouchUp(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time);

				void _OnPrimaryTouchLose(std::shared_ptr<UITouch> touch);

				void _SetTouchPressedState(bool isPressed);

			protected:
				static OnCancelHandlerType	ms_dummyOnCancelHandler;
				OnCancelHandlerType			m_onCancelHandlerFunc;

				std::shared_ptr<ValuedTouchComponent>	m_touchComponent;
				std::shared_ptr<UITouch>				m_processingTouch;

				bool m_isPressed;
				bool m_isLocked;

				UICoordinateType m_moveThreshold;
			};
		}
	}
}

#endif
