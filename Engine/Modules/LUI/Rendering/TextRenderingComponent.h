////////////////////////////////////////////////////////////////////////////////
// Rendering/TextRenderingComponent.h (Leggiero/Modules - LegacyUI)
//
// Rendering component to render typesetted text
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__RENDERING__TEXT_RENDERING_COMPONENT_H
#define __LM_LUI__RENDERING__TEXT_RENDERING_COMPONENT_H


// Standard Library
#include <memory>
#include <vector>

// External Library
#include <GLES3.h>

// Leggiero.Graphics
#include <Graphics/Common/GLColor.h>
#include <Graphics/Shader/CommonGLVertexType.h>

// Leggiero.Font
#include <Font/Common/Typesetting.h>

// Leggiero.LegacyUI
#include "UIRenderingComponent.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Rendering
		{
			// Typesetted Text Rendering Component
			class TextRenderingComponent
				: public UIRenderingComponent
			{
			public:
				TextRenderingComponent(std::shared_ptr<Font::CachedGlyphTypesetting> typesetting,
					bool isRenderShadow = false, const Graphics::GLByteARGB &shadowColor = Graphics::GLByteARGB::kTransparent, const UIVector2D &shadowOffset = UIVector2D::kZero,
					Graphics::GLByteARGB globalMultiplyColor = Graphics::GLByteARGB::kWhite);
				virtual ~TextRenderingComponent();

			public:	// IUIComponent
				virtual std::shared_ptr<IUIComponent> Clone(const UIObject &ownerObject) const override;

			public:	// UIRenderingComponent
				virtual void Render(const UIRenderer &renderer, IUITransform &effectiveTransform, const IUIClipping &effectiveClipping, float alpha) override;

				virtual UIElementSize GetVisualSize() override;

			public:
				void SetTypesetting(std::shared_ptr<Font::CachedGlyphTypesetting> typesetting);

				void SetGlobalMultiplyColor(Graphics::GLByteARGB multiplyColor) { m_globalMultiplyColor = multiplyColor; }
				const Graphics::GLByteARGB GetGlobalMultiplyColor() const { return m_globalMultiplyColor; }

				void TurnShadow(bool isTurnOn) { m_isRenderShadow = isTurnOn; }
				void SetShadow(const Graphics::GLByteARGB &shadowColor, const UIVector2D &shadowOffset) { m_shadowColor = shadowColor; m_shadowOffset = shadowOffset; }

			protected:
				std::shared_ptr<Font::CachedGlyphTypesetting> m_typesetting;

				std::vector<Graphics::TextureColoredVertex>	m_vertexBuffer;
				std::vector<GLushort>						m_indexBuffer;

				Graphics::GLByteARGB	m_globalMultiplyColor;

				bool					m_isRenderShadow;
				Graphics::GLByteARGB	m_shadowColor;
				UIVector2D				m_shadowOffset;
			};
		}
	}
}

#endif
