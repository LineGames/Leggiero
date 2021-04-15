////////////////////////////////////////////////////////////////////////////////
// OboeBackend/OboeCommon.h (Leggiero/Modules - Sound)
//
// Common Utility for Oboe Backend
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__OBOE_BACKEND__OBOE_COMMON_H
#define __LM_SOUND__OBOE_BACKEND__OBOE_COMMON_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <cstdint>
#include <initializer_list>
#include <vector>

// Leggiero.Sound
#include "../Common/SoundTypes.h"


namespace Leggiero
{
	namespace Sound
	{
		namespace Oboe
		{
			// Default values for optimal setting
			void SetDefaultValuesForStream(int32_t defaultSampleRate, int32_t defaultFramesPerBurst);

			// Data Buffer Type
			using DataBufferType = std::vector<uint8_t>;

			// Data Buffer Default Size
			size_t GetDataBufferDefaultSize();

			// Set Preparing Sampling Rates
			void SetPreparingFrequencies(std::vector<SamplingFrequencyType> preparingFrequencies);
			void SetPreparingFrequencies(std::initializer_list<SamplingFrequencyType> preparingFrequencies);

			// Get Preparing Sampling Rates
			const std::vector<SamplingFrequencyType> &GetPreparingFrequencies();
		}
	}
}

#endif
