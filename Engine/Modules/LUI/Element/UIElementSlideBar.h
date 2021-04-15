////////////////////////////////////////////////////////////////////////////////
// Element/UIElementSlideBar.h (Leggiero/Modules - LegacyUI)
//
// Slide bar UI element
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__ELEMENT__UI_ELEMENT_SLIDE_BAR_H
#define __LM_LUI__ELEMENT__UI_ELEMENT_SLIDE_BAR_H


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
			// Adjustable Slider
			class UIElementSlideBar
				: public UIObject
			{
			public:
				using OnValueChangedHandlerType = std::function<void(std::shared_ptr<UIElementSlideBar>, double)>;

			public:
				UIElementSlideBar(std::shared_ptr<UIManager> ownerManager, UICoordinateType width, UICoordinateType height, UICoordinateType barLineWidth, Graphics::GLByteARGB barColor,
					UICoordinateType markerSize, UICoordinateType markerLineWidth, Graphics::GLByteARGB markerColor,
					OnValueChangedHandlerType onValueChanged = ms_dummyOnValueChangedHandler, int subRulerDivide = 0, double initialValue = 0.0,
					bool isChangeMarkerOnPressed = false, UICoordinateType markerSizeOnPressed = static_cast<UICoordinateType>(0.0f), UICoordinateType markerLineWidthOnPressed = static_cast<UICoordinateType>(0.0f), Graphics::GLByteARGB markerColorOnPressed = Graphics::GLByteARGB::kTransparent);
				virtual ~UIElementSlideBar();

			protected:
				UIElementSlideBar(std::shared_ptr<UIManager> ownerManager);

			public:
				virtual std::shared_ptr<UIObject> Clone() const override;

			public:
				void SetOnValueChangedHandler(OnValueChangedHandlerType handler) { m_onValueChangedHandlerFunc = handler; }
				void DiscardOnValueChangedHandler() { m_onValueChangedHandlerFunc = ms_dummyOnValueChangedHandler; }

			public:
				void SetBarColor(Graphics::GLByteARGB barColor);
				void SetMarkerColor(Graphics::GLByteARGB markerColor, Graphics::GLByteARGB markerColorOnPressed = Graphics::GLByteARGB::kTransparent);

			public:
				double GetValue() const { return m_currentValue; }
				void ForceSetValue(double value);

			protected:
				void _OnTouchDown(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time, bool isCoveredByDescendant);

				void _OnTouchMove(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time);

				void _OnTouchCancel(std::shared_ptr<UITouch> touch, GameTimeClockType::time_point time);
				void _OnTouchUp(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time);

				void _OnPrimaryTouchLose(std::shared_ptr<UITouch> touch);

				void _SetTouchPressedState(bool isPressed);
				void _UpdatePressedGraphic();

				void _UpdateMarkerPosition();
				void _UpdateTouchedValue();

			protected:
				virtual void OnEnableChanged(bool isEnable) override { _UpdatePressedGraphic(); }

			protected:
				UICoordinateType m_width;
				UICoordinateType m_height;
				UICoordinateType m_barLineWidth;
				Graphics::GLByteARGB	m_barColor;

				UICoordinateType m_markerSize;
				UICoordinateType m_markerLineWidth;
				Graphics::GLByteARGB	m_markerColor;

				bool				m_isChangeMarkerOnPressed;
				UICoordinateType	m_markerSizeOnPressed;
				UICoordinateType	m_markerLineWidthOnPressed;
				Graphics::GLByteARGB	m_markerColorOnPressed;

				int m_subRulerDivide;

			protected:
				std::vector<std::shared_ptr<UIShapeRect> >	m_barShapes;
				std::vector<std::shared_ptr<UIShapeRect> >	m_markerShapes;
				std::vector<std::shared_ptr<UIShapeRect> >	m_pressedMarkerShapes;
				std::shared_ptr<UIObject>		m_markerMainObject;
				std::shared_ptr<UIObject>		m_pressedMarkerMainObject;
				std::shared_ptr<UIObject>		m_touchVirtualObject;

			protected:
				static OnValueChangedHandlerType	ms_dummyOnValueChangedHandler;
				OnValueChangedHandlerType			m_onValueChangedHandlerFunc;

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
