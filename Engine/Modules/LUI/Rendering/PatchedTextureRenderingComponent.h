////////////////////////////////////////////////////////////////////////////////
// Rendering/PatchedTextureRenderingComponent.h (Leggiero/Modules - LegacyUI)
//
// Resizable Patched Texture Rendering Component
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__RENDERING__PATCHED_TEXTURE_RENDERING_COMPONENT_H
#define __LM_LUI__RENDERING__PATCHED_TEXTURE_RENDERING_COMPONENT_H


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
			// 3-Patch Texture Rendering Component
			class Patch3TextureRenderingComponent
				: public UIRenderingComponent
				, public IRectSizeSettable
			{
			public:
				Patch3TextureRenderingComponent(UICoordinateType width, UICoordinateType height, bool isVertical,
					std::shared_ptr<Graphics::GLTextureResource> startTexture, const Graphics::TextureRectSection &startTextureSection,
					std::shared_ptr<Graphics::GLTextureResource> middleTexture, const Graphics::TextureRectSection &middleTextureSection,
					std::shared_ptr<Graphics::GLTextureResource> finishTexture, const Graphics::TextureRectSection &finishTextureSection,
					Graphics::GLByteARGB color = Graphics::GLByteARGB::kWhite);
				virtual ~Patch3TextureRenderingComponent();

			public:	// IUIComponent
				virtual std::shared_ptr<IUIComponent> Clone(const UIObject &ownerObject) const override;

			public:	// UIRenderingComponent
				virtual void Render(const UIRenderer &renderer, IUITransform &effectiveTransform, const IUIClipping &effectiveClipping, float alpha) override;

				virtual UIElementSize GetVisualSize() override { return UIElementSize(m_width, m_height); }

			public:
				void SetSize(UICoordinateType width, UICoordinateType height);
				virtual void SetUIWidgetRectSize(UICoordinateType width, UICoordinateType height) override { SetSize(width, height); }

				void SetFlipState(size_t index, bool isFlipHorizontal, bool isFlipVertical);

				void SetColor(Graphics::GLByteARGB color) { m_color = color; }

			protected:
				void _FlushRenderingBuffer(const UIRenderer &renderer, size_t textureIndex);

			protected:
				std::shared_ptr<Graphics::GLTextureResource>	m_texture[3];
				Graphics::TextureRectSection					m_section[3];
				bool											m_isFlipHorizontal[3];
				bool											m_isFlipVertical[3];

				bool m_isVertical;
				UICoordinateType m_width;
				UICoordinateType m_height;

				Graphics::GLByteARGB m_color;

				std::vector<Graphics::TextureColoredVertex>	m_vertexBuffer;
				std::vector<GLushort>						m_indexBuffer;
			};


			// 9-Patch Texture Rendering Component
			class Patch9TextureRenderingComponent
				: public UIRenderingComponent
				, public IRectSizeSettable
			{
			public:
				Patch9TextureRenderingComponent(UICoordinateType width, UICoordinateType height,
					std::shared_ptr<Graphics::GLTextureResource> ltTexture, const Graphics::TextureRectSection &ltTextureSection,
					std::shared_ptr<Graphics::GLTextureResource> ctTexture, const Graphics::TextureRectSection &ctTextureSection,
					std::shared_ptr<Graphics::GLTextureResource> rtTexture, const Graphics::TextureRectSection &rtTextureSection,
					std::shared_ptr<Graphics::GLTextureResource> lmTexture, const Graphics::TextureRectSection &lmTextureSection,
					std::shared_ptr<Graphics::GLTextureResource> cmTexture, const Graphics::TextureRectSection &cmTextureSection,
					std::shared_ptr<Graphics::GLTextureResource> rmTexture, const Graphics::TextureRectSection &rmTextureSection,
					std::shared_ptr<Graphics::GLTextureResource> lbTexture, const Graphics::TextureRectSection &lbTextureSection,
					std::shared_ptr<Graphics::GLTextureResource> cbTexture, const Graphics::TextureRectSection &cbTextureSection,
					std::shared_ptr<Graphics::GLTextureResource> rbTexture, const Graphics::TextureRectSection &rbTextureSection,
					Graphics::GLByteARGB color = Graphics::GLByteARGB::kWhite,
					UICoordinateRatioType partScaleX = static_cast<UICoordinateRatioType>(1.0f), UICoordinateRatioType partScaleY = static_cast<UICoordinateRatioType>(1.0f));
				virtual ~Patch9TextureRenderingComponent();

			public:	// IUIComponent
				virtual std::shared_ptr<IUIComponent> Clone(const UIObject &ownerObject) const override;

			public:	// UIRenderingComponent
				virtual void Render(const UIRenderer &renderer, IUITransform &effectiveTransform, const IUIClipping &effectiveClipping, float alpha) override;

				virtual UIElementSize GetVisualSize() override { return UIElementSize(m_width, m_height); }

			public:
				enum class SubPatchIndexType : size_t
				{
					kLT = 0,
					kCT = 1,
					kRT = 2,
					kLM = 3,
					kCM = 4,
					kRM = 5,
					kLB = 6,
					kCB = 7,
					kRB = 8,
				};

			public:
				void SetSize(UICoordinateType width, UICoordinateType height);
				virtual void SetUIWidgetRectSize(UICoordinateType width, UICoordinateType height) override { SetSize(width, height); }

				void SetFlipState(SubPatchIndexType subPatch, bool isFlipHorizontal, bool isFlipVertical);

				void SetColor(Graphics::GLByteARGB color) { m_color = color; }

			protected:
				void _FlushRenderingBuffer(const UIRenderer &renderer, size_t textureIndex);

			protected:
				std::shared_ptr<Graphics::GLTextureResource>	m_texture[9];
				Graphics::TextureRectSection					m_section[9];
				bool											m_isFlipHorizontal[9];
				bool											m_isFlipVertical[9];
				// LT CT RT LM CM RM LB CB RB

				UICoordinateType m_width;
				UICoordinateType m_height;

				UICoordinateRatioType m_partScaleX;
				UICoordinateRatioType m_partScaleY;

				Graphics::GLByteARGB m_color;

				std::vector<Graphics::TextureColoredVertex>	m_vertexBuffer;
				std::vector<GLushort>						m_indexBuffer;
			};
		}
	}
}

#endif
