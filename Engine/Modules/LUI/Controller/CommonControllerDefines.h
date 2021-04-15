////////////////////////////////////////////////////////////////////////////////
// Controller/CommonControllerDefines.h (Leggiero/Modules - LegacyUI)
//
// Common value definitions for LUI controllers
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__CONTROLLER__COMMON_CONTROLLER_DEFINES_H
#define __LM_LUI__CONTROLLER__COMMON_CONTROLLER_DEFINES_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"
#include "../Prefab/UIFabricatedComponent.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Controller
		{
			// Common Sub-object Name Definitions
			namespace CommonSubObjectNames
			{
				extern const UIFabricatedComponent::ObjectDescriptionIdType kInactiveBG;
				extern const UIFabricatedComponent::ObjectDescriptionIdType kNormalBG;
				extern const UIFabricatedComponent::ObjectDescriptionIdType kPushBG;
				extern const UIFabricatedComponent::ObjectDescriptionIdType kActiveBG;
				extern const UIFabricatedComponent::ObjectDescriptionIdType kActiveNormalBG;
				extern const UIFabricatedComponent::ObjectDescriptionIdType kActivePushBG;
				extern const UIFabricatedComponent::ObjectDescriptionIdType kDisableBG;

				extern const UIFabricatedComponent::ObjectDescriptionIdType kInactiveContents;
				extern const UIFabricatedComponent::ObjectDescriptionIdType kNormalContents;
				extern const UIFabricatedComponent::ObjectDescriptionIdType kPushContents;
				extern const UIFabricatedComponent::ObjectDescriptionIdType kActiveContents;
				extern const UIFabricatedComponent::ObjectDescriptionIdType kActiveNormalContents;
				extern const UIFabricatedComponent::ObjectDescriptionIdType kActivePushContents;
				extern const UIFabricatedComponent::ObjectDescriptionIdType kDisableContents;
			}
		}
	}
}

#endif
