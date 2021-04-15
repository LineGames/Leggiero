////////////////////////////////////////////////////////////////////////////////
// Rendering/UIRenderer.h (Leggiero/Modules - LegacyUI)
//
// UI Renderer Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIRenderer.h"

// External Library
#include <glm/gtc/type_ptr.hpp>

// Leggiero.LegacyUI
#include "../UIManager.h"
#include "../UIObject.h"
#include "../Component/UIComponentType.h"
#include "../Component/IUIComponent.h"
#include "../Component/UILayoutComponent.h"
#include "UIRenderingComponent.h"
#include "UIShaders.h"
#include "UIClipping.h"


namespace Leggiero
{
	namespace LUI
	{
		//////////////////////////////////////////////////////////////////////////////// UIRenderer

		//------------------------------------------------------------------------------
		UIRenderer::UIRenderer(std::shared_ptr<UIManager> manager, std::shared_ptr<Shaders::UIColorPrimitiveShader> colorShader, std::shared_ptr<Shaders::UITextureColorShader> textureShader, std::shared_ptr<Shaders::UITextureBlurShader> blurShader)
			: m_renderingUIManager(manager)
			, m_colorSimpleShader(colorShader), m_textureColorShader(textureShader), m_textureBlurShader(blurShader)
		{
		}

		//------------------------------------------------------------------------------
		UIRenderer::~UIRenderer()
		{
		}

		//------------------------------------------------------------------------------
		// Set Screen Transform before Rendering
		void UIRenderer::SetScreenTransform(const glm::mat4 &projection, const glm::mat4 &view, bool isInitializeModel)
		{
			static glm::mat4 identityMatrix(1.0f);

			if (m_colorSimpleShader)
			{
				m_colorSimpleShader->Use();
				glUniformMatrix4fv(m_colorSimpleShader->GetProjectionMatrixLocation(), 1, GL_FALSE, glm::value_ptr(projection));
				glUniformMatrix4fv(m_colorSimpleShader->GetViewMatrixLocation(), 1, GL_FALSE, glm::value_ptr(view));
				if (isInitializeModel)
				{
					glUniformMatrix4fv(m_colorSimpleShader->GetModelMatrixLocation(), 1, GL_FALSE, glm::value_ptr(identityMatrix));
				}
			}
			if (m_textureColorShader)
			{
				m_textureColorShader->Use();
				glUniformMatrix4fv(m_textureColorShader->GetProjectionMatrixLocation(), 1, GL_FALSE, glm::value_ptr(projection));
				glUniformMatrix4fv(m_textureColorShader->GetViewMatrixLocation(), 1, GL_FALSE, glm::value_ptr(view));
				if (isInitializeModel)
				{
					glUniformMatrix4fv(m_textureColorShader->GetModelMatrixLocation(), 1, GL_FALSE, glm::value_ptr(identityMatrix));
				}
			}
			if (m_textureBlurShader)
			{
				m_textureBlurShader->Use();
				glUniformMatrix4fv(m_textureBlurShader->GetProjectionMatrixLocation(), 1, GL_FALSE, glm::value_ptr(projection));
				glUniformMatrix4fv(m_textureBlurShader->GetViewMatrixLocation(), 1, GL_FALSE, glm::value_ptr(view));
				if (isInitializeModel)
				{
					glUniformMatrix4fv(m_textureBlurShader->GetModelMatrixLocation(), 1, GL_FALSE, glm::value_ptr(identityMatrix));
				}
			}
		}

		//------------------------------------------------------------------------------
		// Set color modification state for UI Renderer
		void UIRenderer::SetColorModification(float hShift, float sMultifly, float sAdjust, float lMultifly, float lAdjust)
		{
			float hslModifyVector[5] = { hShift, sMultifly, sAdjust, lMultifly, lAdjust };

			m_colorModifyHShift = hShift;
			m_colorModifySMultiply = sMultifly;
			m_colorModifySAdjust = sAdjust;
			m_colorModifyLMultiply = lMultifly;
			m_colorModifyLAdjust = lAdjust;

			if (m_colorSimpleShader)
			{
				m_colorSimpleShader->Use();
				glUniform1fv(m_colorSimpleShader->GetHSLModificationLocation(), 5, hslModifyVector);
			}
			if (m_textureColorShader)
			{
				m_textureColorShader->Use();
				glUniform1fv(m_textureColorShader->GetHSLModificationLocation(), 5, hslModifyVector);
			}
			if (m_textureBlurShader)
			{
				m_textureBlurShader->Use();
				glUniform1fv(m_textureBlurShader->GetHSLModificationLocation(), 5, hslModifyVector);
			}
		}

		//------------------------------------------------------------------------------
		// Do Render a Frame
		void UIRenderer::Render()
		{
			UIVector2D globalOffset(static_cast<UICoordinateType>(0.0f), static_cast<UICoordinateType>(0.0f));

			std::shared_ptr<UIManager> managerCopy(m_renderingUIManager.lock());
			if (!managerCopy)
			{
				return;
			}

			_RenderObject(managerCopy->m_root, UIVector2D::kZero, 1.0f);
		}

		//------------------------------------------------------------------------------
		// Render Target Object
		void UIRenderer::_RenderObject(std::shared_ptr<UIObject> renderingObject, const UIVector2D &parentalOffset, float effectiveAlpha)
		{
			if (!renderingObject || !renderingObject->IsVisible())
			{
				return;
			}

			std::shared_ptr<UIRenderingComponent> renderingComponent = renderingObject->GetComponentObject<UIRenderingComponent>(UIComponentType::kRendering);
			std::shared_ptr<CalculatedUILayoutInformation> cachedLayoutInfo(renderingObject->GetCachedLayoutInformation());
			if (!cachedLayoutInfo || (!cachedLayoutInfo->isOffsetCalculated))
			{
				return;
			}

			if (cachedLayoutInfo->applyingClipping->IsAllClipping())
			{
				return;
			}

			UIVector2D objectOffset(parentalOffset.x + cachedLayoutInfo->parentalOffsetX, parentalOffset.y + cachedLayoutInfo->parentalOffsetY);

			float objectAlpha = effectiveAlpha * renderingObject->GetAlpha();

			renderingObject->FrameGraphicPrepare(*this, objectOffset, *(cachedLayoutInfo->applyingClipping));

			for (std::shared_ptr<UIObject> &currentObject : renderingObject->m_preChildren)
			{
				_RenderObject(currentObject, objectOffset, objectAlpha);
			}

			if (renderingComponent)
			{
				renderingComponent->Render(*this, *(cachedLayoutInfo->applyingTransform), *(cachedLayoutInfo->applyingClipping), objectAlpha);
			}

			for (std::shared_ptr<UIObject> &currentObject : renderingObject->m_postChildren)
			{
				_RenderObject(currentObject, objectOffset, objectAlpha);
			}
		}
	}
}
