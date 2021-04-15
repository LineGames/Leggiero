////////////////////////////////////////////////////////////////////////////////
// Rendering/TextureRenderingComponent.h (Leggiero/Modules - LegacyUI)
//
// Rendering component to render textured rectangular image
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__RENDERING__TEXTURE_RENDERING_COMPONENT_H
#define __LM_LUI__RENDERING__TEXTURE_RENDERING_COMPONENT_H


// Standard Library
#include <memory>
#include <vector>

// External Library
#include <GLES3.h>

// Leggiero.Graphics
#include <Graphics/Common/GLColor.h>
#include <Graphics/Texture/TextureSection.h>
#include <Graphics/Shader/CommonGLVertexType.h>

// Leggiero.LegacyUI
#include "UIRenderingComponent.h"
#include "../Common/UISizeSettable.h"


namespace Leggiero
{
	// Forward Declaration
	namespace Graphics
	{
		class GLTextureResource;
	}


	namespace LUI
	{
		namespace Rendering
		{
			// Rectangular texture image rendering component
			class TextureRenderingComponent
				: public UIRenderingComponent
				, public IRectSizeSettable
			{
			public:
				TextureRenderingComponent(std::shared_ptr<Graphics::GLTextureResource> texture, Graphics::GLByteARGB color = Graphics::GLByteARGB::kWhite);
				TextureRenderingComponent(std::shared_ptr<Graphics::GLTextureResource> texture, const Graphics::TextureRectSection &section, Graphics::GLByteARGB color = Graphics::GLByteARGB::kWhite);
				TextureRenderingComponent(std::shared_ptr<Graphics::GLTextureResource> texture, const Graphics::TextureRectSection &section, UICoordinateType width, UICoordinateType height, Graphics::GLByteARGB color = Graphics::GLByteARGB::kWhite);
				virtual ~TextureRenderingComponent();

				// IUIComponent
			public:
				virtual std::shared_ptr<IUIComponent> Clone(const UIObject &ownerObject) const override;

				// UIRenderingComponent
			public:
				virtual void Render(const UIRenderer &renderer, IUITransform &effectiveTransform, const IUIClipping &effectiveClipping, float alpha) override;

				virtual UIElementSize GetVisualSize() override { return UIElementSize(m_width, m_height); }

			public:
				void SetTexture(std::shared_ptr<Graphics::GLTextureResource> texture);
				void SetTexture(std::shared_ptr<Graphics::GLTextureResource> texture, const Graphics::TextureRectSection &section);

				void SetSize(UICoordinateType width, UICoordinateType height);
				virtual void SetUIWidgetRectSize(UICoordinateType width, UICoordinateType height) override { SetSize(width, height); }
				void SetSizeRatio(UICoordinateRatioType widthRatio, UICoordinateRatioType heightRatio);

				void SetColor(Graphics::GLByteARGB color) { m_color = color; }

				void SetFlip(bool isFlipHorizontal, bool isFlipVertical) { m_isFlipHorizontal = isFlipHorizontal; m_isFlipVertical = isFlipVertical; }

			protected:
				std::shared_ptr<Graphics::GLTextureResource>	m_texture;
				Graphics::TextureRectSection					m_section;

				UICoordinateType m_width;
				UICoordinateType m_height;

				bool m_isFlipHorizontal;
				bool m_isFlipVertical;

				Graphics::GLByteARGB m_color;

				std::vector<Graphics::TextureColoredVertex>	m_vertexBuffer;
				std::vector<GLushort>						m_indexBuffer;
			};


			// Texture rendering component with gaussian blur effect
			class BlurredTextureRenderingComponent
				: public TextureRenderingComponent
			{
			public:
				BlurredTextureRenderingComponent(std::shared_ptr<Graphics::GLTextureResource> texture, Graphics::GLByteARGB color = Graphics::GLByteARGB::kWhite);
				BlurredTextureRenderingComponent(std::shared_ptr<Graphics::GLTextureResource> texture, const Graphics::TextureRectSection &section, Graphics::GLByteARGB color = Graphics::GLByteARGB::kWhite);
				BlurredTextureRenderingComponent(std::shared_ptr<Graphics::GLTextureResource> texture, const Graphics::TextureRectSection &section, UICoordinateType width, UICoordinateType height, Graphics::GLByteARGB color = Graphics::GLByteARGB::kWhite);
				BlurredTextureRenderingComponent(std::shared_ptr<Graphics::GLTextureResource> texture, const Graphics::TextureRectSection &section, UICoordinateType width, UICoordinateType height, float widthBlurRatio, float heightBlurRatio, Graphics::GLByteARGB color = Graphics::GLByteARGB::kWhite);
				virtual ~BlurredTextureRenderingComponent();

			public:	// IUIComponent
				virtual std::shared_ptr<IUIComponent> Clone(const UIObject &ownerObject) const override;

				
			public:	// UIRenderingComponent
				virtual void Render(const UIRenderer &renderer, IUITransform &effectiveTransform, const IUIClipping &effectiveClipping, float alpha) override;

			public:
				void SetLinearBlurMultiplier(float widthRatio, float heightRatio);

			protected:
				float m_widthTexelUVRatio;
				float m_heightTexelUVRatio;
			};
		}
	}
}

#endif
