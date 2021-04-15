////////////////////////////////////////////////////////////////////////////////
// Provider/OggSoundProvider.h (Leggiero/Modules - Sound)
//
// Sound provider for Ogg Vorbis format data
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__PROVIDER__OGG_SOUND_PROVIDER_H
#define __LM_SOUND__PROVIDER__OGG_SOUND_PROVIDER_H


// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// Standard Library
#include <cstdio>
#include <istream>
#include <memory>
#include <string>

// External Library
#include <vorbis/vorbisfile.h>

// Leggiero.Utility
#include <Utility/Threading/ManagedThreadPrimitives.h>

// Leggiero.Sound
#include "ISoundProvider.h"


namespace Leggiero
{
	namespace Sound
	{
		// Forward Declaration
		namespace _Internal
		{
			struct OGGRawMemoryBuffer;
		}


		// Ogg Vorbis Sound Data Provider
		class OggSoundProvider : public ISoundProvider
		{
		public:
			OggSoundProvider(FILE *source, bool needToClose = false);
			OggSoundProvider(const char *filePath);
			OggSoundProvider(const std::string &filePath);
			OggSoundProvider(std::istream *source);
			OggSoundProvider(std::shared_ptr<std::istream> source);
			OggSoundProvider(const void *memoryBuffer, size_t bufferSize);
			virtual ~OggSoundProvider();

		protected:
			void _InitializeSoundData();

		private:
			// Prevent Creating of NULL Sound Provider
			OggSoundProvider() { }

		public:	// ISoundProvider
			virtual SampleFormat			GetFormat() const override { return m_format; }
			virtual SamplingFrequencyType	GetFrequency() const override { return m_frequency; }

			virtual size_t	GetSampleLength() const override { return (size_t)m_lengthInSample; }
			virtual float	GetLengthInSecond() const override { return (float)m_lengthInSecond; }

			virtual size_t	FillSampleData(void *dataBuffer, size_t bufferSize, SampleNumberType startSample, SampleNumberType *outWrittenSamples = nullptr) override;

		public:
			bool IsValid() const { return m_isInitialized; }

		protected:
			bool	m_isInitialized;

			SampleFormat			m_format;
			SamplingFrequencyType	m_frequency;
			int						m_channelNumber;

			double		m_lengthInSecond;
			ogg_int64_t m_lengthInSample;

			FILE *m_ownFileSource;

			std::shared_ptr<std::istream> m_sharedIStreamPtr;

			std::shared_ptr<_Internal::OGGRawMemoryBuffer> m_bufferSharedPtr;

			OggVorbis_File	m_sourceVorbisFile;
			Utility::Threading::SafePthreadLock m_sourceFileMutex;

			int m_lastBitstream;

			SampleNumberType m_lastReadSamplePosition;
		};
	}
}

#endif
