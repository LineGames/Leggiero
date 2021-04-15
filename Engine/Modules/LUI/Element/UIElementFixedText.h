////////////////////////////////////////////////////////////////////////////////
// Element/UIElementFixedText.h (Leggiero/Modules - LegacyUI)
//
// Fixed Text Element
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__ELEMENT__UI_ELEMENT_FIXED_TEXT_H
#define __LM_LUI__ELEMENT__UI_ELEMENT_FIXED_TEXT_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <functional>
#include <memory>

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
			// Fixed UI Text
			class UIElementFixedText
				: public UIObject
			{
			public:
				using TypesettingGeneratorType = std::function<std::shared_ptr<Font::CachedGlyphTypesetting>()>;

			public:
				UIElementFixedText(std::shared_ptr<UIManager> ownerManager, TypesettingGeneratorType typesettingGeneratorFunc,
					bool isRenderShadow = false, const Graphics::GLByteARGB &shadowColor = Graphics::GLByteARGB::kTransparent, const UIVector2D &shadowOffset = UIVector2D::kZero);
				UIElementFixedText(std::shared_ptr<UIManager> ownerManager, std::shared_ptr<Font::CachedGlyphTypesetting> typesetting,
					bool isRenderShadow = false, const Graphics::GLByteARGB &shadowColor = Graphics::GLByteARGB::kTransparent, const UIVector2D &shadowOffset = UIVector2D::kZero);
				virtual ~UIElementFixedText();

			protected:
				UIElementFixedText(std::shared_ptr<UIManager> ownerManager)
					: UIObject(ownerManager)
				{ }

			public:
				void Prepare();

			public:	// IUIObject
				virtual std::shared_ptr<UIObject> Clone() const override;

				virtual UIElementSize CalculateSize() override;
				virtual UICoordinateType CalculateWidth() override;
				virtual UICoordinateType CalculateHeight() override;

				virtual void GraphicPrepare() override;

			public:
				void SetMultiplyColor(Graphics::GLByteARGB multiplyColor);
				const Graphics::GLByteARGB GetMultiplyColor() const;

				void TurnShadow(bool isTurnOn);
				void SetShadow(const Graphics::GLByteARGB &shadowColor, const UIVector2D &shadowOffset);

			protected:
				virtual std::shared_ptr<UIObject> _AllocateCloningObject() const override;

			protected:
				static TypesettingGeneratorType	ms_dummyTypesettingGenerator;
				TypesettingGeneratorType		m_typesettingGeneratorFunc;

				std::shared_ptr<Font::CachedGlyphTypesetting> m_typesetting;

				std::shared_ptr<Rendering::TextRenderingComponent>	m_textRenderingComponent;
			};
		}
	}
}

#endif
