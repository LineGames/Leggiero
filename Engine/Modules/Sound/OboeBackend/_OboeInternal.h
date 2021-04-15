////////////////////////////////////////////////////////////////////////////////
// OboeBackend/_OboeInternal.h (Leggiero/Modules - Sound)
//
// Internal utilities used in oboe backend
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__OBOE_BACKEND___OBOE_INTERNAL_H
#define __LM_SOUND__OBOE_BACKEND___OBOE_INTERNAL_H


// Standard Library
#include <atomic>
#include <cstdint>
#include <vector>
#include <memory>

// External Library
#include <oboe/Oboe.h>

// Leggiero.Sound
#include "../Common/SoundTypes.h"


namespace Leggiero
{
	namespace Sound
	{
		namespace Oboe
		{
			// Forward Declaration
			class IOboeStreamRendering;


			namespace _Internal
			{
				class RenderingQueueBuffer
				{
				public:
					RenderingQueueBuffer();

				public:
					std::atomic<uint64_t>								queueVersion;
					std::vector<std::shared_ptr<IOboeStreamRendering> > renderingQueue;
				};
			}
		}
	}
}

#endif
