////////////////////////////////////////////////////////////////////////////////
// Element/UIElementVariantText.h (Leggiero/Modules - LegacyUI)
//
// Text UI element that can be changed
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__ELEMENT__UI_ELEMENT_VARIANT_TEXT_H
#define __LM_LUI__ELEMENT__UI_ELEMENT_VARIANT_TEXT_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <functional>
#include <memory>
#include <string>

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

		namespace Rendering
		{
			class TextRenderingComponent;
		}


		namespace Element
		{
			// Variant UI Text
			class UIElementVariantText
				: public UIObject
			{
			public:
				using TypesettingGeneratorType = std::function<std::shared_ptr<Font::CachedGlyphTypesetting>(GameDataString)>;

			public:
				UIElementVariantText(std::shared_ptr<UIManager> ownerManager, TypesettingGeneratorType typesettingGeneratorFunc, const GameDataString &initialString,
					bool isRenderShadow = false, const Graphics::GLByteARGB &shadowColor = Graphics::GLByteARGB::kTransparent, const UIVector2D &shadowOffset = UIVector2D::kZero);
				virtual ~UIElementVariantText();

			protected:
				UIElementVariantText(std::shared_ptr<UIManager> ownerManager)
					: UIObject(ownerManager)
				{ }

			public:
				void Prepare();

			public:	// IUIObject
				virtual std::shared_ptr<UIObject> Clone() const override;

				virtual void GraphicPrepare() override;
				virtual void FrameGraphicPrepare(const UIRenderer &renderer, UIVector2D offsetPosition, const IUIClipping &effectiveClipping) override;

				virtual UIElementSize CalculateSize() override;
				virtual UICoordinateType CalculateWidth() override;
				virtual UICoordinateType CalculateHeight() override;

			public:
				virtual void SetString(const GameDataString &stringToSet);

			public:
				void SetMultiplyColor(Graphics::GLByteARGB multiplyColor);
				const Graphics::GLByteARGB GetMultiplyColor() const;

				void TurnShadow(bool isTurnOn);
				void SetShadow(const Graphics::GLByteARGB &shadowColor, const UIVector2D &shadowOffset);

			protected:
				TypesettingGeneratorType		m_typesettingGeneratorFunc;

				std::shared_ptr<Font::CachedGlyphTypesetting>	m_typesetting;
				GameDataString									m_currentString;

				bool			m_isDirty;
				GameDataString	m_toChangeString;

				std::shared_ptr<Rendering::TextRenderingComponent>	m_textRenderingComponent;
			};
		}
	}
}

#endif
