////////////////////////////////////////////////////////////////////////////////
// Common/IUIArea.h (Leggiero/Modules - LegacyUI)
//
// Interface for an area in UI System
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__COMMON__I_UI_AREA_H
#define __LM_LUI__COMMON__I_UI_AREA_H


// Standard Library
#include <memory>

// Leggiero.LegacyUI
#include "UICommonTypes.h"


namespace Leggiero
{
	namespace LUI
	{
		// UI Area Interface
		class IUIArea
		{
		public:
			IUIArea() { }
			virtual ~IUIArea() { }

		public:
			virtual bool IsInArea(UICoordinateType x, UICoordinateType y) const = 0;
			virtual bool IsInArea(const UIVector2D &position) const { return IsInArea(position.x, position.y); }

		public:
			static std::shared_ptr<IUIArea> kNoArea;
			static std::shared_ptr<IUIArea> kEverywhereArea;
		};
	}
}

#endif
