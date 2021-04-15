////////////////////////////////////////////////////////////////////////////////
// Rendering/CachedTextRenderingComponent.h (Leggiero/Modules - LegacyUI)
//
// Rendering component to render cached characters text
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__RENDERING__CACHED_TEXT_RENDERING_COMPONENT_H
#define __LM_LUI__RENDERING__CACHED_TEXT_RENDERING_COMPONENT_H


// Standard Library
#include <memory>
#include <vector>

// External Library
#include <GLES3.h>

// Leggiero.Graphics
#include <Graphics/Common/GLColor.h>
#include <Graphics/Shader/CommonGLVertexType.h>

// Leggiero.Font
#include <Font/Common/CachedGlyph.h>

// Leggiero.LegacyUI
#include "UIRenderingComponent.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Rendering
		{
			// Cached Typesetted Text Rendering Component
			class CachedTextRenderingComponent
				: public UIRenderingComponent
			{
			public:
				// Struct represents each drawing character
				struct DrawingCharacter
				{
				public:
					std::shared_ptr<Font::CachedGlyph>	glyphInformation;
					UIVector2D							characterOrigin;
					Graphics::GLByteARGB				multiplyColor;
					UICoordinateType					advance;

					int pixelWidth;
					int pixelHeight;

					UIVector2D additionalOffset;

				public:
					DrawingCharacter(std::shared_ptr<Font::CachedGlyph> glyph,
						UIVector2D givenOrigin = UIVector2D::kZero, Graphics::GLByteARGB color = Graphics::GLByteARGB::kWhite);
				};

			public:
				CachedTextRenderingComponent(const std::vector<DrawingCharacter> &cachedText, UICoordinateType logicalHeight,
					bool isRenderShadow = false, const Graphics::GLByteARGB &shadowColor = Graphics::GLByteARGB::kTransparent, const UIVector2D &shadowOffset = UIVector2D::kZero,
					Graphics::GLByteARGB globalMultiplyColor = Graphics::GLByteARGB::kWhite);
				virtual ~CachedTextRenderingComponent();

			public:	// IUIComponent
				virtual std::shared_ptr<IUIComponent> Clone(const UIObject &ownerObject) const override;

			public:	// UIRenderingComponent
				virtual void Render(const UIRenderer &renderer, IUITransform &effectiveTransform, const IUIClipping &effectiveClipping, float alpha) override;

				virtual UIElementSize GetVisualSize() override;

			public:
				void SetText(const std::vector<DrawingCharacter> &cachedText, UICoordinateType logicalHeight);

				void SetGlobalMultiplyColor(Graphics::GLByteARGB multiplyColor) { m_globalMultiplyColor = multiplyColor; }
				const Graphics::GLByteARGB GetGlobalMultiplyColor() const { return m_globalMultiplyColor; }

				void TurnShadow(bool isTurnOn) { m_isRenderShadow = isTurnOn; }
				void SetShadow(const Graphics::GLByteARGB &shadowColor, const UIVector2D &shadowOffset) { m_shadowColor = shadowColor; m_shadowOffset = shadowOffset; }

				bool HasShadow() const { return m_isRenderShadow; }
				Graphics::GLByteARGB GetShadowColor() const { return m_shadowColor; }
				UIVector2D GetShadowOffset() const { return m_shadowOffset; }

				void SetLogicalHeight(UICoordinateType logicalHeight) { m_logicalHeight = logicalHeight; }
				UICoordinateType GetLogicalHeight() const { return m_logicalHeight; }

			protected:
				void _CalculateSize();

			protected:
				std::vector<DrawingCharacter>		m_cachedText;
				UICoordinateType					m_logicalHeight;

				std::vector<Graphics::TextureColoredVertex>	m_vertexBuffer;
				std::vector<GLushort>						m_indexBuffer;

				Graphics::GLByteARGB	m_globalMultiplyColor;

				bool					m_isRenderShadow;
				Graphics::GLByteARGB	m_shadowColor;
				UIVector2D				m_shadowOffset;

				UIElementSize			m_cahcedSize;
			};
		}
	}
}

#endif
