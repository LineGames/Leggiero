////////////////////////////////////////////////////////////////////////////////
// Component/UIComponentType.h (Leggiero/Modules - LegacyUI)
//
// UI Component Type Definition
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__COMPONENT__UI_COMPONENT_TYPE_H
#define __LM_LUI__COMPONENT__UI_COMPONENT_TYPE_H


// Leggiero.Utility
#include <Utility/Sugar/EnumClass.h>


namespace Leggiero
{
	namespace LUI
	{
		// Type of UI Component
		enum class UIComponentType
		{
			kINVALID,

			kFabricated,

			kLayout,
			kTransform,
			kClipping,

			kTouchInteraction,

			kRendering,
		};
	}
}

#endif
