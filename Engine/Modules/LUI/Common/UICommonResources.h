////////////////////////////////////////////////////////////////////////////////
// Common/UICommonResources.h (Leggiero/Modules - LegacyUI)
//
// Object to store UI common resources
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__COMMON__UI_COMMON_RESOURCES_H
#define __LM_LUI__COMMON__UI_COMMON_RESOURCES_H


// Standard Library
#include <memory>

// Leggiero.LegacyUI
#include "UICommonTypes.h"


namespace Leggiero
{
	// Forward Declaration
	namespace Graphics
	{
		class GraphicResourceManagerComponent;
	}


	namespace LUI
	{
		// Forward Declarations
		namespace Shaders
		{
			class UIColorPrimitiveShader;
			class UITextureColorShader;
			class UITextureBlurShader;
		}


		// Common Resources Storage
		class UICommonResources
		{
		public:
			// Can be used to store user UI contents via inheritance
			virtual ~UICommonResources();

		public:
			void InitializeDefaultUIObjects(Graphics::GraphicResourceManagerComponent *resourceManager);

		public:
			std::shared_ptr<Shaders::UIColorPrimitiveShader> m_uiColorShader;
			std::shared_ptr<Shaders::UITextureColorShader> m_uiTextureShader;
			std::shared_ptr<Shaders::UITextureBlurShader> m_uiBlurShader;
		};
	}
}

#endif
