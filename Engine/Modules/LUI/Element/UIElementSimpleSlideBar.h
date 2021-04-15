////////////////////////////////////////////////////////////////////////////////
// Element/UIElementSimpleSliderBar.h (Leggiero/Modules - LegacyUI)
//
// Simple slide bar element with given handle
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__ELEMENT__UI_ELEMENT_SIMPLE_SLIDE_BAR_H
#define __LM_LUI__ELEMENT__UI_ELEMENT_SIMPLE_SLIDE_BAR_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <functional>
#include <memory>
#include <vector>

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
		class UIShapeRect;
		class ValuedTouchComponent;
		class UITouch;


		namespace Element
		{
			// Adjustable Slider Element
			class UIElementSimpleSlideBar
				: public UIObject
			{
			public:
				// Rendering Controller Definition for interaction
				class RenderingController
				{
				public:
					virtual void UpdateFrameSelf(GameTimeClockType::duration frameInterval, bool isUsing) { }
					virtual void FrameGraphicPrepare(const UIRenderer &renderer, UIVector2D offsetPosition, const IUIClipping &effectiveClipping) { }
				};

			public:
				using OnValueChangedHandlerType = std::function<void(std::shared_ptr<UIElementSimpleSlideBar>, double)>;
				using OnTouchStateChangedHandlerType = std::function<void(std::shared_ptr<UIElementSimpleSlideBar>, bool)>;

			public:
				UIElementSimpleSlideBar(std::shared_ptr<UIManager> ownerManager, UICoordinateType width,
					std::shared_ptr<UIObject> marker, UICoordinateRatioType markerInteractionWidthRatio = static_cast<UICoordinateRatioType>(1.0f), UICoordinateRatioType markerInteractionHeightRatio = static_cast<UICoordinateRatioType>(1.0f),
					int subRulerDivide = 0, double initialValue = 0.0, OnValueChangedHandlerType onValueChanged = ms_dummyOnValueChangedHandler,
					std::shared_ptr<RenderingController> renderingController = nullptr);
				virtual ~UIElementSimpleSlideBar();

			protected:
				UIElementSimpleSlideBar(std::shared_ptr<UIManager> ownerManager);

			public:
				virtual std::shared_ptr<UIObject> Clone() const override;

				virtual void _UpdateFrameSelf(GameTimeClockType::duration frameInterval) override;
				virtual void FrameGraphicPrepare(const UIRenderer &renderer, UIVector2D offsetPosition, const IUIClipping &effectiveClipping) override;

			public:
				void SetOnValueChangedHandler(OnValueChangedHandlerType handler) { m_onValueChangedHandlerFunc = handler; }
				void DiscardOnValueChangedHandler() { m_onValueChangedHandlerFunc = ms_dummyOnValueChangedHandler; }

				void SetOnTouchStateChangedHandler(OnTouchStateChangedHandlerType handler) { m_onTouchStateChangedHandlerFunc = handler; }
				void DiscardOnTouchStateChangedHandler() { m_onTouchStateChangedHandlerFunc = ms_dummyOnTouchStateChangedHandler; }

				void SetRenderingController(std::shared_ptr<RenderingController> controller);

			public:
				double GetValue() const { return m_currentValue; }
				void ForceSetValue(double value, bool isSuppressNotify = false, bool isForceNotify = false);

				int GetSubRulerDivide() const { return m_subRulerDivide; }
				void SetSubRulerDivide(int subRulerDevide, bool isSuppressNotify = true);

				std::shared_ptr<UIObject> GetMarkerObject() const { return m_markerObject; }

				bool IsHoldPrimaryOnControlling() const { return m_isHoldPrimaryOnControlling; }
				void SetIsHoldPrimaryOnControlling(bool isHoldPrimaryOnControlling) { m_isHoldPrimaryOnControlling = isHoldPrimaryOnControlling; }

				UICoordinateType GetScrollingWidth() const { return m_width; }

			protected:
				void _OnTouchDown(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time, bool isCoveredByDescendant);

				void _OnTouchMove(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time);

				void _OnTouchCancel(std::shared_ptr<UITouch> touch, GameTimeClockType::time_point time);
				void _OnTouchUp(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time);

				bool _OnPrimaryTouchSteal(std::shared_ptr<UITouch> touch, std::shared_ptr<UIObject> stealer);
				void _OnPrimaryTouchLose(std::shared_ptr<UITouch> touch);

				void _SetTouchPressedState(bool isPressed);

				void _UpdateMarkerPosition();
				void _UpdateTouchedValue();

			protected:
				UICoordinateType m_width;

				UICoordinateRatioType m_markerInteractionWidthRatio;
				UICoordinateRatioType m_markerInteractionHeightRatio;

				int m_subRulerDivide;

				bool m_isHoldPrimaryOnControlling;

			protected:
				std::shared_ptr<UIObject>				m_markerObject;
				std::shared_ptr<UIObject>				m_touchVirtualObject;
				std::shared_ptr<RenderingController>	m_renderingController;

			protected:
				static OnValueChangedHandlerType	ms_dummyOnValueChangedHandler;
				OnValueChangedHandlerType			m_onValueChangedHandlerFunc;

				static OnTouchStateChangedHandlerType	ms_dummyOnTouchStateChangedHandler;
				OnTouchStateChangedHandlerType			m_onTouchStateChangedHandlerFunc;

				std::shared_ptr<UITouch> m_processingTouch;

				bool m_isPressed;

			protected:
				double m_currentValue;
				double m_touchStartValue;
			};
		}
	}
}

#endif
