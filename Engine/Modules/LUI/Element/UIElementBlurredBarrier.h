////////////////////////////////////////////////////////////////////////////////
// Element/UIElementBlurredBarrier.h (Leggiero/Modules - LegacyUI)
//
// UI Barrier with blurred background effect
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__ELEMENT__UI_ELEMENT_BLURRED_BARRIER_H
#define __LM_LUI__ELEMENT__UI_ELEMENT_BLURRED_BARRIER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <memory>

// Leggiero.Graphics
#include <Graphics/Common/GLColor.h>

// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"
#include "../UIObject.h"


namespace Leggiero
{
	// Forward Declarations
	namespace Graphics
	{
		class GraphicResourceManagerComponent;
		class DynamicTextureResource;
		class RenderTargetTextureResource;
		class TextureRectSection;
	}


	namespace LUI
	{
		// Forward Declarations
		class UIManager;
		class UIShapeRect;

		namespace Shaders
		{
			class UITextureColorShader;
			class UITextureBlurShader;
		}

		namespace Rendering
		{
			class TextureRenderingComponent;
		}


		namespace Element
		{
			// UI Barrier
			// if not in dynamic mode, animation changes can introduce un-expected effect
			class UIElementBlurredBarrier
				: public UIObject
			{
			public:
				UIElementBlurredBarrier(std::shared_ptr<UIManager> ownerManager, Graphics::GraphicResourceManagerComponent *pGraphicResourceManager, Graphics::GLByteARGB alternativeColor = Graphics::GLByteARGB::kBlack,
					bool isSkipDefaultBlur = false, int blurIterations = 0, std::shared_ptr<Shaders::UITextureBlurShader> moreBlurShader = nullptr, float downsampleRatio = 1.0f, std::shared_ptr<Shaders::UITextureColorShader> downsampleShader = nullptr);
				UIElementBlurredBarrier(std::shared_ptr<UIManager> ownerManager, Graphics::GraphicResourceManagerComponent *pGraphicResourceManager, UICoordinateType maxWidth, UICoordinateType maxHeight, Graphics::GLByteARGB alternativeColor = Graphics::GLByteARGB::kBlack,
					bool isSkipDefaultBlur = false, int blurIterations = 0, std::shared_ptr<Shaders::UITextureBlurShader> moreBlurShader = nullptr, float downsampleRatio = 1.0f, std::shared_ptr<Shaders::UITextureColorShader> downsampleShader = nullptr);
				virtual ~UIElementBlurredBarrier();

			public:
				void Prepare();

			public:	// IUIObject
				virtual std::shared_ptr<UIObject> Clone() const override;

				virtual void GraphicPrepare() override;
				virtual void FrameGraphicPrepare(const UIRenderer &renderer, UIVector2D offsetPosition, const IUIClipping &effectiveClipping) override;

				virtual UIElementSize CalculateSize() override { return UIElementSize(GetWidth(), GetHeight()); }
				virtual UICoordinateType CalculateWidth() override { return GetWidth(); }
				virtual UICoordinateType CalculateHeight() override { return GetHeight(); }

			public:
				void SetSize(UICoordinateType width, UICoordinateType height) { m_width = width; m_height = height; }
				void SetWidth(UICoordinateType width) { m_width = width; }
				void SetHeight(UICoordinateType height) { m_height = height; }

				UICoordinateType GetWidth() const { return m_width; }
				UICoordinateType GetHeight() const { return m_height; }

				void SetColor(Graphics::GLByteARGB color);

				void SetIsDynamic(bool isDynamic) { m_isDynamic = isDynamic; }
				void MakeDirty() { m_isDirty = true; }

				void ChangeMoreBlurIteration(int iterations);

			protected:
				virtual std::shared_ptr<UIObject> _AllocateCloningObject() const override;

			protected:
				bool _PrepareTexture();
				bool _BakeProcessedTexture();

				static void _OnTextureRestore(std::weak_ptr<UIElementBlurredBarrier> weakThis, std::shared_ptr<Graphics::DynamicTextureResource> texture);

			protected:
				UICoordinateType m_width;
				UICoordinateType m_height;

				Graphics::GraphicResourceManagerComponent *m_pGraphicResourceManager;

				std::shared_ptr<Rendering::TextureRenderingComponent>	m_renderingComponent;
				std::shared_ptr<Graphics::DynamicTextureResource>		m_texture;
				std::shared_ptr<Graphics::TextureRectSection>	m_textureSection;
				UICoordinateType m_textureSampledWidth;
				UICoordinateType m_textureSampledHeight;

				UICoordinateType m_requiredWidth;
				UICoordinateType m_requiredHeight;

				bool m_isGraphicPrepareTried;

				bool m_isEnableTextureCapture;
				Graphics::GLByteARGB	m_alternativeColor;
				std::shared_ptr<UIShapeRect>	m_alternativeRectShape;

				bool m_isSkipDefaultBlur;

				bool m_isDynamic;
				bool m_isDirty;
				bool m_isBlurIterationChanged;

				bool	m_isDownsampling;
				float	m_downsampleRatio;
				std::shared_ptr<Graphics::RenderTargetTextureResource>	m_downSampledTexture;
				std::shared_ptr<Shaders::UITextureColorShader>			m_downsampleShader;

				bool	m_isMoreBlurEnabled;
				int		m_blurIterations;
				std::shared_ptr<Graphics::RenderTargetTextureResource>	m_blurIterationTwoPassTexture;
				std::shared_ptr<Shaders::UITextureBlurShader>			m_moreBlurShader;
			};
		}
	}
}

#endif
