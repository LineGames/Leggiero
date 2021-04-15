////////////////////////////////////////////////////////////////////////////////
// Rendering/UIRenderer.h (Leggiero/Modules - LegacyUI)
//
// UI Renderer
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__RENDERING__UI_RENDERER_H
#define __LM_LUI__RENDERING__UI_RENDERER_H


// Standard Library
#include <memory>

// External Library
#include <glm/glm.hpp>

// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"


namespace Leggiero
{
	namespace LUI
	{
		// Forward Declarations
		class UIManager;
		class UIObject;

		namespace Shaders
		{
			class UIColorPrimitiveShader;
			class UITextureColorShader;
			class UITextureBlurShader;
		}


		// UI Renderer
		class UIRenderer
		{
		public:
			UIRenderer(std::shared_ptr<UIManager> manager, 
				std::shared_ptr<Shaders::UIColorPrimitiveShader> colorShader, std::shared_ptr<Shaders::UITextureColorShader> textureShader, std::shared_ptr<Shaders::UITextureBlurShader> blurShader);
			virtual ~UIRenderer();

		public:
			void Render();

		public:
			void SetScreenTransform(const glm::mat4 &projection, const glm::mat4 &view, bool isInitializeModel = true);
			void SetColorModification(float hShift = 0.0f, float sMultifly = 1.0f, float sAdjust = 0.0f, float lMultifly = 1.0f, float lAdjust = 0.0f);

		public:
			std::shared_ptr<Shaders::UIColorPrimitiveShader> GetColorSimpleShader() const { return m_colorSimpleShader; }
			std::shared_ptr<Shaders::UITextureColorShader> GetTextureColorShader() const { return m_textureColorShader; }
			std::shared_ptr<Shaders::UITextureBlurShader> GetTextureBlurShader() const { return m_textureBlurShader; }

		public:
			float GetColorHShift() const { return m_colorModifyHShift; }
			float GetColorSMultiply() const { return m_colorModifySMultiply; }
			float GetColorSAdjust() const { return m_colorModifySAdjust; }
			float GetColorLMultiply() const { return m_colorModifyLMultiply; }
			float GetColorLAdjust() const { return m_colorModifyLAdjust; }

		protected:
			void _RenderObject(std::shared_ptr<UIObject> renderingObject, const UIVector2D &parentalOffset, float effectiveAlpha);

		protected:
			std::weak_ptr<UIManager> m_renderingUIManager;

			std::shared_ptr<Shaders::UIColorPrimitiveShader> m_colorSimpleShader;
			std::shared_ptr<Shaders::UITextureColorShader> m_textureColorShader;
			std::shared_ptr<Shaders::UITextureBlurShader> m_textureBlurShader;

		protected:
			float m_colorModifyHShift;
			float m_colorModifySMultiply;
			float m_colorModifySAdjust;
			float m_colorModifyLMultiply;
			float m_colorModifyLAdjust;
		};
	}
}

#endif
