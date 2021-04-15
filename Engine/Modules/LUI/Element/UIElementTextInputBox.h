////////////////////////////////////////////////////////////////////////////////
// Element/UIElementTextInputBox.h (Leggiero/Modules - LegacyUI)
//
// UI Element for Text Input
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__ELEMENT__UI_ELEMENT_TEXT_INPUT_BOX_H
#define __LM_LUI__ELEMENT__UI_ELEMENT_TEXT_INPUT_BOX_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <functional>
#include <memory>

// Leggiero.Utility
#include <Utility/Sugar/EnumClass.h>

// Leggiero.Graphics
#include <Graphics/Common/GLColor.h>

// Leggiero.Font
#include <Font/Common/Typesetting.h>

// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"
#include "../UIObject.h"


namespace Leggiero
{
	namespace LUI
	{
		// Forward Declarations
		class UIManager;
		class UITouch;
		class ValuedTouchComponent;
		class UIRectangularClipping;

		namespace Rendering
		{
			class TextRenderingComponent;
		}


		namespace Element
		{
			// UI Text Input Box
			class UIElementTextInputBox
				: public UIObject
			{
			public:
				enum class StateType
				{
					kNormal = 0x0,

					kPushed = 0x1,
					kDisabled = 0x4,
				};

			public:
				// Rendering Controller to deal with Interaction
				class RenderingController
				{
				public:
					virtual void UpdateFrameSelf(GameTimeClockType::duration frameInterval, StateType currentState, const GameDataString &currentTextValue) { }
					virtual void FrameGraphicPrepare(const UIRenderer &renderer, UIVector2D offsetPosition, const IUIClipping &effectiveClipping) { }
				};

			public:
				// Text UI Object of Text Input Box
				class TextUIObject
					: public UIObject
				{
					friend class Element::UIElementTextInputBox;

				public:
					TextUIObject(std::shared_ptr<UIManager> ownerManager, std::shared_ptr<Rendering::TextRenderingComponent> renderingComponent);
					virtual ~TextUIObject();

				public:	// UIObject
					virtual std::shared_ptr<UIObject> Clone() const override;

					virtual UIElementSize CalculateSize() override;
					virtual UICoordinateType CalculateWidth() override;
					virtual UICoordinateType CalculateHeight() override;

				protected:
					std::shared_ptr<Font::CachedGlyphTypesetting> m_effectiveTypesetting;
				};

			public:
				using TypesettingGeneratorType = std::function<std::shared_ptr<Font::CachedGlyphTypesetting>(GameDataString)>;

				using EventHandlerType = std::function<void(std::shared_ptr<UIElementTextInputBox>, GameDataString)>;
				using TextInputFilterType = std::function<GameDataString(std::shared_ptr<UIElementTextInputBox>, GameDataString)>;
				using TextInputValidatorType = std::function<bool(std::shared_ptr<UIElementTextInputBox>, GameDataString)>;

			public:
				UIElementTextInputBox(std::shared_ptr<UIManager> ownerManager, UICoordinateType width, UICoordinateType height,
					TypesettingGeneratorType typesettingGeneratorFunc, Graphics::GLByteARGB textMultiplyColor = Graphics::GLByteARGB::kWhite, const GameDataString &initialString = u8"",
					const GameDataString &dialogTitle = u8"", const GameDataString &dialogMessage = u8"", const GameDataString &dialogAcceptButton = u8"OK", const GameDataString &dialogCancelButton = u8"Cancel",
					EventHandlerType onTextInput = ms_dummyEventHandler, EventHandlerType onValidationFailed = ms_dummyEventHandler,
					TextInputFilterType textInputFilter = ms_dummyFilter, TextInputValidatorType textInputValidator = ms_dummyValidator,
					std::shared_ptr<RenderingController> renderingController = nullptr);
				UIElementTextInputBox(std::shared_ptr<UIManager> ownerManager, UICoordinateType width, UICoordinateType height,
					UICoordinateType clippingRectLeft, UICoordinateType clippingRectTop, UICoordinateType clippingRectWidth, UICoordinateType clippingRectHeight,
					TypesettingGeneratorType typesettingGeneratorFunc, Graphics::GLByteARGB textMultiplyColor = Graphics::GLByteARGB::kWhite, const GameDataString &initialString = u8"",
					const GameDataString &dialogTitle = u8"", const GameDataString &dialogMessage = u8"", const GameDataString &dialogAcceptButton = u8"OK", const GameDataString &dialogCancelButton = u8"Cancel",
					EventHandlerType onTextInput = ms_dummyEventHandler, EventHandlerType onValidationFailed = ms_dummyEventHandler,
					TextInputFilterType textInputFilter = ms_dummyFilter, TextInputValidatorType textInputValidator = ms_dummyValidator,
					std::shared_ptr<RenderingController> renderingController = nullptr);
				virtual ~UIElementTextInputBox();

			protected:
				UIElementTextInputBox(std::shared_ptr<UIManager> ownerManager);

			public:	// UIObject
				virtual std::shared_ptr<UIObject> Clone() const override;

				virtual void GraphicPrepare() override;

				virtual void _UpdateFrameSelf(GameTimeClockType::duration frameInterval) override;
				virtual void FrameGraphicPrepare(const UIRenderer &renderer, UIVector2D offsetPosition, const IUIClipping &effectiveClipping) override;

			public:
				void SetOnTextInputHandler(EventHandlerType handler) { m_onTextInputHandlerFunc = handler; }
				void DiscardOnTextInputHandler() { m_onTextInputHandlerFunc = ms_dummyEventHandler; }

				void SetOnValidationFailedHandler(EventHandlerType handler) { m_onTextInputValidationFailedHandlerFunc = handler; }
				void DiscardOnValidationFailedHandler() { m_onTextInputValidationFailedHandlerFunc = ms_dummyEventHandler; }

				void SetTextInputFilter(TextInputFilterType filter) { m_inputFilter = filter; }
				void DiscardTextInputFilter() { m_inputFilter = ms_dummyFilter; }

				void SetTextInputValidator(TextInputValidatorType validator) { m_inputValidator = validator; }
				void DiscardTextInputValidator() { m_inputValidator = ms_dummyValidator; }

			public:
				void SetRenderingController(std::shared_ptr<RenderingController> controller);

			public:
				void SetInputDialogTitleText(const GameDataString &title) { m_inputDialogTitleText = title; }
				void SetInputDialogMessageText(const GameDataString &message) { m_inputDialogMessageText = message; }
				void SetInputDialogAcceptButtonText(const GameDataString &buttonText) { m_inputDialogAcceptButtonText = buttonText; }
				void SetInputDialogCancelButtonText(const GameDataString &buttonText) { m_inputDialogCancelButtonText = buttonText; }

			public:
				GameDataString GetCurrentValue() { return m_currentStringValue; }

				void SetString(const GameDataString &stringToSet);

				void SetTextMultiplyColor(Graphics::GLByteARGB multiplyColor);
				const Graphics::GLByteARGB GetTextMultiplyColor() const;

				std::shared_ptr<TextUIObject> GetTextSubUIObject() { return m_textSubObject; }

			public:
				void SetWidth(UICoordinateType width);
				void SetHeight(UICoordinateType height);

				void SetTouchWidthRatio(UICoordinateRatioType ratio);
				void SetTouchHeightRatio(UICoordinateRatioType ratio);

				std::shared_ptr<UIRectangularClipping> GetRawTextClippingComponent(bool isProhibitClippingBySize = true);

				void InitializeAdjustClippingLayoutAsBox();

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
				void _OnTextInputFinished(bool isSuccessed, GameDataString inputText);

			protected:
				UICoordinateType m_width;
				UICoordinateType m_height;

				UICoordinateRatioType m_touchWidthRatio;
				UICoordinateRatioType m_touchHeightRatio;

			protected:
				std::shared_ptr<UIObject>				m_touchVirtualObject;
				std::shared_ptr<RenderingController>	m_renderingController;

			protected:
				static EventHandlerType		ms_dummyEventHandler;

				EventHandlerType	m_onTextInputHandlerFunc;
				EventHandlerType	m_onTextInputValidationFailedHandlerFunc;

				static TextInputFilterType		ms_dummyFilter;
				TextInputFilterType				m_inputFilter;

				static TextInputValidatorType	ms_dummyValidator;
				TextInputValidatorType			m_inputValidator;

			protected:
				std::shared_ptr<UITouch> m_processingTouch;

				bool m_isPressed;

			protected:
				GameDataString m_currentStringValue;

				GameDataString m_inputDialogTitleText;
				GameDataString m_inputDialogMessageText;
				GameDataString m_inputDialogAcceptButtonText;
				GameDataString m_inputDialogCancelButtonText;

			protected:
				TypesettingGeneratorType						m_typesettingGeneratorFunc;

				std::shared_ptr<Font::CachedGlyphTypesetting>	m_typesetting;
				GameDataString									m_currentString;

				bool										m_isDirty;

				std::shared_ptr<TextUIObject>						m_textSubObject;
				std::shared_ptr<Rendering::TextRenderingComponent>	m_textRenderingComponent;

			protected:
				bool										m_isClippingBySize;
				std::shared_ptr<UIObject>					m_textClippingSubObject;
				std::shared_ptr<UIRectangularClipping>		m_textClippingRectComponent;
			};

			MAKE_ENUM_FLAG(UIElementTextInputBox::StateType);
		}
	}
}

#endif
