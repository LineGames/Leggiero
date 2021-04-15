////////////////////////////////////////////////////////////////////////////////
// Common/UISizeSettable.h (Leggiero/Modules - LegacyUI)
//
// Size settable interface of UI things
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__COMMON__UI_SIZE_SETTABLE_H
#define __LM_LUI__COMMON__UI_SIZE_SETTABLE_H


// Leggiero.LegacyUI
#include "UICommonTypes.h"


namespace Leggiero
{
	namespace LUI
	{
		// Rectangular size settable interface
		class IRectSizeSettable
		{
		public:
			virtual void SetUIWidgetRectSize(UICoordinateType width, UICoordinateType height) = 0;
		};
	}
}

#endif
