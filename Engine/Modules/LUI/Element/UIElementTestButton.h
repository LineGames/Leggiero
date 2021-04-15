////////////////////////////////////////////////////////////////////////////////
// Element/UIElementTestButton.h (Leggiero/Modules - LegacyUI)
//
// UI Button for Testing
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__ELEMENT__UI_ELEMENT_TEST_BUTTON_H
#define __LM_LUI__ELEMENT__UI_ELEMENT_TEST_BUTTON_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <functional>
#include <memory>

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
			// Basic UI Button for Testing Purpose
			class UIElementTestButton
				: public UIObject
			{
			public:
				using OnClickHandlerType = std::function<void(std::shared_ptr<UIElementTestButton>)>;

			public:
				UIElementTestButton(std::shared_ptr<UIManager> ownerManager, UICoordinateType width, UICoordinateType height, Graphics::GLByteARGB color,
					UICoordinateType borderWidth = static_cast<UICoordinateType>(0.0f), Graphics::GLByteARGB borderColor = Graphics::GLByteARGB::kTransparent,
					std::shared_ptr<UIObject> buttonContent = nullptr, OnClickHandlerType onClick = ms_dummyOnClickHandler,
					bool isChangeColorOnPressed = false, Graphics::GLByteARGB colorOnPressed = Graphics::GLByteARGB::kTransparent, Graphics::GLByteARGB borderColorOnPressed = Graphics::GLByteARGB::kTransparent);
				virtual ~UIElementTestButton();

			protected:
				UIElementTestButton(std::shared_ptr<UIManager> ownerManager);

			public:
				virtual std::shared_ptr<UIObject> Clone() const override;

			public:
				void SetOnClickHandler(OnClickHandlerType handler) { m_onClickHandlerFunc = handler; }
				void DiscardOnClickHandler() { m_onClickHandlerFunc = ms_dummyOnClickHandler; }

			public:
				void SetIsClickedInvertGraphic(bool isInvert) { m_isClickGraphicInvert = isInvert; _UpdatePressedGraphic(); }

			protected:
				void _OnTouchDown(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time, bool isCoveredByDescendant);
				void _OnTouchIn(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time, bool isFirstIn, bool isCoveredByDescendant);

				void _OnTouchOut(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time);
				void _OnTouchCovered(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time);
				void _OnTouchCancel(std::shared_ptr<UITouch> touch, GameTimeClockType::time_point time);
				void _OnTouchUp(std::shared_ptr<UITouch> touch, UICoordinateType x, UICoordinateType y, GameTimeClockType::time_point time);

				void _OnPrimaryTouchLose(std::shared_ptr<UITouch> touch);

				void _SetTouchPressedState(bool isPressed);
				void _UpdatePressedGraphic();

			protected:
				virtual void OnEnableChanged(bool isEnable) override { _UpdatePressedGraphic(); }

			protected:
				UICoordinateType m_width;
				UICoordinateType m_height;

				Graphics::GLByteARGB	m_buttonColor;

				UICoordinateType		m_buttonBorderWidth;
				Graphics::GLByteARGB	m_buttonBorderColor;

				std::shared_ptr<UIObject>	m_buttonContent;

				bool					m_isChangeColorOnPressed;
				Graphics::GLByteARGB	m_colorOnPressed;
				Graphics::GLByteARGB	m_borderColorOnPressed;

			protected:
				std::shared_ptr<UIShapeRect>	m_buttonMainRectShape;
				std::shared_ptr<UIObject>		m_buttonMainObject;
				std::shared_ptr<UIObject>		m_touchVirtualObject;

			protected:
				static OnClickHandlerType	ms_dummyOnClickHandler;
				OnClickHandlerType			m_onClickHandlerFunc;

				std::shared_ptr<UITouch> m_processingTouch;

				bool m_isPressed;

			protected:
				bool m_isClickGraphicInvert;
			};
		}
	}
}

#endif
