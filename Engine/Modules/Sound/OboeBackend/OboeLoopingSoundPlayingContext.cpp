////////////////////////////////////////////////////////////////////////////////
// OboeBackend/OboeLoopingSoundPlayingContext.cpp (Leggiero/Modules - Sound)
//
// Looping Playing Context Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "OboeLoopingSoundPlayingContext.h"

// Leggiero.Sound
#include "../Provider/LoopHoldSoundProvider.h"


namespace Leggiero
{
	namespace Sound
	{
		namespace Oboe
		{
			//////////////////////////////////////////////////////////////////////////////// OboeLoopingSoundPlayingContext

			//------------------------------------------------------------------------------
			OboeLoopingSoundPlayingContext::OboeLoopingSoundPlayingContext(std::shared_ptr<ISoundProvider> sourceSoundProvider, std::shared_ptr<Utility::Object::PointerHolder> mixerHolder,
				SampleNumberType loopSectionStart, SampleNumberType loopSectionFinish,
				bool isStartImmediately, float volume)
				: OboeSoundPlayingContext(std::make_shared<LoopHoldSoundProvider>(sourceSoundProvider, loopSectionStart, loopSectionFinish, true), mixerHolder, isStartImmediately, volume, false)
				, m_isLooping(true)
			{
			}

			//------------------------------------------------------------------------------
			// Stop sound looping (play is not stopped)
			void OboeLoopingSoundPlayingContext::StopLooping()
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
