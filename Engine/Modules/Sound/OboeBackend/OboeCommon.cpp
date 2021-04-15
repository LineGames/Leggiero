////////////////////////////////////////////////////////////////////////////////
// OboeBackend/OboeCommon.cpp (Leggiero/Modules - Sound)
//
// Oboe Common Utility Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "OboeCommon.h"

// Standard Library
#include <set>

// External Library
#include <oboe/Oboe.h>


namespace Leggiero
{
	namespace Sound
	{
		namespace Oboe
		{
			//------------------------------------------------------------------------------
			namespace _Internal
			{
				std::vector<SamplingFrequencyType> g_preparingFrequencies{ 44100, 48000 };
			}

			//------------------------------------------------------------------------------
			// Default values for optimal setting
			void SetDefaultValuesForStream(int32_t defaultSampleRate, int32_t defaultFramesPerBurst)
			{
				oboe::DefaultStreamValues::SampleRate = defaultSampleRate;
				oboe::DefaultStreamValues::FramesPerBurst = defaultFramesPerBurst;
			}

			//------------------------------------------------------------------------------
			// Set Preparing Sampling Rates
			void SetPreparingFrequencies(std::vector<SamplingFrequencyType> preparingFrequencies)
			{
				_Internal::g_preparingFrequencies.clear();
				_Internal::g_preparingFrequencies.insert(_Internal::g_preparingFrequencies.end(), preparingFrequencies.begin(), preparingFrequencies.end());
			}

			//------------------------------------------------------------------------------
			// Set Preparing Sampling Rates
			void SetPreparingFrequencies(std::initializer_list<SamplingFrequencyType> preparingFrequencies)
			{
				std::set<SamplingFrequencyType> preparingFrequenciesSet(preparingFrequencies);
				_Internal::g_preparingFrequencies.clear();
				_Internal::g_preparingFrequencies.insert(_Internal::g_preparingFrequencies.end(), preparingFrequenciesSet.begin(), preparingFrequenciesSet.end());
			}

			//------------------------------------------------------------------------------
			// Get Preparing Sampling Rates
			const std::vector<SamplingFrequencyType> &GetPreparingFrequencies()
			{
				return _Internal::g_preparingFrequencies;
			}
		}
	}
}
