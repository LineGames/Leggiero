////////////////////////////////////////////////////////////////////////////////
// OboeBackend/OboePlatformCompat.cpp (Leggiero/Modules - Sound)
//
// Dummy implementation for compatibility
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "OboeCommon.h"


namespace Leggiero
{
	namespace Sound
	{
		namespace Oboe
		{
			//------------------------------------------------------------------------------
			// Default values for optimal setting
			void SetDefaultValuesForStream(int32_t defaultSampleRate, int32_t defaultFramesPerBurst)
			{
			}

			//------------------------------------------------------------------------------
			// Data Buffer Default Size
			size_t GetDataBufferDefaultSize()
			{
				return 0;
			}

			//------------------------------------------------------------------------------
			// Set Preparing Sampling Rates
			void SetPreparingFrequencies(std::vector<SamplingFrequencyType> preparingFrequencies)
			{
			}

			//------------------------------------------------------------------------------
			// Set Preparing Sampling Rates
			void SetPreparingFrequencies(std::initializer_list<SamplingFrequencyType> preparingFrequencies)
			{
			}

			//------------------------------------------------------------------------------
			// Get Preparing Sampling Rates
			const std::vector<SamplingFrequencyType> &GetPreparingFrequencies()
			{
				const static std::vector<SamplingFrequencyType> dummyBuffer;
				return dummyBuffer;
			}
		}
	}
}
