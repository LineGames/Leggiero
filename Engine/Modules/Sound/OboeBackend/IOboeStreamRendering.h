////////////////////////////////////////////////////////////////////////////////
// OboeBackend/IOboeStreamRendering.h (Leggiero/Modules - Sound)
//
// Oboe Stream Rendering Interface
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__OBOE_BACKEND__I_OBOE_STREAM_RENDERING_H
#define __LM_SOUND__OBOE_BACKEND__I_OBOE_STREAM_RENDERING_H


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
			// Stream Rendering Sound Object
			class IOboeStreamRendering
			{
			public:
				virtual ~IOboeStreamRendering() { }

			public:
				virtual bool IsHighPriorityObject() const { return false; }
				virtual bool RenderAudio(void *audioData, int32_t numFrames, int32_t streamDataFrameBytes, oboe::AudioFormat streamDataFormat, int32_t streamChannelCount) = 0;

				virtual bool IsStoppedRendering() = 0;
				virtual bool IsPausedRendering() = 0;
			};
		}
	}
}

#endif
