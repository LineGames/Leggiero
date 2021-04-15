////////////////////////////////////////////////////////////////////////////////
// OpenALBackend/OpenALLoopingSoundPlayingContext.cpp (Leggiero/Modules - Sound)
//
// Looping Playing Context Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "OpenALLoopingSoundPlayingContext.h"

// Leggiero.Sound
#include "../Provider/LoopHoldSoundProvider.h"


namespace Leggiero
{
	namespace Sound
	{
		namespace OpenAL
		{
			//////////////////////////////////////////////////////////////////////////////// OpenALLoopingSoundPlayingContext

			//------------------------------------------------------------------------------
			OpenALLoopingSoundPlayingContext::OpenALLoopingSoundPlayingContext(std::shared_ptr<ISoundProvider> sourceSoundProvider, std::shared_ptr<Utility::Object::PointerHolder> mixerHolder, ALuint source, ALuint buffers[], size_t bufferCount,
				SampleNumberType loopSectionStart, SampleNumberType loopSectionFinish,
				bool isStartImmediately, float volume)
				: OpenALSoundPlayingContext(std::make_shared<LoopHoldSoundProvider>(sourceSoundProvider, loopSectionStart, loopSectionFinish, true), mixerHolder, source, buffers, bufferCount, isStartImmediately, volume, false)
				, m_isLooping(true)
			{
			}

			//------------------------------------------------------------------------------
			// Stop sound looping (play is not stopped)
			void OpenALLoopingSoundPlayingContext::StopLooping()
			{
				if (!m_isLooping)
				{
					return;
				}

				m_isLooping = false;

				// Clamp Current Position
				if (!m_soundProvider)
				{
					return;
				}
				std::shared_ptr<LoopHoldSoundProvider> pSoundProvider = std::dynamic_pointer_cast<LoopHoldSoundProvider>(m_soundProvider);
				if (!pSoundProvider)
				{
					return;
				}

				SampleNumberType loopingIntervalStart = pSoundProvider->GetLoopStartSample();
				SampleNumberType loopingIntervalFinish = pSoundProvider->GetLoopFinishSample();

				pSoundProvider->SetLooping(false);

				if (loopingIntervalStart >= loopingIntervalFinish)
				{
					return;
				}

				if (m_lastSampleNumber >= loopingIntervalFinish)
				{
					m_lastSampleNumber = loopingIntervalStart + ((m_lastSampleNumber - loopingIntervalStart) % (loopingIntervalFinish - loopingIntervalStart));
				}
			}
		}
	}
}
