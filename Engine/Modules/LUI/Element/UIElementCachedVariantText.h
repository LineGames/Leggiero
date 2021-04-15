////////////////////////////////////////////////////////////////////////////////
// Element/UIElementCachedVariantText.h (Leggiero/Modules - LegacyUI)
//
// Changable, but optimized using caching UI Element for Text
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__ELEMENT__UI_ELEMENT_CACHED_VARIANT_TEXT_H
#define __LM_LUI__ELEMENT__UI_ELEMENT_CACHED_VARIANT_TEXT_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Leggiero.Graphics
#include <Graphics/Common/GLColor.h>

// Leggiero.Font
#include <Font/Common/CachedGlyph.h>

// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"
#include "../UIObject.h"
#include "../Rendering/CachedTextRenderingComponent.h"


// Forward Declarations
namespace Leggiero
{
	namespace Graphic
	{
		class GraphicFontManagerComponent;

		namespace FreeType
		{
			class MultiPageFontSetting;
		}
	}
}


namespace Leggiero
{
	// Forward Declarations
	namespace Font
	{
		class GlyphManagerComponent;
		class MultiPageFontSetting;
	}

	namespace LUI
	{
		// Forward Declaration
		class UIManager;


		namespace Element
		{
			// Changable UI Text
			class UIElementCachedVariantText
				: public UIObject
			{
			public:
				using GlyphGeneratorType = std::function<Rendering::CachedTextRenderingComponent::DrawingCharacter(Font::GlyphCodePointType)>;

				static GlyphGeneratorType GlyphGeneratorFuncGen(Font::GlyphManagerComponent *fontManager,
					std::shared_ptr<Font::MultiPageFontSetting> fontSettting, Font::GlyphSizeType size,
					Graphics::GLByteARGB color = Graphics::GLByteARGB::kWhite, float strokeWidth = 0.0f, Graphics::GLByteARGB strokeColor = Graphics::GLByteARGB::kTransparent,
					bool isFitGrid = true, Font::TextureType textureType = Font::TextureType::kNormal);

			public:
				UIElementCachedVariantText(std::shared_ptr<UIManager> ownerManager, GlyphGeneratorType glyphGeneratorFunc, UICoordinateType logicalHeight, const GameDataString &initialString,
					bool isRenderShadow = false, const Graphics::GLByteARGB &shadowColor = Graphics::GLByteARGB::kTransparent, const UIVector2D &shadowOffset = UIVector2D::kZero,
					const Graphics::GLByteARGB &multiplyColor = Graphics::GLByteARGB::kWhite,
					const GameDataString &initialCachingCharacters = "0123456789.,+-");
				virtual ~UIElementCachedVariantText();

			protected:
				UIElementCachedVariantText(std::shared_ptr<UIManager> ownerManager)
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
				virtual void SetString(GameDataString &&stringToSet);

			public:
				void SetMultiplyColor(Graphics::GLByteARGB multiplyColor);
				const Graphics::GLByteARGB GetMultiplyColor() const;

				void SetLogicalHeight(UICoordinateType logicalHeight);
				UICoordinateType GetLogicalHeight() const;

				void TurnShadow(bool isTurnOn);
				void SetShadow(const Graphics::GLByteARGB &shadowColor, const UIVector2D &shadowOffset);

			protected:
				void _ResolveCacheRegister();
				void _DoTypesetting();

			protected:
				GlyphGeneratorType		m_glyphGeneratorFunc;

				std::vector<Rendering::CachedTextRenderingComponent::DrawingCharacter>	m_cachedTextDrawing;
				GameDataString												m_currentString;

				bool						m_isDirty;
				GameDataString	m_toChangeString;

				std::shared_ptr<Rendering::CachedTextRenderingComponent>	m_textRenderingComponent;

				std::vector<Font::GlyphCodePointType>																	m_cacheWaitQueue;
				std::unordered_map<Font::GlyphCodePointType, Rendering::CachedTextRenderingComponent::DrawingCharacter> m_glyphCache;
			};
		}
	}
}

#endif
