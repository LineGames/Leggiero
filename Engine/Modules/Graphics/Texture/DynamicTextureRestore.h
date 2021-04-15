////////////////////////////////////////////////////////////////////////////////
// Texture/DynamicTextureRestore.h (Leggiero/Modules - Graphics)
//
// Defines related to Restoration of Dynmaic Texture
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_GRAPHICS__TEXTURE__DYNAMIC_TEXTURE_RESTORE_H
#define __LM_GRAPHICS__TEXTURE__DYNAMIC_TEXTURE_RESTORE_H


// Leggiero.Basics
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <functional>
#include <memory>


namespace Leggiero
{
	namespace Graphics
	{
		// Forward Declaration of Dynamic Texture
		class DynamicTextureResource;


		// Delegate for Dynamic Texture Restore Event
		using DynamicTextureRestoreNotifyDelegate = std::function<void(std::shared_ptr<DynamicTextureResource>)>;


		namespace PredefinedDynamicTextureNotifier
		{
			// Non-notifying dummy delegate
			extern const DynamicTextureRestoreNotifyDelegate kNonNotifyingDynamicTextureRestoreNotifier;
		}
	}
}

#endif
