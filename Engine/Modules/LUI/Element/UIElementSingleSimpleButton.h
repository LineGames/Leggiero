////////////////////////////////////////////////////////////////////////////////
// Element/UIElementSingleSimpleButton.h (Leggiero/Modules - LegacyUI)
//
// Single button element to process simple interaction
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__ELEMENT__UI_ELEMENT_SINGLE_SIMPLE_BUTTON_H
#define __LM_LUI__ELEMENT__UI_ELEMENT_SINGLE_SIMPLE_BUTTON_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <functional>
#include <memory>

// Leggiero.Utility
#include <Utility/Sugar/EnumClass.h>

// Leggiero.Graphics
#include <Graphics/Common/GLColor.h>

// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"
#include "../UIObject.h"


namespace Leggiero
{
	namespace LUI
	{
		// Forward Declarations
		class UIManager;
		class ValuedTouchComponent;
		class UITouch;


		namespace Element
		{
			// Simple UI Button
			class SingleSimpleButton
				: public UIObject
			{
			public:
				enum class StateType
				{
					kNormal = 0x0,

					kPushed = 0x1,
					kActive = 0x2,
					kDisabled = 0x4,
				};

			public:
				// Rendering Controller Definition for interaction
				class RenderingController
				{
				public:
					virtual void UpdateFrameSelf(GameTimeClockType::duration frameInterval, StateType currentState) { }
					virtual void FrameGraphicPrepare(const UIRenderer &renderer, UIVector2D offsetPosition, const IUIClipping &effectiveClipping) { }
				};

			public:
				using OnClickHandlerType = std::function<void(std::shared_ptr<SingleSimpleButton>)>;

			public:
				SingleSimpleButton(std::shared_ptr<UIManager> ownerManager, UICoordinateType touchWidth, UICoordinateType touchHeight,
					OnClickHandlerType onClick = ms_dummyOnClickHandler,
					std::shared_ptr<RenderingController> renderingController = nullptr);
				SingleSimpleButton(std::shared_ptr<UIManager> ownerManager, UICoordinateType touchWidth, UICoordinateType touchHeight, UICoordinateType touchCoverWidth, UICoordinateType touchCoverHeight,
					OnClickHandlerType onClick = ms_dummyOnClickHandler,
					std::shared_ptr<RenderingController> renderingController = nullptr);
				virtual ~SingleSimpleButton();

			protected:
				SingleSimpleButton(std::shared_ptr<UIManager> ownerManager);

			public:	// UIObject
				virtual std::shared_ptr<UIObject> Clone() const override;

				virtual void _UpdateFrameSelf(GameTimeClockType::duration frameInterval) override;
				virtual void FrameGraphicPrepare(const UIRenderer &renderer, UIVector2D offsetPosition, const IUIClipping &effectiveClipping) override;

			public:
				void SetOnClickHandler(OnClickHandlerType handler) { m_onClickHandlerFunc = handler; }
				void DiscardOnClickHandler() { m_onClickHandlerFunc = ms_dummyOnClickHandler; }

				void SetRenderingController(std::shared_ptr<RenderingController> controller);

				bool IsActiveState() const { return m_isActive; }
				void SetIsActive(bool isActive) { m_isActive = isActive; }

			public:
				void SetTouchWidth(UICoordinateType width);
				void SetTouchHeight(UICoordinateType height);

			protected:
				void _OnTouchDown(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time, bool isCoveredByDescendant);
				void _OnTouchIn(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time, bool isFirstIn, bool isCoveredByDescendant);

				void _OnTouchOut(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time);
				void _OnTouchCovered(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time);
				void _OnTouchCancel(std::shared_ptr<UITouch> touch, GameTimeClockType::time_point time);
				void _OnTouchUp(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time);

				void _OnPrimaryTouchLose(std::shared_ptr<UITouch> touch);

				void _SetTouchPressedState(bool isPressed);

			protected:
				UICoordinateType m_width;
				UICoordinateType m_height;

				UICoordinateType m_coverWidth;
				UICoordinateType m_coverHeight;

			protected:
				std::shared_ptr<UIObject>				m_touchVirtualObject;
				std::shared_ptr<RenderingController>	m_renderingController;

			protected:
				static OnClickHandlerType	ms_dummyOnClickHandler;
				OnClickHandlerType			m_onClickHandlerFunc;

				std::shared_ptr<UITouch> m_processingTouch;

				bool m_isPressed;
				bool m_isActive;
			};

			MAKE_ENUM_FLAG(SingleSimpleButton::StateType);
		}
	}
}

#endif
