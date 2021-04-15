////////////////////////////////////////////////////////////////////////////////
// Common/UICommonResources.cpp (Leggiero/Modules - LegacyUI)
//
// Common Resource Base Implementations
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UICommonResources.h"

// Leggiero.LegacyUI
#include "../Rendering/UIShaders.h"


namespace Leggiero
{
	namespace LUI
	{
		//////////////////////////////////////////////////////////////////////////////// UICommonResources

		//------------------------------------------------------------------------------
		UICommonResources::~UICommonResources()
		{
		}

		//------------------------------------------------------------------------------
		void UICommonResources::InitializeDefaultUIObjects(Graphics::GraphicResourceManagerComponent *resourceManager)
		{
			m_uiColorShader = Shaders::UIColorPrimitiveShader::Create(resourceManager);
			m_uiTextureShader = Shaders::UITextureColorShader::Create(resourceManager);
			m_uiBlurShader = Shaders::UITextureBlurShader::Create(resourceManager);
		}
	}
}
