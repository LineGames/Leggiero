////////////////////////////////////////////////////////////////////////////////
// Touch/UITouchComponent.cpp (Leggiero/Modules - LegacyUI)
//
// UI Touch Component Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UITouchComponent.h"

// Leggiero.LegacyUI
#include "../Common/IUIArea.h"


namespace Leggiero
{
	namespace LUI
	{
		//////////////////////////////////////////////////////////////////////////////// UITouchComponent

		//------------------------------------------------------------------------------
		std::shared_ptr<IUIArea> UITouchComponent::GetInteractionArea() const
		{
			return IUIArea::kNoArea;
		}
	}
}
